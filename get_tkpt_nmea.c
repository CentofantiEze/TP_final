#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

status_t get_tkpt_nmea(FILE * f, data_structs_s * structs, tkpt_s * tkpt) {
	
	int c, checksum;
	size_t i;
	char aux[MAX_SUBSTR_NMEA];
	bool_t tkpt_flag = FALSE;
	
	if(!f || !structs) 
		return ST_NULL_PTR;
	/* LC. Primero busco el signo pesos */
	
	while(tkpt_flag == FALSE) {
		
		while((c = fgetc(f)) != CHAR_INIT_NMEA && c != EOF) {
		}
		checksum = 0;
		/* Empieza checksum */
		/* LC. Busco los dígitos "GP". Sino están tiro warning */
		for(i = 0; i < TALKER_ID_CHAR; i++) {
			aux[i] = (c = fgetc(f));
			checksum ^= c;
		}
		aux[i] = '\0';
		if((strcmp(aux, GP_INDICATOR))) {
			/* tirar warning */
		}

		for(i = 0; i < NMEA_PROTOCOL; i++) {
			aux[i] = (c = fgetc(f));
			checksum ^= c;

		}
		aux[i] = '\0';
		/* Busco hasta la coma y comparo con "GGA", "ZDA", "RMC". Guardo en aux */

		if(!(strcmp(aux, GGA_INDICATOR))) {
			if(structs->zda->time_flag == TRUE) 
			    tkpt_flag = TRUE;
			read_nmea_gga(f, structs, checksum); 
			gga2tkpt(tkpt, structs->gga); 
	
            puts("Entré a GGA");
            
		} else if(!(strcmp(aux, ZDA_INDICATOR))) {
			structs->zda->time_flag = TRUE;
			read_nmea_zda(f, structs, checksum); // Tiene adentro gga2tkpt, devuelve un puntero a tkpt
			zda2tkpt(tkpt, structs->zda); 
            puts("Entré a ZDA");

		} else if(!(strcmp(aux, RMC_INDICATOR))) {
			if(structs->zda->time_flag == TRUE) 
			    tkpt_flag = TRUE;
			read_nmea_rmc(f, structs, checksum); // Incluye: si no hay dato temporal, po
			rmc2tkpt(tkpt, structs->rmc); 
		    puts("Entré a RMC");
		    
		} else
		/* Tirar error porque no se encontró "GGA", "ZDA" o "RMC" */
		    return ST_IGNORE_STATEMENT;
	} /* while */
	
	return ST_OK;

}

status_t hexstring_2_integer(int d1, int d2, int *dec_num) {

	size_t hex1, hex2;
	char hexa_dic[] = {STR_HEX}, *ptr;

	if(! dec_num) {
		return ST_NULL_PTR;
	}

	/*[11] Busca el primer caracter en el diccionario de digitos hexadecimales (STR_HEX), si no lo encuentra devuelve -1.*/
	if((ptr = strrchr(hexa_dic, d1)) == NULL) {
		return ST_INVALID_ARGUMENT;
	}
	/*[12] Si el caracter se encuentra en el diccionario, se calcula su posición respecto del primer caracter del diccionario.*/
	hex1 = (size_t)ptr - (size_t)hexa_dic;
	
	if((ptr = strrchr(hexa_dic, d2)) == NULL) {
		return ST_INVALID_ARGUMENT;
	}
	hex2 = (size_t)ptr - (size_t)hexa_dic;
	
	/*[13] Si la conversion resulta invalida devuelve -1. De otro modo, realiza la conversion.*/
	if(hex1 >= HEX_DIGITS || hex2 >= HEX_DIGITS) {
		return ST_INVALID_ARGUMENT;
	}

	*dec_num = (int)hex1 * HEX_DIGITS + hex2;

	return ST_OK;

}

status_t get_nmea_data(unsigned int qfields, char string[][MAX_SUBSTR_NMEA], FILE * f, int * current_checksum) {
	
	size_t i,j;
	int c, nmea_checksum, check1, check2;
	bool_t flag_end = FALSE;
  

	if(!string || !f || !current_checksum)
		return ST_NULL_PTR;
	fgetc(f);
	for(j = 0; j < qfields; j++) {
		for(i = 0; (c = (string)[j][i] = fgetc(f))!= EOF && c != CHAR_DELIM && c != CHAR_INIT_NMEA && c != CHAR_END_NMEA; i++) {
			*current_checksum ^= c;
			
		}
		
		if(c == CHAR_DELIM || (c == CHAR_END_NMEA && (j == qfields - 1))) {
			(string)[j][i] = '\0'; // piso la coma
			if(c != CHAR_DELIM) 
			    flag_end = TRUE; // veo si se alcanzó un '*'
			
		} else
		    return ST_IGNORE_STATEMENT;
		
	
	}
	if(flag_end != TRUE)
		while((c = fgetc(f)) != CHAR_END_NMEA && c != EOF) {// si no se había alcanzado el '*' sigue haciendo checksum hasta encontrarlo
			*current_checksum ^= c;
		}
    
    check1 = fgetc(f);
    check2 = fgetc(f); // validar que no sea EOF
    
	hexstring_2_integer((char)check1, (char)check2, &nmea_checksum); //validar

	if(nmea_checksum != *current_checksum) 
		return ST_IGNORE_STATEMENT;
	return ST_OK;
}

status_t gga_time_of_fix(char * s, data_structs_s * structs) {
	
	char aux[MAX_SUBSTR_NMEA], *end_ptr;
	int hours, minutes, seconds, miliseconds, read_digits;

	if(!s || !structs) {
		return ST_NULL_PTR;
	}

	strncpy(aux, s, HOURS_DIGITS); // validar
	read_digits = HOURS_DIGITS;
	aux[read_digits] = '\0';

	hours = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;	
	}

	strncpy(aux, s + read_digits, MINUTES_DIGITS); // validar
	read_digits += MINUTES_DIGITS;
	aux[read_digits] = '\0';

	minutes = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;	
	}

	strncpy(aux, s + read_digits, SECONDS_DIGITS); // validar
	read_digits += SECONDS_DIGITS;
	aux[read_digits] = '\0';

	seconds = strtof(aux, &end_ptr);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}
	
	strncpy(aux, s + read_digits + 1, MILISECONDS_DIGITS); // validar, sumo uno por el punto
	read_digits += MILISECONDS_DIGITS + 1;
	aux[read_digits] = '\0';

	miliseconds = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}

	(structs->gga->gga_time).tm_hour = hours;
	(structs->gga->gga_time).tm_min = minutes;   
	(structs->gga->gga_time).tm_sec = seconds;
	structs->gga->gga_time_milisec = miliseconds;
	//(gga->gga_time).tm_year = arg->time->tm_year; 
	//(gga->gga_time).tm_mon = arg->time->tm_mon;
	//(gga->gga_time).tm_mday = arg->time->tm_mday;

	return ST_OK;
}

/*[19] Lee la latitud de la sentencia NMEA y la guarda en una variable double (trackpt.latitude). Recibe el puntero pos_ptr apuntado al primer caracter de la latitud.
	   Devuelve: ST_OK si la lectura se realiza correctamente y ST_NUMERICAL_ERROR si los datos son invalidos.*/
status_t latitude(char * str1, char * str2, data_structs_s * structs, void * nmea_struct) {

	char aux[MAX_SUBSTR_NMEA];
	double lat;
	char *end_ptr;
	size_t degrees;
	int south_flag = 1, read_digits;
	float minutes;

	if(!str1 || !str2 || !structs || !nmea_struct) {
		return ST_NULL_PTR;
	}

	/*[20] Lee los grados y minutos, los convierte a numeros y realiza el pasaje a grados.*/
	strncpy(aux, str1, NMEA_LATITUDE_DEGREES); // validar
	read_digits = NMEA_LATITUDE_DEGREES;
	aux[read_digits] = '\0';

	degrees = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}
	
	strncpy(aux, str1 + read_digits, NMEA_LATITUDE_MINUTES); // validar
	read_digits += NMEA_LATITUDE_MINUTES;
	aux[read_digits] = '\0';

	minutes = strtof(aux, &end_ptr);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}
	
	/*[21] Lee el caracter que indica Norte o Sur. En caso de que se lea el caracter asociado a Sur multiplica por -1 los grados.*/
	if(str2[0] == SOUTH_CHAR) {
		south_flag = -1;
	}
	
	lat = south_flag * (degrees + minutes / CONVERSION_FACTOR_MINUTES);
	structs->gga->latitude = lat; // esto está bien??


	return ST_OK;
}

status_t longitude(char * str1, char * str2, data_structs_s * structs, void * nmea_struct) {

	char aux[MAX_SUBSTR_NMEA];
	double lon;
	char *end_ptr;
	unsigned int degrees;
	int west_flag = 1, read_digits;
	float minutes;

	if(!str1 || !str2 || !structs || !nmea_struct) {
		return ST_NULL_PTR;
	}

	strncpy(aux, str1, NMEA_LONGITUDE_DEGREES); // validar
	read_digits = NMEA_LONGITUDE_DEGREES;
	aux[read_digits] = '\0';

	degrees = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}

	strncpy(aux, str1 + read_digits, NMEA_LONGITUDE_MINUTES); // validar
	read_digits += NMEA_LONGITUDE_MINUTES;
	aux[read_digits] = '\0';

	minutes = strtof(aux, &end_ptr);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}
	
	if(str2[0] == WEST_CHAR) {
		west_flag = -1;
	}

	
	lon = west_flag * (degrees + minutes / CONVERSION_FACTOR_MINUTES);
	structs->gga->longitude = lon; // esto está bien??

	return ST_OK;
}


/*[23] Lee la calidad del fix y la carga en la estructura.
	   Devuelve: ST_OK si la lectura se realiza correctamente y ST_NUMERICAL_ERROR si los datos son invalidos.*/
status_t gga_quality_of_fix(char * s, data_structs_s * structs) {
	/*[24] Carga el numerodo de la calidad del fix en una variable. Comprueba que sea un numero valido y lo carga en la estructura.*/
	int option = (int)s[0] - ZERO_ASCII_VALUE;

	if(!s || !structs) {
		return ST_NULL_PTR;
	}

	if(!isdigit(s[0])) {
		return ST_NUMERICAL_ERROR;
	}
	if(option > MAX_QUALITY) {
		return ST_NUMERICAL_ERROR;
	}
		
	structs->gga->quality = option;


	return ST_OK;		
}

/*[25] Lee la cantidad de satelites y la carga en la estructura.
	   Devuelve: ST_OK si la lectura se realiza correctamente y ST_INVALID_NUMBER_ERROR si los datos son invalidos.*/
status_t gga_num_of_satellites(char * s, data_structs_s * structs) {

	char *end_ptr;
	unsigned int satellites;

	if(!s || !structs) 
		return ST_NULL_PTR;


	satellites = strtoul(s, &end_ptr, 10);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}
	if(satellites < MIN_SAT || satellites > MAX_SAT) 
		return ST_NUMERICAL_ERROR;
	
	structs->gga->n_sat = satellites;
	return ST_OK;
}

/*[26] Lee hdop y la carga en la estructura.
	   Devuelve: ST_OK si la lectura se realiza correctamente y ST_NUMERICAL_ERROR si los datos son invalidos.*/
status_t gga_hdop(char * s, data_structs_s * structs) {
    double hdop;
	char *end_ptr;

	if(!s || !structs) {
		return ST_NULL_PTR;
	}

	hdop = strtof(s, &end_ptr);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}

	structs->gga->hdop = hdop;

	return ST_OK;
}

/*[27] Lee la elevacion y la carga en la estructura.
	   Devuelve: ST_OK si la lectura se realiza correctamente y ST_NUMERICAL_ERROR si los datos son invalidos.*/
status_t gga_elevation(char * s, data_structs_s * structs) {

    double elevation;
	char *end_ptr;

	if(!s || !structs) 
		return ST_NULL_PTR;

	elevation = strtof(s, &end_ptr);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}

	/*[28] Ignora las unidades de la elevacion.*/

	structs->gga->elevation = elevation;
	return ST_OK;
}

/*[28] Lee la separacion del geoide y la carga en la estructura.
	   Devuelve: ST_OK si la lectura se realiza correctamente y ST_NUMERICAL_ERROR si los datos son invalidos.*/
status_t gga_undulation_of_geoid(char * s, data_structs_s * structs) {
    double undulation;
	char *aux_ptr;

	if(!s || !structs) {
		return ST_NULL_PTR;
	}

	undulation = strtof(s, &aux_ptr);
	if(*aux_ptr != '\0' && *aux_ptr != '\n') {
		return ST_NUMERICAL_ERROR;
	}

	structs->gga->undulation_of_geoid = undulation;

	return ST_OK;
}

status_t read_nmea_gga(FILE * f, data_structs_s * structs, int checksum) {
	
	char s[GGA_FIELDS][MAX_SUBSTR_NMEA];

	if(!f || !structs)
		return ST_NULL_PTR;

	get_nmea_data(GGA_FIELDS, s, f, &checksum); // validar
	gga_time_of_fix(s[GGA_TIME_FIELD], structs); // chequear BIEN lo de struct tm arg (está mal, pero cómo iría?)
	latitude(s[GGA_LAT_FIELD], s[GGA_LAT_N_S_FIELD], structs, structs->gga);
	longitude(s[GGA_LONG_FIELD], s[GGA_LONG_W_E_FIELD], structs, structs->gga);
	gga_quality_of_fix(s[GGA_FIX_FIELD], structs);
	gga_num_of_satellites(s[GGA_SAT_FIELD], structs);
	gga_hdop(s[GGA_HDOP_FIELD], structs);
	gga_elevation(s[GGA_ELEV_FIELD], structs);
	gga_undulation_of_geoid(s[GGA_UNDULATION_FIELD], structs);


	return ST_OK;
}

status_t gga2tkpt(tkpt_s * tkpt, gga_s * gga) {
	
	if(!tkpt || !gga)
		return ST_NULL_PTR;


	tkpt->tkpt_time.tm_hour = gga->gga_time.tm_hour; // ???
	tkpt->tkpt_time.tm_min = gga->gga_time.tm_min;
	tkpt->tkpt_time.tm_sec = gga->gga_time.tm_sec;
	tkpt->tkpt_msec = gga->gga_time_milisec;
	tkpt->latitude = gga->latitude;
	tkpt->longitude = gga->longitude;
	tkpt->elevation = gga->elevation;	

	return ST_OK;

}

status_t zda2tkpt(tkpt_s * tkpt, zda_s * zda) {
	
	if(!tkpt || !zda)
		return ST_NULL_PTR;


	tkpt->tkpt_time.tm_hour = zda->zda_time.tm_hour; // ???
	tkpt->tkpt_time.tm_min = zda->zda_time.tm_min;
	tkpt->tkpt_time.tm_sec = zda->zda_time.tm_sec;
	tkpt->tkpt_time.tm_year = zda->zda_time.tm_year;
	tkpt->tkpt_time.tm_mon = zda->zda_time.tm_mon;
	tkpt->tkpt_time.tm_year = zda->zda_time.tm_year;
	tkpt->tkpt_msec = zda->zda_time_milisec;

	return ST_OK;

}

status_t rmc2tkpt(tkpt_s * tkpt, rmc_s * rmc) {
	
	if(!tkpt || !rmc)
		return ST_NULL_PTR;

	tkpt->latitude = rmc->latitude;
	tkpt->longitude = rmc->longitude;
	tkpt->elevation = 0;		

	return ST_OK;

}


status_t zda_time_of_fix(char * str1, char * str2, char * str3, char * str4, data_structs_s * structs) {
	
	char aux[MAX_SUBSTR_NMEA], *end_ptr;
	int hours, minutes, seconds, miliseconds, year, month, day, read_digits;

	if(!str1 || !str2 || !str3 || !str4 || !structs) {
		return ST_NULL_PTR;
	}

	strncpy(aux, str1, HOURS_DIGITS); // validar
	read_digits = HOURS_DIGITS;
	aux[read_digits] = '\0';

	hours = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;	
	}

	strncpy(aux, str1 + read_digits, MINUTES_DIGITS); // validar
	read_digits += MINUTES_DIGITS;
	aux[read_digits] = '\0';

	minutes = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;	
	}

	strncpy(aux, str1 + read_digits, SECONDS_DIGITS); // validar
	read_digits += SECONDS_DIGITS;
	aux[read_digits] = '\0';

	seconds = strtof(aux, &end_ptr);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}
	
	strncpy(aux, str1 + read_digits + 1, MILISECONDS_DIGITS); // validar, sumo uno por el punto
	read_digits += MILISECONDS_DIGITS + 1;
	aux[read_digits] = '\0';

	miliseconds = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}

	year = strtof(str2, &end_ptr);
	if(*end_ptr != '\0' && *end_ptr != '\n') {
		return ST_NUMERICAL_ERROR;
	}
	month = strtof(str3, &end_ptr);
	if(*end_ptr != '\0' && *end_ptr != '\n') {
		return ST_NUMERICAL_ERROR;
	}
	day = strtof(str4, &end_ptr);
	if(*end_ptr != '\0' && *end_ptr != '\n') {
		return ST_NUMERICAL_ERROR;
	}


	(structs->zda->zda_time).tm_hour = hours;
	(structs->zda->zda_time).tm_min = minutes;   
	(structs->zda->zda_time).tm_sec = seconds;
	structs->zda->zda_time_milisec = miliseconds;
	(structs->zda->zda_time).tm_year = year; 
	(structs->zda->zda_time).tm_mon = month;
	(structs->zda->zda_time).tm_mday = day;
	
	return ST_OK;
}

status_t zda_time_zone(char * s, data_structs_s * structs) {

	char *aux_ptr;
	int time_zone;

	if(!s || !structs) {
		return ST_NULL_PTR;
	}

	time_zone = strtol(s, &aux_ptr, 10);
	if(*aux_ptr != '\0' && *aux_ptr != '\n') {
		return ST_NUMERICAL_ERROR;
	}
	structs->zda->time_zone = time_zone;
	return ST_OK;
}

status_t zda_time_difference(char * s, data_structs_s * structs) {

	char *aux_ptr;
	int time_diff;

	if(!s || !structs) {
		return ST_NULL_PTR;
	}

	time_diff = strtol(s, &aux_ptr, 10);
	if(*aux_ptr != '\0' && *aux_ptr != '\n') {
		return ST_NUMERICAL_ERROR;
	}

	structs->zda->diff_minutes = time_diff;

	return ST_OK;
}


status_t read_nmea_zda(FILE * f, data_structs_s * structs, int checksum) {
	
	char s[ZDA_FIELDS][MAX_SUBSTR_NMEA];

	if(!f || !structs)
		return ST_NULL_PTR;
	
	get_nmea_data(ZDA_FIELDS, s, f, &checksum); // validar
	zda_time_of_fix(s[ZDA_TIME_FIELD], s[ZDA_Y_FIELD], s[ZDA_M_FIELD], s[ZDA_D_FIELD], structs); // chequear BIEN lo de struct tm arg (está mal, pero cómo iría?)
	zda_time_zone(s[ZDA_TIMEZONE_FIELD], structs);
	zda_time_difference(s[ZDA_DIFF_FIELD], structs);

	return ST_OK;

}

status_t rmc_time_of_fix(char * s, data_structs_s * structs) {
	
	char aux[MAX_SUBSTR_NMEA], *end_ptr;
	int hours, minutes, seconds, miliseconds, read_digits;

	if(!s || !structs) {
		return ST_NULL_PTR;
	}

	strncpy(aux, s, HOURS_DIGITS); // validar
	read_digits = HOURS_DIGITS;
	aux[read_digits] = '\0';

	hours = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;	
	}

	strncpy(aux, s + read_digits, MINUTES_DIGITS); // validar
	read_digits += MINUTES_DIGITS;
	aux[read_digits] = '\0';

	minutes = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;	
	}

	strncpy(aux, s + read_digits, SECONDS_DIGITS); // validar
	read_digits += SECONDS_DIGITS;
	aux[read_digits] = '\0';

	seconds = strtof(aux, &end_ptr);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}
	
	strncpy(aux, s + read_digits + 1, MILISECONDS_DIGITS); // validar, sumo uno por el punto
	read_digits += MILISECONDS_DIGITS + 1;
	aux[read_digits] = '\0';

	miliseconds = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}

	(structs->rmc->rmc_time).tm_hour = hours;
	(structs->rmc->rmc_time).tm_min = minutes;   
	(structs->rmc->rmc_time).tm_sec = seconds;
	structs->rmc->rmc_time_milisec = miliseconds;
	//(zda->zda_time).tm_year = arg->time->tm_year; 
	//(zda->zda_time).tm_mon = arg->time->tm_mon;
	//(zda->zda_time).tm_mday = arg->time->tm_mday;

	return ST_OK;
}

status_t rmc_status(char * s, data_structs_s * structs) {

	if(!s || !structs) {
		return ST_NULL_PTR;
	}

	if(s[0] == RMC_CHAR_A)
		structs->rmc->status = RMC_STATUS_A;
	else if(s[0] == RMC_CHAR_V)
		structs->rmc->status = RMC_STATUS_V;
	else{}
		

	return ST_OK;
	
}

status_t rmc_speed(char * s, data_structs_s * structs) {

	char *aux_ptr;
	double speed;

	if(!s || !structs) {
		return ST_NULL_PTR;
	}

	speed = strtof(s, &aux_ptr);
	if(*aux_ptr != '\0' && *aux_ptr != '\n') {
		return ST_NUMERICAL_ERROR;
	}

	structs->rmc->speed = speed;

	return ST_OK;
}

status_t rmc_angle(char * s, data_structs_s * structs) {

	char *aux_ptr;
	double angle;

	if(!s || !structs) {
		return ST_NULL_PTR;
	}

	angle = strtof(s, &aux_ptr);
	if(*aux_ptr != '\0' && *aux_ptr != '\n') {
		return ST_NUMERICAL_ERROR;
	}

	structs->rmc->tracking_angle = angle;

	return ST_OK;
}


status_t rmc_magnetic_deviation(char * s, data_structs_s * structs) {

	char *aux_ptr;
	double magnetic;

	if(!s || !structs) {
		return ST_NULL_PTR;
	}

	magnetic = strtof(s, &aux_ptr);
	if(*aux_ptr != '\0' && *aux_ptr != '\n') {
		return ST_NUMERICAL_ERROR;
	}

	structs->rmc->magnetic_deviation = magnetic;

	return ST_OK;
}

status_t rmc_latitude(char * str1, char * str2, data_structs_s * structs, void * nmea_struct) {

	char aux[MAX_SUBSTR_NMEA];
	double lat;
	char *end_ptr;
	size_t degrees;
	int south_flag = 1, read_digits;
	float minutes;

	if(!str1 || !str2 || !structs || !nmea_struct) {
		return ST_NULL_PTR;
	}

	/*[20] Lee los grados y minutos, los convierte a numeros y realiza el pasaje a grados.*/
	strncpy(aux, str1, NMEA_LATITUDE_DEGREES); // validar
	read_digits = NMEA_LATITUDE_DEGREES;
	aux[read_digits] = '\0';

	degrees = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}
	
	strncpy(aux, str1 + read_digits, NMEA_LATITUDE_MINUTES); // validar
	read_digits += NMEA_LATITUDE_MINUTES;
	aux[read_digits] = '\0';

	minutes = strtof(aux, &end_ptr);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}
	
	/*[21] Lee el caracter que indica Norte o Sur. En caso de que se lea el caracter asociado a Sur multiplica por -1 los grados.*/
	if(str2[0] == SOUTH_CHAR) {
		south_flag = -1;
	}
	
	lat = south_flag * (degrees + minutes / CONVERSION_FACTOR_MINUTES);
	structs->rmc->latitude = lat; // esto está bien??


	return ST_OK;
}

status_t rmc_longitude(char * str1, char * str2, data_structs_s * structs, void * nmea_struct) {

	char aux[MAX_SUBSTR_NMEA];
	double lon;
	char *end_ptr;
	unsigned int degrees;
	int west_flag = 1, read_digits;
	float minutes;

	if(!str1 || !str2 || !structs || !nmea_struct) {
		return ST_NULL_PTR;
	}

	strncpy(aux, str1, NMEA_LONGITUDE_DEGREES); // validar
	read_digits = NMEA_LONGITUDE_DEGREES;
	aux[read_digits] = '\0';

	degrees = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}

	strncpy(aux, str1 + read_digits, NMEA_LONGITUDE_MINUTES); // validar
	read_digits += NMEA_LONGITUDE_MINUTES;
	aux[read_digits] = '\0';

	minutes = strtof(aux, &end_ptr);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}
	
	if(str2[0] == WEST_CHAR) {
		west_flag = -1;
	}

	
	lon = west_flag * (degrees + minutes / CONVERSION_FACTOR_MINUTES);
	structs->rmc->longitude = lon; 
	return ST_OK;
}

status_t read_nmea_rmc(FILE * f, data_structs_s * structs, int checksum) {
	
	char s[RMC_FIELDS][MAX_SUBSTR_NMEA];

	if(!f || !structs)
		return ST_NULL_PTR;
	
	get_nmea_data(RMC_FIELDS, s, f, &checksum); // validar
	rmc_time_of_fix(s[RMC_TIME_FIELD], structs);
	rmc_status(s[RMC_STATUS_FIELD], structs);
	latitude(s[RMC_LAT_FIELD], s[RMC_LAT_N_S_FIELD], structs, structs->rmc);
	longitude(s[RMC_LONG_FIELD], s[RMC_LONG_W_E_FIELD], structs, structs->rmc);
	rmc_speed(s[RMC_SPEED_FIELD], structs);
	rmc_angle(s[RMC_ANGLE_FIELD], structs);
	rmc_magnetic_deviation(s[RMC_MAGNETIC_FIELD], structs);

	return ST_OK;

}
