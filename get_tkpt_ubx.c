status_ t protocol_detect(arg_s * metadata); //Cambia la estructura
status_t get_tkpt(arg_s * metadata, tkpt_s *); 
status_t get_tkpt_NMEA(arg_s * metadata, tkpt_s *);
status_t get_tkpt_UBX(arg_s * metadata, tkpt_s *);
status_t gpx_process(Lista *, process_t ran);

int main(int argc, char *argv[]){
	protocol_detect()
	while(!get_trackpoint){
	    if(prot==NMEA)
		    get_trackpoint_NMEA
		else
			get_trackpoint_UBX
		srand(time(NULL));
		ran=rand()%3; //process_t ran      process_t es un tipo enumerativo
	    gpx_process(ran);
	}
	gpx_process(PROCESS_ALL);
	
}

#define DATA_NAV_PVT_S 0
#define DATA_TIM_TOS_S 1
#define DATA_NAV_POSLLH_S 2
#define DATA_TKPT_S 3


status_t get_tkpt_ubx(FILE * f, void * data_structs[]) { //FALTA CHECKSUM

    uchar aux;
	bool_t tkpt_flag = FALSE;

	if(! f || ! data_structs)
	    return ST_NULL_PTR;
		
	if(feof(f))
	    return ST_ERROR_EOF;
	
    search_sync(f); //Busca los 2 caracteres de sincronismo
	
	while(tkpt_flag == FALSE) {
	
	    if(fread(&aux, sizeof(uchar), 1, f) != 1) //Lee CLASS
	        return ST_CORRUPT_FILE;
	
        if(aux == CLASS_TIM_TOS) {
	        if(load_tim_tos(f, data_structs[DATA_TIM_TOS_S]) != ST_OK) //Lee y carga tim_tos
			    return ST_IGNORE_STATEMENT;
			if(tim_tos2tkpt(data_structs[DATA_TIM_TOS_S], data_structs[DATA_TKPT_S]) != ST_OK) //Carga en tkpt
			    return ST_IGNORE_STATEMENT;
		    data_structs[DATA_TIM_TOS_S]->time_flag = TRUE;
	    }
		
	    if(aux != CLASS_NAV)
	        return ST_IGNORE_STATEMENT;
		
        if(fread(&aux, sizeof(uchar), 1, f) != 1) //Lee ID
	        return ST_CORRUPT_FILE;
	
	    if(aux == ID_NAV_PVT) {
	        if(load_nav_pvt(f, data_structs[DATA_NAV_PVT_S]) != ST_OK) //Lee y carga nav_pvt
			    return ST_IGNORE_STATEMENT;
			if(nav_pvt2tkpt(data_structs[DATA_NAV_PVT_S], data_structs[DATA_TKPT_S]) != ST_OK) //Carga en tkpt
			    return ST_IGNORE_STATEMENT;
		    if(data_structs[DATA_TIM_TOS_S]->time_flag == TRUE)
			    tkpt_flag = TRUE;
	    }
	
	    if(aux == ID_NAV_POSLLH) {
	       if(load_nav_posllh(f, data_structs[DATA_NAV_POSLLH_S]) != ST_OK) //Lee y cargar nav_posllh
		       return ST_IGNORE_STATEMENT;
			if(nav_posllh2tkpt(data_structs[DATA_NAV_POSLLH_S], data_structs[DATA_TKPT_S]) != ST_OK) //Carga en tkpt
			    return ST_IGNORE_STATEMENT;
			if(data_structs[DATA_TIM_TOS_S]->time_flag == TRUE)
		        tkpt_flag = TRUE;
	    }
	    
		if(aux != ID_NAV_PVT && aux != ID_NAV_POSLLH)
	        return ST_IGNORE_STATEMENT;
		
		return ST_NO_TIME_DATA; //Si carga alguna estructura nav, pero no hay datos en tim_tos 
    }
    
	return ST_OK;
}


void search_sync(FILE * f) {

	do{
		while(fread(&aux, sizeof(uchar), 1, f) == 1 && aux != SYNC_1);
	} while(fread(&aux, sizeof(uchar), 1, f) == 1 && aux != SYNC_2);
}


status_t load_tim_tos (FILE *f, tim_tos_s * tim_tos){
	
	uchar aux;
	int l, aux2;
	
	if(! f || ! tim_tos)
	    return ST_NULL_PTR;
	
	if(fread(&aux, sizeof(uchar), 1, f) != 1) //Lee ID
	    return ST_CORRUPT_FILE;
		
	if(aux != ID_TIM_TOS)
	    return ST_IGNORE_STATEMENT;
	/*Las siguientes 9 lineas leen el largo (que esta en Little Endian) y lo guarda en la variable l */
    if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
		
	l = (int)aux;
	
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
		
	l += 256*(int)aux;
	
	if(l != LENGHT_TIM_TOS)
	    return ST_IGNORE_STATEMENT;
		
	if(fread(tim_tos->version, sizeof(uchar), 1, f) != 1) //Lee version y lo guarda en la estructura tim_tos
	    return ST_CORRUPT_FILE;
		
	if(fread(tim_tos->GNSS, sizeof(uchar), 1, f) != 1) //Lee GNSS y lo guarda en la estructura tim_tos  ***DEFINIR GNSS EN LA ESTRUCTURA***
	    return ST_CORRUPT_FILE;
		
	if(fseek(f, 6, SEEK_CURR) != 0) //Ignora 6 bytes
	    return //ERROR NO PUDO CORRER EL PUNTERO
	/* Las siguientes 11 lineas leen year (En Little Endian) y lo cargan en la estructura tim_tos*/
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
	
	aux2 = (int)aux;
	
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
		
	aux2 += 256*(int)aux;
		
	tim_tos->tim_time->tm_year = aux2;
	
	if(fread(tim_tos->tim_time->tm_mon, sizeof(uchar), 1, f) != 1) //Lee mes y lo guarda en la estructura tim_tos
	    return ST_CORRUPT_FILE;
		
	if(fread(tim_tos->tim_time->tm_mday, sizeof(uchar), 1, f) != 1) //Lee dia y lo guarda en la estructura tim_tos
	    return ST_CORRUPT_FILE;
		
	if(fread(tim_tos->tim_time->tm_hour, sizeof(uchar), 1, f) != 1) //Lee hora y lo guarda en la estructura tim_tos
	    return ST_CORRUPT_FILE;
		
	if(fread(tim_tos->tim_time->tm_min, sizeof(uchar), 1, f) != 1) //Lee min y lo guarda en la estructura tim_tos
	    return ST_CORRUPT_FILE;
		
	if(fread(tim_tos->tim_time->tm_sec, sizeof(uchar), 1, f) != 1) //Lee sec y lo guarda en la estructura tim_tos
	    return ST_CORRUPT_FILE;
		
	if(fseek(f, 9, SEEK_CURR) != 0) //Ignora 9 bytes
	    return //ERROR NO PUDO CORRER EL PUNTERO
		
	//RESTA LEER LOS ULTIMOS 2 CAMPOS, QUE VIENEN DADOS EN 4 BYTES; Y HACER EL CHECKSUM.
	return ST_OK;
}


status_t load_nav_pvt(FILE* f, nav_pvt_s *nav_pvt){

    uchar aux;
	int l, aux2;
	
	if(! f || ! nav_pvt)
	    return ST_NULL_PTR;
	/*Las siguientes 9 lineas leen el largo (Little Endian) y lo guardan en la variable l*/
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
		
	l = (int)aux;
	
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
		
	l += 256*(int)aux;
	
	//LEER pvt_elapsed_time (4 BYTES)
	
	/*Las siguientes 11 lineas leen year (Little Endian) y lo guardan en la estructura */
    	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
	
	aux2 = (int)aux;
	
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
		
	aux2 += 256*(int)aux;
		
	nav_pvt->pvt_time->tm_year = aux2;
	
	if(fread(nav_pvt->pvt_time->tm_mon, sizeof(uchar), 1, f) != 1) //Lee mes y lo guarda en la estructura nav_pvt
	    return ST_CORRUPT_FILE;
		
	if(fread(nav_pvt->pvt_time->tm_mday, sizeof(uchar), 1, f) != 1) //Lee dia y lo guarda en la estructura nav_pvt
	    return ST_CORRUPT_FILE;
		
	if(fread(nav_pvt->pvt_time->tm_hour, sizeof(uchar), 1, f) != 1) //Lee hora y lo guarda en la estructura nav_pvt
	    return ST_CORRUPT_FILE;
		
	if(fread(nav_pvt->pvt_time->tm_min, sizeof(uchar), 1, f) != 1) //Lee min y lo guarda en la estructura nav_pvt
	    return ST_CORRUPT_FILE;
		
	if(fread(nav_pvt->pvt_time->tm_sec, sizeof(uchar), 1, f) != 1) //Lee sec y lo guarda en la estructura nav_pvt
	    return ST_CORRUPT_FILE;
		
	//LEER VALIDITY FLAGS (1 BYTE)
	
	if(fseek(f, 8, SEEK_CURR) != 0) //Ignora 8 bytes
	    return //ERROR NO PUDO CORRER EL PUNTERO
		
	if(fread(nav_pvt->fix, sizeof(uchar), 1, f) != 1) //Lee fix y lo guarda en la estructura nav_pvt
	    return ST_CORRUPT_FILE;
		
	//LEER FIX STATUTS(1 BYTE)
	
	if(fseek(f, 1, SEEK_CURR) != 0) //Ignora 1 byte
	    return //ERROR NO PUDO CORRER EL PUNTERO
		
	if(fread(nav_pvt->n_sat, sizeof(uchar), 1, f) != 1) //Lee n_sat y lo guarda en la estructura nav_pvt
	    return ST_CORRUPT_FILE;
		
	if(fread(nav_pvt->longitude, sizeof(double), 1, f) != 1) //Lee longitud y lo guarda en la estructura nav_pvt
        return ST_CORRUPT_FILE;
	
	if(fread(nav_pvt->latitude, sizeof(double), 1, f) != 1) //Lee latitud y lo guarda en la estructura nav_pvt
        return ST_CORRUPT_FILE;
	
	if(fread(nav_pvt->elipsode_height, sizeof(double), 1, f) != 1) //Lee elipsode_height y lo guarda en la estructura nav_pvt
        return ST_CORRUPT_FILE;
	
	if(fread(nav_pvt->oversea_height, sizeof(double), 1, f) != 1) //Lee oversea_height y lo guarda en la estructura nav_pvt
        return ST_CORRUPT_FILE;
	
	if(fseek(f, 36, SEEK_CURR) != 0) //Ignora 36 bytes
	    return //ERROR NO PUDO CORRER EL PUNTERO
		
    	if(fread(nav_pvt->pdop, sizeof(double), 1, f) != 1) //Lee pdop y lo guarda en la estructura nav_pvt
        return ST_CORRUPT_FILE;
		
	if(fseek(f, 14, SEEK_CURR) != 0) //Ignora 6 bytes (reservado) + 8 bytes
	    return //ERROR NO PUDO CORRER EL PUNTERO
		
	//FALTA HACER CHECKSUM
	return ST_OK;
}


status_t load_nav_posllh(FILE* f, nav_posllh_s * nav_posllh){
	
	uchar aux;
	int l, aux2;
	
	if( ! f || ! nav_posllh)
	   return ST_NULL_PTR;
	   
   /*Las siguientes 9 lineas leen el largo (Little Endian) y lo guardan en la variable l*/
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
		
	l = (int)aux;
	
	if(fread(&aux, sizeof(uchar), 1, f) != 1)
	    return ST_CORRUPT_FILE;
		
	l += 256*(int)aux;
	
	//LEER posllh_elapsed_time (4 BYTES)
	
	if(fread(nav_posllh->longitude, sizeof(double), 1, f) != 1) //Lee longitude y lo guarda en la estructura nav_posllh
        return ST_CORRUPT_FILE;
	
	if(fread(nav_posllh->latitude, sizeof(double), 1, f) != 1) //Lee latitude y lo guarda en la estructura nav_posllh
        return ST_CORRUPT_FILE;
		
	if(fread(nav_posllh->elipsode_height, sizeof(double), 1, f) != 1) //Lee elipsode_height y lo guarda en la estrcutura nav_posllh
        return ST_CORRUPT_FILE;
		
	if(fread(nav_posllh->oversea_height, sizeof(double), 1, f) != 1) //Lee oversea_height y lo guarda en la estrcutura nav_posllh
        return ST_CORRUPT_FILE;
		
	if(fread(nav_posllh->hor_precision, sizeof(double), 1, f) != 1) //Lee hot_precision y lo guarda en la estrcutura nav_posllh
        return ST_CORRUPT_FILE;
		
	if(fread(nav_posllh->ver_precision, sizeof(double), 1, f) != 1) //Lee ver_precision y lo guarda en la estrcutura nav_posllh
        return ST_CORRUPT_FILE;
		
		//FALTA HACER CHECKSUM
	return ST_OK;
}


status_t tim_tos2tkpt(tim_tos_s* tim_tos, tkpt_s* tkpt){
	
	if( ! tim_tos || ! tkpt)
	    return ST_NULL_PTR;
		
	tkpt->tkpt_time = tim_tos->tim_time;
	//DE DONDE SACO tkpt_msec??
	return ST_OK;
}


status_t nav_pvt2tkpt(nav_pvt_s* nav_pvt, tkpt_s* tkpt){
	
	if( ! nav_pvt || ! tkpt)
	    return ST_NULL_PTR;
	
	tkpt->tkpt_time = nav_pvt->pvt_time;
	tkpt->latitude = nav_pvt->latitude;
	tkpt->longitude = nav_pvt->longitude;
	
	//DE DONDE SACO tkpt_msec? ---- ¿CON QUE SE CORRESPONDE ELEVATION?
	return ST_OK;
}


status_t nav_posllh2tkpt(nav_posllh_s* nav_posllh, tkpt_s* tkpt){
	
	if( ! nav_posllh || ! tkpt)
	    return ST_NULL_PTR;
		
	tkpt->latitude = nav_posllh->latitude;
	tkpt->longitude = nav_posllh->longitude;
	
	//¿ELEVATION?
	return ST_OK;
}

//LA ESTRUCTURA nav_pvt TIENE DATOS DE TIEMPO, ENTONCES NO DEBERIA ESPERAR UNA tim_tos PARA MANDAR UN tkpt, NO?
