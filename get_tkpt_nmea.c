#define DATA_GGA_S 0
#define DATA_ZDA_S 1
#define DATA_RMC_S 2
#define DATA_TKPT_S 3

#define GGA_FIELDS 12
#define ZDA_FIELDS 4
#define RMC_FIELDS 10

#define GGA_TIME_FIELD 0
#define GGA_LAT_FIELD 1
#define GGA_LAT_N_S_FIELD 2
#define GGA_LONG_FIELD 3
#define GGA_LONG_W_E_FIELD 4
#define GGA_FIX_FIELD 5
#define GGA_SAT_FIELD 6
#define GGA_HDOP_FIELD 7
#define GGA_ELEV_FIELD 8
#define GGA_UNDULATION_FIELD 10

#define CHAR_INIT_NMEA '$'
#define CHAR_END_NMEA '*'
#define TALKER_ID_CHAR 2
#define NMEA_PROTOCOL 3
#define CHAR_DELIM ','
#define MAX_STR_NMEA 70
#define MAX_SUBSTR_NMEA 10
#define STR_HEX "0123456789ABCDEF"
#define HEXSTRING_NULL_FIND_INT -1
#define HEX_DIGITS 16
#define GP_INDICATOR "GP"
#define GGA_INDICATOR "GGA"
#define ZDA_INDICATOR "ZDA"
#define RMC_INDICATOR "RMC"


/* macros de tiempo de fix */
#define HOURS_DIGITS 2
#define MINUTES_DIGITS 2
#define SECONDS_DIGITS 2
#define MILISECONDS_DIGITS 3

/* macros de latitud y longitud */
#define NMEA_LATITUDE_DEGREES 2
#define NMEA_LATITUDE_MINUTES 5
#define NMEA_LONGITUDE_DEGREES 3
#define NMEA_LONGITUDE_MINUTES 5
#define CONVERSION_FACTOR_MINUTES 60
#define SOUTH_CHAR 'S'
#define WEST_CHAR 'W'

/* macros de calidad de fix */

#define INVALID_FIX_CHAR '0'
#define GPX_FIX_CHAR '1'
#define DGPS_FIX_CHAR '2'
#define PPS_FIX_CHAR '3'
#define RTK_FIX_CHAR '4'
#define FLOAT_RTK_FIX_CHAR '5'
#define ESTIMATED_FIX_CHAR '6'
#define MANUAL_FIX_CHAR '7'
#define SIMULATION_FIX_CHAR '8'

/* macros de cantidad de satelites */
#define MAX_SAT 12
#define MIN_SAT 0

/* el resto */

#define HDOP_DIGITS 2
#define ELEVATION_DIGITS 2
#define UNDULATION_OF_GEOID_DIGITS 2


status_t get_tkpt_nmea(FILE * f, void * data_structs[]) {
	
	int c, checksum;
	size_t i;
	char * aux;
	bool_t tkpt_flag = FALSE, time_flag = FALSE;


	if(!metadata || !data_structs)
		return ST_NULL_PTR;

	/* LC. Primero busco el signo pesos */
	while((c = fgetc(f)) != CHAR_INIT_NMEA && c != EOF);
		
	while(tkpt_flag == FALSE) {

		checksum = 0;
		/* Empieza checksum */
		/* LC. Busco los dígitos "GP". Sino están tiro warning */
		for(i = 0; i < TALKER_ID_CHAR; i++) {
			aux[i] = (c = fgetc(f));
			checksum ^= c;

		}

		if((strcmp(aux, GP_INDICATOR))) {
			/* tirar warning */
		}

		for(i = 0; i < NMEA_PROTOCOL; i++) {
			aux[i] = (c = fgetc(f));
			checksum ^= c;

		}

		/* Busco hasta la coma y comparo con "GGA", "ZDA", "RMC". Guardo en aux */

		if(!(strcmp(aux, GGA_INDICATOR))) {
			tkpt_flag = TRUE;
			read_nmea_gga(f, data_structs[DATA_TKPT_S], data_structs[DATA_GGA_S]); 

		} else if(!(strcmp(aux, ZDA_INDICATOR))) {
			data_structs[DATA_ZDA_S]->time_flag = TRUE;
			read_nmea_zda(f, data_structs[DATA_TKPT_S], data_structs[DATA_ZDA_S]); // Tiene adentro gga2tkpt, devuelve un puntero a tkpt

		} else if(!(strcmp(aux, RMC_INDICATOR))) {
			if(data_structs[DATA_ZDA_S]->time_flag == TRUE)
				tkpt_flag = TRUE;
			read_nmea_rmc(f, data_structs[DATA_TKPT_S], data_structs[DATA_RMC_S]); // Incluye: si no hay dato temporal, po
		}
		
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
		return ST_DATA_ERR;
	}
	/*[12] Si el caracter se encuentra en el diccionario, se calcula su posición respecto del primer caracter del diccionario.*/
	hex1 = (size_t)ptr - (size_t)hexa_dic;
	
	if((ptr = strrchr(hexa_dic, d2)) == NULL) {
		return ST_DATA_ERR;
	}
	hex2 = (size_t)ptr - (size_t)hexa_dic;
	
	/*[13] Si la conversion resulta invalida devuelve -1. De otro modo, realiza la conversion.*/
	if(hex1 >= HEX_DIGITS || hex2 >= HEX_DIGITS) {
		return ST_DATA_ERR;
	}

	*dec_num = (int)hex1 * HEX_DIGITS + hex2;

	return ST_OK;

}

status_t get_nmea_data(unsigned int qfields, char *** string, FILE * f, int * current_checksum) {
	
	size_t i,j;
	int c, nmea_checksum;
	bool_t flag_end = FALSE;


	if(!string || !f || !current_checksum)
		return ST_NULL_PTR;

	for(j = 0; j < qfields; j++) {
		for(i = 0; (c = (*string)[j][i] = fgetc(f))!= EOF && c != CHAR_DELIM && c != CHAR_INIT_NMEA && c != CHAR_END_NMEA; i++) {
			*current_checksum ^= c;
		}
		if(c == CHAR_DELIM || (c == CHAR_END_NMEA && (j == qfields - 1))
			(*string)[j][i] = '\0'; // piso la coma
			if(c != CHAR_DELIM)
				flag_end = TRUE; // veo si se alcanzó un '*'
		else
			return ST_IGNORE_STATEMENT;
	}

	if(flag_end != TRUE)
		while((c = fgetc(f)) != CHAR_END_NMEA && c != EOF) // si no se había alcanzado el '*' sigue haciendo checksum hasta encontrarlo
			*current_checksum ^= c;

	hexstring_2_integer(fgetc(f), fgetc(f), &nmea_checksum); //validar

	if(nmea_checksum != *current_checksum)
		return ST_IGNORE_STATEMENT;

	return ST_OK;
}


/*[14] Carga los datos de tiempo en la estructura trackpt. Lee de la sentencia NMEA (apuntada por pos_ptr) horas, minutos y segundos. El año, mes y dia los recibe de la estructura metadata.
	   Devuelve: ST_OK si los datos se cargan correctamente y ST_NUMERICAL_ERROR si los datos son invalidos.*/
status_t gga_time_of_fix(char * s, gga_s * gga, arg_s * arg) {
	
	char aux[STR_NMEA_DATA_DIGITS], *end_ptr;
	int hours, minutes, seconds, miliseconds, read_digits;
	size_t i;

	if(!s || !gga || !arg) {
		return ST_NULL_PTR;
	}

	strncpy(aux, s, HOUR_DIGITS); // validar
	read_digits = HOUR_DIGITS;
	aux[read_digits] = '\0';

	hours = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;	
	}

	strncpy(aux, s + read_digit, MINUTES_DIGITS); // validar
	read_digits += MINUTES_DIGITS;
	aux[read_digits] = '\0';

	minutes = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;	
	}

	strncpy(aux, s + read_digit, SECONDS_DIGITS); // validar
	read_digits += SECONDS_DIGITS;
	aux[read_digits] = '\0';

	seconds = strtof(aux, &end_ptr);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}
	
	strncpy(aux, s + read_digit + 1, MILISECONDS_DIGITS); // validar, sumo uno por el punto
	read_digits += MILISECONDS_DIGITS + 1;
	aux[read_digits] = '\0';

	miliseconds = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}

	(gga->gga_time).tm_hour = hours;
	(gga->gga_time).tm_min = minutes;   
	(gga->gga_time).tm_sec = seconds;
	gga->gga_time_milisec = miliseconds;
	(gga->gga_time).tm_year = arg->time->tm_year; 
	(gga->gga_time).tm_mon = arg->time->tm_mon;
	(gga->gga_time).tm_mday = arg->time->tm_mday;

	return ST_OK;
}

/*[19] Lee la latitud de la sentencia NMEA y la guarda en una variable double (trackpt.latitude). Recibe el puntero pos_ptr apuntado al primer caracter de la latitud.
	   Devuelve: ST_OK si la lectura se realiza correctamente y ST_NUMERICAL_ERROR si los datos son invalidos.*/
status_t latitude(char * str1, char * str2, gga_s * gga) { 

	char aux[STR_NMEA_DATA_DIGITS]
	double lat;
	char *end_ptr;
	size_t degrees, i;
	int south_flag = 1, read_digits;
	float minutes;

	if(!str1 || !str2 || !gga) {
		return ST_NULL_PTR;
	}

	/*[20] Lee los grados y minutos, los convierte a numeros y realiza el pasaje a grados.*/
	strncpy(aux, str1, NMEA_LATITUDE_DEGREES); // validar
	read_digits = NMEA_LATITDE_DEGREES;
	aux[read_digits] = '\0';

	degrees = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}
	
	strncpy(aux, str1 + read_digit, NMEA_LATITUDE_MINUTES); // validar
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
	gga->latitude = lat;


	return ST_OK;
}

status_t longitude(char * str1, char * str2, gga_s * gga) {

	char aux[STR_NMEA_DATA_DIGITS]
	double lon;
	char *end_ptr;
	size_t degrees, i;
	int west_flag = 1, read_digits;
	float minutes;

	if(!str1 || !str2 || !gga) {
		return ST_NULL_PTR;
	}

	strncpy(aux, str1, NMEA_LONGITUDE_DEGREES); // validar
	read_digits = NMEA_LONGITUDE_DEGREES;
	aux[read_digits] = '\0';

	degrees = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}

	strncpy(aux, str1 + read_digit, NMEA_LONGITUDE_MINUTES); // validar
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
	gga->longitude = lon;

	return ST_OK;
}


/*[23] Lee la calidad del fix y la carga en la estructura.
	   Devuelve: ST_OK si la lectura se realiza correctamente y ST_NUMERICAL_ERROR si los datos son invalidos.*/
status_t gga_quality_of_fix(char * s, gga_s * gga) {
	/*[24] Carga el numerodo de la calidad del fix en una variable. Comprueba que sea un numero valido y lo carga en la estructura.*/
	int option = (int)s[0] - ZERO_ASCII_VALUE;

	if(!s || !gga) {
		return ST_NULL_PTR;
	}

	if(! isdigit(s[0]) {
		return ST_INVALID_NUMBER_ERROR;
	}
	if(option > MAX_QUALITY) {
		return ST_INVALID_NUMBER_ERROR;
	}
		
	gga->quality = option;


	return ST_OK;		
}

/*[25] Lee la cantidad de satelites y la carga en la estructura.
	   Devuelve: ST_OK si la lectura se realiza correctamente y ST_INVALID_NUMBER_ERROR si los datos son invalidos.*/
status_t gga_num_of_satellites(char * s, gga_s * gga) {

	char *end_ptr;
	size_t i, satellites;

	if(!s || !gga) 
		return ST_NULL_PTR;


	satellites = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0') {
		return ST_INVALID_NUMBER_ERROR;
	}
	if(satellites < MIN_SAT || satellites > MAX_SAT) 
		return ST_INVALID_NUMBER_ERROR;
	

	gga->n_sat = satellites;

	return ST_OK;
}

/*[26] Lee hdop y la carga en la estructura.
	   Devuelve: ST_OK si la lectura se realiza correctamente y ST_NUMERICAL_ERROR si los datos son invalidos.*/
status_t gga_hdop(char * s, gga_s * gga) {
	
	size_t i;
	char *end_ptr;

	if(!s || !gga) {
		return ST_NULL_PTR;
	}

	hdop = strtof(s, &end_ptr);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}

	gga->hdop = hdop;

	return ST_OK;
}

/*[27] Lee la elevacion y la carga en la estructura.
	   Devuelve: ST_OK si la lectura se realiza correctamente y ST_NUMERICAL_ERROR si los datos son invalidos.*/
status_t gga_elevation(char * s, gga_s * gga) {
	
	size_t i;
	char *end_ptr;

	if(!s || !gga) 
		return ST_NULL_PTR;

	elevation = strtof(s, &end_ptr);
	if(*end_ptr != '\0') {
		return ST_NUMERICAL_ERROR;
	}

	/*[28] Ignora las unidades de la elevacion.*/

	gga->elevation = elevation;

	return ST_OK;
}

/*[28] Lee la separacion del geoide y la carga en la estructura.
	   Devuelve: ST_OK si la lectura se realiza correctamente y ST_NUMERICAL_ERROR si los datos son invalidos.*/
status_t gga_undulation_of_geoid(char * s, gga_s * gga) {
	
	size_t i;
	char *aux_ptr;

	if(!s || !undulation) {
		return ST_NULL_PTR;
	}

	undulation = strtof(aux, &aux_ptr);
	if(*aux_ptr != '\0' && *aux_ptr != '\n') {
		return ST_NUMERICAL_ERROR;
	}

	gga->undulation_of_geoid = undulation;

	return ST_OK;
}




status_t read_nmea_gga(FILE * f, tkpt_s * tkpt, gga_s * gga, int checksum) {
	
	char * s[GGA_FIELDS];

	if(!f || !tkpt || !gga)
		return ST_NULL_PTR;

	get_nmea_data(GGA_FIELDS, &s, f, &checksum); // validar
	gga_time_of_fix(s[GGA_TIME_FIELD], gga, struct tm arg); // chequear BIEN lo de struct tm arg (está mal, pero cómo iría?)
	latitude(s[GGA_LAT_FIELD], s[GGA_LAT_N_S_FIELD], gga);
	longitude(s[GGA_LONG_FIELD], s[GGA_LONG_W_E_FIELD], gga);
	gga_quality_of_fix(s[GGA_FIX_FIELD], gga);
	gga_num_of_satellites(s[GGA_SAT_FIELD], gga);
	gga_hdop(s[GGA_HDOP_FIELD], gga);
	gga_elevation(s[GGA_ELEV_FIELD], gga);
	gga_undulation_of_geoid(s[GGA_UNDULATION_FIELD], gga);

	return ST_OK;
}

status_t read_nmea_zda(FILE * f, tkpt_s * tkpt, zda_s * zda, int checksum) {
	
	char * s[ZDA_FIELDS];

	if(!f || !tkpt || !gga)
		return ST_NULL_PTR;
	
	get_nmea_data(ZDA_FIELDS, &s, f, &checksum); // validar


}

status_t read_nmea_rmc(FILE * f, tkpt_s * tkpt, rmc_s * rmc, int checksum) {
	
	char * s[RMC_FIELDS];

	if(!f || !tkpt || !gga)
		return ST_NULL_PTR;
	
	get_nmea_data(RMC_FIELDS, &s, f, &checksum); // validar


}

status_t gga2tkpt(tkpt_s * tkpt, gga_s * gga) {
	
	if(!tkpt || !gga)
		return ST_NULL_PTR;

	tkpt->tkpt_time = gga->gga_time; // ???
	tkpt->tkpt_msec = gga->gga_time_milisec;
	tkpt->latitude = gga->latitude;
	tkpt->longitude = gga->longitude;
	tkpt->elevation = gga->elevation;	

	return ST_OK;

}


status_t zda2tkpt(tkpt_s * tkpt, zda_s * zda) {
	
	if(!tkpt || !zda)
		return ST_NULL_PTR;

	tkpt->tkpt_time = zda->zda_time; // ???
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