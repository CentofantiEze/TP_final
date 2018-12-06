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

				log_print(metadata->logfile, TKPT_GEN);
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

    		return ST_OK;
    	}

    	if(l == PAYLOAD_NAV_POSLLH) {

    		if((st = load_nav_posllh(metadata, data_structs->nav_posllh, l, cksm1, cksm2)) != ST_OK)
				return st;

			if((st = nav_posllh2tkpt(data_structs)) != ST_OK)
				return st;

			if(data_structs->tkpt->time_flag == TRUE) {

				log_print(metadata->logfile, TKPT_GEN);
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

	FILE * f = NULL;
	size_t i;
	uchar aux;
	unsigned int aux_unsig;


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
	aux_unsig = (unsigned int)aux;
	
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
	
	cksm1 += aux;
	cksm2 += cksm1;
	aux_unsig += CONV_LTL_ENDN_2 * (unsigned int)aux;
		
	tim_tos->tim_time.tm_year = (int)aux_unsig;
	
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
	aux_unsig = CONV_LTL_ENDN_3 * (unsigned int)aux;
	
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
	
	cksm1 += aux;
	cksm2 += cksm1;
	aux_unsig += CONV_LTL_ENDN_4 * (unsigned int)aux;

	tim_tos->week_number = (uint)aux_unsig;
	
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
	aux_unsig = CONV_LTL_ENDN_3 * (unsigned int)aux;
	
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
	
	cksm1 += aux;
	cksm2 += cksm1;
	aux_unsig += CONV_LTL_ENDN_4 * (unsigned int)aux;

	tim_tos->segs_elapsed = (uint)aux_unsig;

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

	if(! (((nav_pvt->validity_flags) & MASK_VALID_DATE) >> SHIFT_VALID_DATE)) {
		log_print(metadata->logfile, UBX_DATE);
		return ST_INVALID_DATE;
	}

	if(! (((nav_pvt->validity_flags) & MASK_VALID_TIME) >> SHIFT_VALID_TIME)) {
		log_print(metadata->logfile, UBX_TIME);
		return ST_INVALID_TIME;
	}

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

	if(! (((nav_pvt->flags) & MASK_VALID_FIX) >> SHIFT_VALID_FIX))
		log_print(metadata->logfile, NMEA_FIX);

	
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
    nav_pvt->longitude = (double)aux_sign / LONGITUDE_SCALE;
	
	if(read_signed_4(f, &cksm1, &cksm2, &aux_sign) != ST_OK)
		return ST_CORRUPT_FILE;
    nav_pvt->latitude = (double)aux_sign / LATITUDE_SCALE;

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

	if(aux != cksm1) {
		log_print(metadata->logfile, CHECKSUM);
		return ST_INVALID_CHECKSUM;
	}

	if(fread(&aux, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	if(aux != cksm2) {
		log_print(metadata->logfile, CHECKSUM);
		return ST_INVALID_CHECKSUM;
	}

	return ST_OK;
}

status_t load_nav_posllh(arg_s * metadata, nav_posllh_s * nav_posllh, size_t l, uchar cksm1, uchar cksm2) {

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
    nav_posllh->longitude = (double)aux_sign / LONGITUDE_SCALE;
	
	if(read_signed_4(f, &cksm1, &cksm2, &aux_sign) != ST_OK)
		return ST_CORRUPT_FILE;
    nav_posllh->latitude = (double)aux_sign / LATITUDE_SCALE;

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

	if(aux != cksm1) {
		log_print(metadata->logfile, CHECKSUM);
		return ST_INVALID_CHECKSUM;
	}

	if(fread(&aux, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	if(aux != cksm2) {
		log_print(metadata->logfile, CHECKSUM);
		return ST_INVALID_CHECKSUM;
	}

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

	uchar aux1, aux2, aux3, aux4;

	if(! f || ! cksm1 || ! cksm2 || ! val)
		return ST_NULL_PTR;

	*val = 0;

	if(fread(&aux1, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	*cksm1 += aux1;
	*cksm2 += *cksm1;

	if(fread(&aux2, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	*cksm1 += aux2;
	*cksm2 += *cksm1;

	if(fread(&aux3, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	*cksm1 += aux3;
	*cksm2 += *cksm1;

	if(fread(&aux4, sizeof(uchar), 1, f) != 1)
		return ST_CORRUPT_FILE;

	*cksm1 += aux4;
	*cksm2 += *cksm1;

	if(aux4 & 0x80) {

		*val = ~aux1 + (~aux2) * CONV_LTL_ENDN_2 + (~aux3) * CONV_LTL_ENDN_3 + (~aux4) * CONV_LTL_ENDN_4 + 1;
		*val *= -1;
	}

	*val = ~aux1 + aux2 * CONV_LTL_ENDN_2 + aux3 * CONV_LTL_ENDN_3 + aux4 * CONV_LTL_ENDN_4 + 1;

	return ST_OK;
}
