#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

status_t get_tkpt_ubx(arg_s *, data_structs_s *);
status_t search_sync(FILE * f);
status_t load_tim_tos (arg_s *, tim_tos_s *, size_t, uchar, uchar);
status_t load_nav_pvt(arg_s *, nav_pvt_s *, size_t , uchar, uchar);
status_t load_nav_posllh(arg_s *, nav_posllh_s *, size_t, uchar, uchar);
status_t tim_tos2tkpt(data_structs_s *);
status_t nav_pvt2tkpt(data_structs_s *);
status_t nav_posllh2tkpt(data_structs_s *);
status_t get_length(FILE *, size_t *, uchar *, uchar *);
status_t cksm_double(uchar * cksm1, uchar * cksm2, double aux_d);
status_t read_unsigned_4(FILE * f, uchar * cksm1, uchar * cksm2, unsigned int * val);
status_t read_signed_4(FILE * f, uchar * cksm1, uchar * cksm2, long * val);

#define IGNORE_BYTES_TIM_TOS_1 6
#define IGNORE_BYTES_TIM_TOS_2 9
#define IGNORE_BYTES_TIM_TOS_3 24

#define IGNORE_BYTES_PVT_1 8
#define IGNORE_BYTES_PVT_2 1
#define IGNORE_BYTES_PVT_3 36
#define IGNORE_BYTES_PVT_4 6
#define IGNORE_BYTES_PVT_5 8

#define CONV_LTL_ENDN_2 256
#define CONV_LTL_ENDN_3 65536
#define CONV_LTL_ENDN_4 16777216

#define MASK_VALID_DATE 0x1
#define SHIFT_VALID_DATE 0
#define MASK_VALID_TIME 0x2
#define SHIFT_VALID_TIME 1
#define MASK_VALID_FIX 0x1
#define SHIFT_VALID_FIX 0

#define PAYLOAD_TIM_TOS 56
#define PAYLOAD_NAV_PVT 92
#define PAYLOAD_NAV_POSLLH 28

#define CLASS_NAV 0x01
#define CLASS_TIM_TOS 0x0D

#define ID_TIM_TOS 0x12
#define ID_NAV_PVT 0x07
#define ID_NAV_POSLLH 0x02

#define SYNC_1 0xB5
#define SYNC_2 0x62

#define MASK_DOUBLE_B0 0x000000FF
#define MASK_DOUBLE_B1 0x0000FF00
#define MASK_DOUBLE_B2 0x00FF0000
#define MASK_DOUBLE_B3 0xFF000000

#define SHIFT_DOUBLE_B0 0
#define SHIFT_DOUBLE_B1 8
#define SHIFT_DOUBLE_B2 16
#define SHIFT_DOUBLE_B3 24

#define LONGITUDE_SCALE 10000000
#define LATITUDE_SCALE 10000000


status_t get_tkpt_ubx(arg_s * metadata, data_structs_s * data_structs) {

	FILE * f = NULL;
    uchar aux, cksm1 = 0, cksm2 = 0;
    status_t st;
    size_t l;


	if(! data_structs || ! metadata)
	    return ST_NULL_PTR;
	
	f = metadata->infile;

	if(feof(f))
	    return ST_ERROR_EOF;

    if((st = search_sync(f)) != ST_OK)
    	return st;

    if(fread(&aux, sizeof(uchar), 1, f) != 1)
    	return ST_CORRUPT_FILE;

    cksm1 += aux;
    cksm2 += cksm1;

    if(aux == CLASS_TIM_TOS) {

    	if(fread(&aux, sizeof(uchar), 1, f) != 1)
    		return ST_CORRUPT_FILE;

        cksm1 += aux;
    	cksm2 += cksm1;

    	if(aux != ID_TIM_TOS)
    		log_print(metadata->logfile, UBX_ID);

    	if((st = get_length(f, &l, &cksm1, &cksm2)) != ST_OK)
    		return st;

    	if(l != PAYLOAD_TIM_TOS) {
    		log_print(metadata->logfile, UBX_LONG);
    		return ST_IGNORE_STATEMENT;
    	}

    	if((st = load_tim_tos(metadata, data_structs->tim_tos, l, cksm1, cksm2)) != ST_OK)
    		return st;

    	if((st = tim_tos2tkpt(data_structs)) != ST_OK)
    		return st;

    	data_structs->tkpt->time_flag = TRUE;
    	log_print(metadata->logfile, TIME_UPDATED);
    	log_print(metadata->logfile, DATE_UPDATED);

    	return get_tkpt_ubx(metadata, data_structs);
    } 

    if (aux == CLASS_NAV) {

    	if(fread(&aux, sizeof(uchar), 1, f) != 1) // Lee el ID
    		return ST_CORRUPT_FILE;

    	cksm1 += aux;
    	cksm2 += cksm1;

    	if(aux == ID_NAV_PVT) {

	   		if((st = get_length(f, &l, &cksm1, &cksm2)) != ST_OK)
				return st;

			if(l != PAYLOAD_NAV_PVT) {
				log_print(metadata->logfile, UBX_LONG);
				return ST_IGNORE_STATEMENT;
			}


			if((st = load_nav_pvt(metadata, data_structs->nav_pvt, l, cksm1, cksm2)) != ST_OK)
				return st;

			if((st = nav_pvt2tkpt(data_structs)) != ST_OK)
				return st;

			data_structs->tkpt->time_flag = TRUE;
    		log_print(metadata->logfile, TIME_UPDATED);
    		log_print(metadata->logfile, DATE_UPDATED);
    		log_print(metadata->logfile, TKPT_GEN);
puts("tkpt-nav-pvt");
    		return ST_OK;
    	} 

    	if( aux == ID_NAV_POSLLH) {

    		if((st = get_length(f, &l, &cksm1, &cksm2)) != ST_OK)
				return st;

			if(l != PAYLOAD_NAV_POSLLH) {
				log_print(metadata->logfile, UBX_LONG);
				return ST_IGNORE_STATEMENT;
			}

			if((st = load_nav_posllh(metadata, data_structs->nav_posllh, l, cksm1, cksm2)) != ST_OK)
				return st;

			if((st = nav_posllh2tkpt(data_structs)) != ST_OK)
				return st;

			if(data_structs->tkpt->time_flag == TRUE) {
				puts("tkpt-nav-pos-llh");
				log_print(metadata->infile, TKPT_GEN);
				return ST_OK;
			}

			log_print(metadata->logfile, MISSING_DATE_TIME);
			return ST_IGNORE_STATEMENT;
    	}

    	if((st = get_length(f, &l, &cksm1, &cksm2)) != ST_OK)
    		return st;

    	if(l == PAYLOAD_NAV_PVT) {

    		if((st = load_nav_pvt(metadata, data_structs->nav_pvt, l, cksm1, cksm2)) != ST_OK)
				return st;

			if((st = nav_pvt2tkpt(data_structs)) != ST_OK)
				return st;

			data_structs->tkpt->time_flag = TRUE;
    		log_print(metadata->logfile, TIME_UPDATED);
    		log_print(metadata->logfile, DATE_UPDATED);
    		log_print(metadata->logfile, TKPT_GEN);
puts("tkpt-nav-pvt");
    		return ST_OK;
    	}

    	if(l == PAYLOAD_NAV_POSLLH) {

    		if((st = load_nav_posllh(metadata, data_structs->nav_posllh, l, cksm1, cksm2)) != ST_OK)
				return st;

			if((st = nav_posllh2tkpt(data_structs)) != ST_OK)
				return st;

			if(data_structs->tkpt->time_flag == TRUE) {
				puts("tkpt-nav-pos-llh");
				log_print(metadata->infile, TKPT_GEN);
				return ST_OK;
			}

			log_print(metadata->logfile, MISSING_DATE_TIME);
			return ST_IGNORE_STATEMENT;
    	}

    	log_print(metadata->logfile, UBX_LONG);
		return ST_IGNORE_STATEMENT;
    }

    log_print(metadata->logfile, INCORRECT_STATEMENT);
    return ST_IGNORE_STATEMENT;
}


status_t search_sync(FILE * f) {

	int aux = 0;

	if(! f)
		return ST_NULL_PTR;

	do{
		while(fread(&aux, sizeof(uchar), 1, f) == 1 && aux != SYNC_1);
	} while(fread(&aux, sizeof(uchar), 1, f) == 1 && aux != SYNC_2);

	if(aux == EOF)
		return ST_ERROR_EOF;

	if(aux == SYNC_2)
		return ST_OK;

	return ST_CORRUPT_FILE;
}


status_t load_tim_tos(arg_s * metadata, tim_tos_s * tim_tos, size_t l, uchar cksm1, uchar cksm2) {
puts("tim_tos");	
	FILE * f = NULL;
	size_t i;
	uchar aux, aux2;


	if(! tim_tos || ! metadata)
	    return ST_NULL_PTR;

	f = metadata->infile;
		
	if(fread(&aux, sizeof(uchar), 1, f) != 1) //Lee version y lo guarda en la estructura tim_tos
	    return ST_CORRUPT_FILE;
	
	cksm1 += aux;
	cksm2 += cksm1;
	tim_tos->version = (uint)aux;

	if(fread(&aux, sizeof(uchar), 1, f) != 1) //Lee GNSS y lo guarda en la estructura tim_tos  ***DEFINIR GNSS EN LA ESTRUCTURA***
	    return ST_CORRUPT_FILE;

	cksm1 += aux;
	cksm2 += cksm1;
	tim_tos->GNSS = (uint)aux;
	
	for(i = 0; i < IGNORE_BYTES_TIM_TOS_1; i++) {

		if(fread(&aux, sizeof(uchar), 1, f) != 1)
			return ST_CORRUPT_FILE;

		cksm1 += aux;
		cksm2 += cksm1;

	}

	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
	
	cksm1 += aux;
	cksm2 += cksm1;
	aux2 = aux;
	
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
	
	cksm1 += aux;
	cksm2 += cksm1;
	aux2 += CONV_LTL_ENDN_2 * aux;
		
	tim_tos->tim_time.tm_year = (int)aux2;
	
	if(fread(&aux, sizeof(uchar), 1, f) != 1) //Lee mes y lo guarda en la estructura tim_tos
	    return ST_CORRUPT_FILE;

	cksm1 += aux;
	cksm2 += cksm1;
	tim_tos->tim_time.tm_mon = (int)aux;
		
	if(fread(&aux, sizeof(uchar), 1, f) != 1) //Lee dia y lo guarda en la estructura tim_tos
	    return ST_CORRUPT_FILE;

	cksm1 += aux;
	cksm2 += cksm1;
	tim_tos->tim_time.tm_mday = (int)aux;
		
	if(fread(&aux, sizeof(uchar), 1, f) != 1) //Lee hora y lo guarda en la estructura tim_tos
	    return ST_CORRUPT_FILE;

	cksm1 += aux;
	cksm2 += cksm1;
	tim_tos->tim_time.tm_hour = (int)aux;
		
	if(fread(&aux, sizeof(uchar), 1, f) != 1) //Lee hora y lo guarda en la estructura tim_tos
	    return ST_CORRUPT_FILE;

	cksm1 += aux;
	cksm2 += cksm1;
	tim_tos->tim_time.tm_min = (int)aux;
		
	if(fread(&aux, sizeof(uchar), 1, f) != 1) //Lee hora y lo guarda en la estructura tim_tos
	    return ST_CORRUPT_FILE;

	cksm1 += aux;
	cksm2 += cksm1;
	tim_tos->tim_time.tm_sec = (int)aux;
		
	for(i = 0; i < IGNORE_BYTES_TIM_TOS_2; i++) {

		if(fread(&aux, sizeof(uchar), 1, f) != 1)
			return ST_CORRUPT_FILE;

		cksm1 += aux;
		cksm2 += cksm1;

	} 
		
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
	
	cksm1 += aux;
	cksm2 += cksm1;
	aux2 = aux;
	
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
	
	cksm1 += aux;
	cksm2 += cksm1;
	aux2 += CONV_LTL_ENDN_2 * aux;

	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
	
	cksm1 += aux;
	cksm2 += cksm1;
	aux2 = CONV_LTL_ENDN_3 * aux;
	
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
	
	cksm1 += aux;
	cksm2 += cksm1;
	aux2 += CONV_LTL_ENDN_4 * aux;

	tim_tos->week_number = (uint)aux2;
	
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
	
	cksm1 += aux;
	cksm2 += cksm1;
	aux2 = aux;
	
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
	
	cksm1 += aux;
	cksm2 += cksm1;
	aux2 += CONV_LTL_ENDN_2 * aux;

	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
	
	cksm1 += aux;
	cksm2 += cksm1;
	aux2 = CONV_LTL_ENDN_3 * aux;
	
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
	
	cksm1 += aux;
	cksm2 += cksm1;
	aux2 += CONV_LTL_ENDN_4 * aux;

	tim_tos->segs_elapsed = (uint)aux2;

	for(i = 0; i < IGNORE_BYTES_TIM_TOS_3; i++) {

		if(fread(&aux, sizeof(uchar), 1, f) != 1)
			return ST_CORRUPT_FILE;

		cksm1 += aux;
		cksm2 += cksm1;

	}

	if(fread(&aux, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	if(aux != cksm1){
		log_print(metadata->logfile, CHECKSUM);
		return ST_INVALID_CHECKSUM;
	}

	if(fread(&aux, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	if(aux != cksm2) {
		return ST_INVALID_CHECKSUM;
		log_print(metadata->logfile, CHECKSUM);
	}

	return ST_OK;
}


status_t load_nav_pvt(arg_s * metadata, nav_pvt_s *nav_pvt, size_t l, uchar cksm1, uchar cksm2) {
puts("nav_pvt");
	FILE * f = NULL;
	size_t i;
    uchar aux;
    long aux_sign;
	unsigned int aux_unsig;
	
	if(! metadata || ! nav_pvt)	
	    return ST_NULL_PTR;
	
	f = metadata->infile;

	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;

	cksm1 += aux;
	cksm2 += cksm1;
	aux_unsig = (unsigned int)aux;
	
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
	
	cksm1 += aux;
	cksm2 += cksm1;
	aux_unsig += CONV_LTL_ENDN_2 * (unsigned int)aux;

	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;

	cksm1 += aux;
	cksm2 += cksm1;
	aux_unsig += CONV_LTL_ENDN_3 * (unsigned int)aux;
	
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;

	cksm1 += aux;
	cksm2 += cksm1;
	aux_unsig += CONV_LTL_ENDN_4 * (unsigned int)aux;

	nav_pvt->pvt_elapsed_time = aux_unsig;

	
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;

	cksm1 += aux;
	cksm2 += cksm1;
	aux_unsig = (unsigned int)aux;
	
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;

	cksm1 += aux;
	cksm2 += cksm1;
	aux_unsig += CONV_LTL_ENDN_2 * (unsigned int)aux;
		
	nav_pvt->pvt_time.tm_year = (unsigned int)aux_unsig;

	
	if(fread(&aux, sizeof(uchar), 1, f) != 1) 
	    return ST_CORRUPT_FILE;

	cksm1 += aux;
	cksm2 += cksm1;
	nav_pvt->pvt_time.tm_mon = (int)aux;
		
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;

	cksm1 += aux;
	cksm2 += cksm1;
	nav_pvt->pvt_time.tm_mday = (int)aux;
		
	if(fread(&aux, sizeof(uchar), 1, f) != 1) //Lee hora y lo guarda en la estructura tim_tos
	    return ST_CORRUPT_FILE;

	cksm1 += aux;
	cksm2 += cksm1;
	nav_pvt->pvt_time.tm_hour = (int)aux;
		
	if(fread(&aux, sizeof(uchar), 1, f) != 1) //Lee hora y lo guarda en la estructura tim_tos
	    return ST_CORRUPT_FILE;

	cksm1 += aux;
	cksm2 += cksm1;
	nav_pvt->pvt_time.tm_min = (int)aux;
		
	if(fread(&aux, sizeof(uchar), 1, f) != 1) //Lee hora y lo guarda en la estructura tim_tos
	    return ST_CORRUPT_FILE;

	cksm1 += aux;
	cksm2 += cksm1;
	nav_pvt->pvt_time.tm_sec = (int)aux;
		
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;

	cksm1 += aux;
	cksm2 += cksm1;
	nav_pvt->validity_flags = aux;

	if(! (((nav_pvt->validity_flags) & MASK_VALID_DATE) >> SHIFT_VALID_DATE))
		return ST_INVALID_DATE;

	if(! (((nav_pvt->validity_flags) & MASK_VALID_TIME) >> SHIFT_VALID_TIME))
		return ST_INVALID_TIME;
	
	for(i = 0; i < IGNORE_BYTES_PVT_1; i++) {

		if(fread(&aux, sizeof(uchar), 1, f) != 1)
			return ST_CORRUPT_FILE;

		cksm1 += aux;
		cksm2 += cksm1;
	}
		

	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;

	cksm1 += aux;
	cksm2 += cksm1;
	nav_pvt->fix = aux;

		
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;

	cksm1 += aux;
	cksm2 += cksm1;
	nav_pvt->flags = aux;

	//if(! (((nav_pvt->flags) & MASK_VALID_FIX) >> SHIFT_VALID_FIX))
	//	return ST_INVALID_FIX;
	
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
			return ST_CORRUPT_FILE;

		cksm1 += aux;
		cksm2 += cksm1;
		
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;

	cksm1 += aux;
	cksm2 += cksm1;
	nav_pvt->n_sat = (uint)aux;
	
	if(read_signed_4(f, &cksm1, &cksm2, &aux_sign) != ST_OK)
		return ST_CORRUPT_FILE;
    nav_pvt->longitude = aux_sign / LONGITUDE_SCALE;
	
	if(read_signed_4(f, &cksm1, &cksm2, &aux_sign) != ST_OK)
		return ST_CORRUPT_FILE;
    nav_pvt->latitude = aux_sign / LATITUDE_SCALE;

	if(read_signed_4(f, &cksm1, &cksm2, &aux_sign) != ST_OK)
		return ST_CORRUPT_FILE;
	nav_pvt->elipsode_height = (double)aux_sign;

	if(read_signed_4(f, &cksm1, &cksm2, &aux_sign) != ST_OK) 
        return ST_CORRUPT_FILE;
	nav_pvt->oversea_height = (double)aux_sign;


	for(i = 0; i < IGNORE_BYTES_PVT_3; i++) {

		if(fread(&aux, sizeof(uchar), 1, f) != 1)
			return ST_CORRUPT_FILE;

		cksm1 += aux;
		cksm2 += cksm1;
	}
		
    if(fread(&aux, sizeof(uchar), 1, f) != 1)
        return ST_CORRUPT_FILE;

    cksm1 += aux;
	cksm2 += cksm1;
    nav_pvt->pdop = (unsigned int)aux;

	if(fread(&aux, sizeof(uchar), 1, f) != 1)
        return ST_CORRUPT_FILE;

    cksm1 += aux;
	cksm2 += cksm1;
    nav_pvt->pdop += (unsigned int)aux * CONV_LTL_ENDN_2;

		
	for(i = 0; i < IGNORE_BYTES_PVT_4; i++) {

		if(fread(&aux, sizeof(uchar), 1, f) != 1)
			return ST_CORRUPT_FILE;

		cksm1 += aux;
		cksm2 += cksm1;
	}

	for(i = 0; i < IGNORE_BYTES_PVT_5; i++) {

		if(fread(&aux, sizeof(uchar), 1, f) != 1)
			return ST_CORRUPT_FILE;

		cksm1 += aux;
		cksm2 += cksm1;
	}

	if(fread(&aux, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	if(aux != cksm1)
		return ST_INVALID_CHECKSUM;

	if(fread(&aux, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	if(aux != cksm2)
		return ST_INVALID_CHECKSUM;

	return ST_OK;
}

status_t load_nav_posllh(arg_s * metadata, nav_posllh_s * nav_posllh, size_t l, uchar cksm1, uchar cksm2) {
puts("posllh");
	FILE * f = NULL;
	uchar aux;
	unsigned int aux_unsig;
	long aux_sign;

	if(! metadata || ! nav_posllh)
		return ST_NULL_PTR;

	f = metadata->infile;
	
	if(read_unsigned_4(f, &cksm1, &cksm2, &aux_unsig) != ST_OK)
		return ST_CORRUPT_FILE;
	nav_posllh->posllh_elapsed_time = (unsigned int)aux_unsig;


	if(read_signed_4(f, &cksm1, &cksm2, &aux_sign) != ST_OK)
		return ST_CORRUPT_FILE;
    nav_posllh->longitude = aux_sign / LONGITUDE_SCALE;
	
	if(read_signed_4(f, &cksm1, &cksm2, &aux_sign) != ST_OK)
		return ST_CORRUPT_FILE;
    nav_posllh->latitude = aux_sign / LATITUDE_SCALE;

	if(read_signed_4(f, &cksm1, &cksm2, &aux_sign) != ST_OK)
		return ST_CORRUPT_FILE;
	nav_posllh->elipsode_height = (double)aux_sign;

	if(read_signed_4(f, &cksm1, &cksm2, &aux_sign) != ST_OK) 
        return ST_CORRUPT_FILE;
	nav_posllh->oversea_height = (double)aux_sign;


	if(read_unsigned_4(f, &cksm1, &cksm2, &aux_unsig) != ST_OK)
		return ST_CORRUPT_FILE;
	nav_posllh->hor_precision = (double)aux_unsig;


	if(read_unsigned_4(f, &cksm1, &cksm2, &aux_unsig) != ST_OK)
		return ST_CORRUPT_FILE;
	nav_posllh->ver_precision = (double)aux_unsig;

		
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	if(aux != cksm1)
		return ST_INVALID_CHECKSUM;

	if(fread(&aux, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	if(aux != cksm2)
		return ST_INVALID_CHECKSUM;

	return ST_OK;
}


status_t tim_tos2tkpt(data_structs_s * data_structs) {
	
	if( ! data_structs)
	    return ST_NULL_PTR;

	data_structs->tkpt->tkpt_time = data_structs->tim_tos->tim_time;
	data_structs->tkpt->tkpt_msec = 0;

	return ST_OK;
}


status_t nav_pvt2tkpt(data_structs_s * data_structs) {
	
	if( ! data_structs)
	    return ST_NULL_PTR;
	
	data_structs->tkpt->tkpt_time = data_structs->nav_pvt->pvt_time;
	data_structs->tkpt->latitude = data_structs->nav_pvt->latitude;
	data_structs->tkpt->longitude = data_structs->nav_pvt->longitude;
	data_structs->tkpt->tkpt_msec = 0;
	data_structs->tkpt->elevation = 0;

	return ST_OK;
}


status_t nav_posllh2tkpt(data_structs_s * data_structs) {
	
	if( ! data_structs)
	    return ST_NULL_PTR;
		
	data_structs->tkpt->latitude = data_structs->nav_posllh->latitude;
	data_structs->tkpt->longitude = data_structs->nav_posllh->longitude;
	data_structs->tkpt->elevation = 0;

	return ST_OK;
}





status_t get_length(FILE * f, size_t * l, uchar * cksm1, uchar * cksm2) {

	uchar aux;

	if(! f || ! l || ! cksm1 || ! cksm2)
		return ST_NULL_PTR;

	if(fread(&aux, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	*cksm1 += aux;
	*cksm2 += *cksm1;

	*l = (size_t)aux;

	if(fread(&aux, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	*cksm1 += aux;
	*cksm2 += *cksm1;
	
	*l += CONV_LTL_ENDN_2 * (size_t)aux;

	return ST_OK;

}

status_t cksm_double(uchar * cksm1, uchar * cksm2, double aux_d) {

	uchar b1, b2, b3, b4;

	if(! cksm1 || ! cksm2)
		return ST_NULL_PTR;

	b1 = ((long int)aux_d & MASK_DOUBLE_B0) >> SHIFT_DOUBLE_B0;
	b2 = ((long int)aux_d & MASK_DOUBLE_B1) >> SHIFT_DOUBLE_B1;
	b3 = ((long int)aux_d & MASK_DOUBLE_B2) >> SHIFT_DOUBLE_B2;
	b4 = ((long int)aux_d & MASK_DOUBLE_B3) >> SHIFT_DOUBLE_B3;

	*cksm1 += b1;
	*cksm2 += *cksm1;
	*cksm1 += b2;
	*cksm2 += *cksm1;
	*cksm1 += b3;
	*cksm2 += *cksm1;
	*cksm1 += b4;
	*cksm2 += *cksm1;

	return ST_OK;
}


status_t read_unsigned_4(FILE * f, uchar * cksm1, uchar * cksm2, unsigned int * val) {

	uchar aux;

	if(! f || ! cksm1 || ! cksm2 || ! val)
		return ST_NULL_PTR;

	if(fread(&aux, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	*val = (unsigned long int)aux;
	*cksm1 += aux;
	*cksm2 += *cksm1;


	if(fread(&aux, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	*val += (unsigned long int)aux * CONV_LTL_ENDN_2;
	*cksm1 += aux;
	*cksm2 += *cksm1;


	if(fread(&aux, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	*val += (unsigned long int)aux * CONV_LTL_ENDN_3;
	*cksm1 += aux;
	*cksm2 += *cksm1;


	if(fread(&aux, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	*val += (unsigned long int)aux * CONV_LTL_ENDN_4;
	*cksm1 += aux;
	*cksm2 += *cksm1;

	return ST_OK;
}

status_t read_signed_4(FILE * f, uchar * cksm1, uchar * cksm2, long * val) {

	uchar aux;

	if(! f || ! cksm1 || ! cksm2 || ! val)
		return ST_NULL_PTR;

	*val = 0;

	if(fread(&aux, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	*val |= (long)aux;
	*cksm1 += aux;
	*cksm2 += *cksm1;


	if(fread(&aux, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	*val |= ((long)aux << SHIFT_DOUBLE_B1);
	*cksm1 += aux;
	*cksm2 += *cksm1;


	if(fread(&aux, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	*val |= ((long)aux << SHIFT_DOUBLE_B2);
	*cksm1 += aux;
	*cksm2 += *cksm1;


	if(fread(&aux, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	*val |= ((long)aux << SHIFT_DOUBLE_B3);
	*cksm1 += aux;
	*cksm2 += *cksm1;

	return ST_OK;
}
