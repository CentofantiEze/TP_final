status_t protocol_detect(arg_s * metadata) {

	unsigned char aux;

	if(!metadata)
		return ST_NULL_PTR;

	if((metadata->infile = fopen(metadata->infile_name, "rb")) == NULL)
		return ST_ERR_OPEN_IN_FILE;


	do {

		while(fread(&aux, sizeof(unsigned char), 1, metadata->infile) == 1 && aux != SYNC_1 && aux != CHAR_INIT_NMEA);
		
		if(aux == CHAR_INIT_NMEA) {
			metadata->protocol = P_NMEA;
			if((metadata->infile = freopen(metadata->infile_name, "rt", metadata->infile)) == NULL)
				return ST_ERR_OPEN_IN_FILE;
			return ST_OK;
		}

		if(aux != SYNC_1)
			return ST_CORRUPT_FILE;

	} while(fread(&aux, sizeof(unsigned char), 1, metadata->infile) == 1 && aux != SYNC_2 && aux != CHAR_INIT_NMEA);

	if(aux == CHAR_INIT_NMEA) {
		metadata->protocol = P_NMEA;
		if((metadata->infile = freopen(metadata->infile_name, "rt", metadata->infile)) == NULL)
			return ST_ERR_OPEN_IN_FILE;
		return ST_OK;
	}
	
	if(aux == SYNC_2){
		metadata->protocol = P_UBX;
		if((metadata->infile = freopen(metadata->infile_name, "rb", metadata->infile)) == NULL)
			return ST_ERR_OPEN_IN_FILE;
		return ST_OK;
	}

	return ST_CORRUPT_FILE;

}


status_t data_structs_create(arg_s * metadata, void * data_structs[]) {

	if(! data_structs || ! metadata)
		return ST_NULL_PTR;

	if(metadata->protocol == P_NMEA) {

		if((data_structs[DATA_GGA_S] = (gga_s *)calloc(1, sizeof(gga_s))) == NULL)
			return ST_NO_MEM;

		if((data_structs[DATA_ZDA_S] = (zda_s *)calloc(1, sizeof(zda_s))) == NULL) {
			free(data_structs[DATA_GGA_S]);
			return ST_NO_MEM;
		}

		if((data_structs[DATA_RMC_S] = (rmc_s *)calloc(1, sizeof(rmc_s))) == NULL) {
			free(data_structs[DATA_GGA_S]);
			free(data_structs[DATA_ZDA_S]);
			return ST_NO_MEM;
		}

		if((data_structs[DATA_TKPT_S] = (tkpt_s *)calloc(1, sizeof(tkpt_s))) == NULL) {
			free(data_structs[DATA_GGA_S]);
			free(data_structs[DATA_ZDA_S]);
			free(data_structs[DATA_RMC_S]);
			return ST_NO_MEM;
		}

		return ST_OK;

	} else if(metadata->protocol == P_UBX) {

		if((data_structs[DATA_NAV_PVT_S] = (nav_pvt_s *)calloc(1, sizeof(nav_pvt_s))) == NULL)
			return ST_NO_MEM;

		if((data_structs[DATA_TIM_TOS_S] = (tim_tos_s *)calloc(1, sizeof(tim_tos_s))) == NULL) {
			free(data_structs[DATA_NAV_PVT_S]);
			return ST_NO_MEM;
		}

		if((data_structs[DATA_NAV_POSLLH_S] = (nav_posllh_s *)calloc(1, sizeof(nav_posllh_s))) == NULL) {
			free(data_structs[DATA_NAV_PVT_S]);
			free(data_structs[DATA_TIM_TOS_S]);
			return ST_NO_MEM;
		}

		if((data_structs[DATA_TKPT_S] = (tkpt_s *)calloc(1, sizeof(tkpt_s))) == NULL) {
			free(data_structs[DATA_NAV_PVT_S]);
			free(data_structs[DATA_TIM_TOS_S]);
			free(data_structs[DATA_NAV_POSLLH_S]);
			return ST_NO_MEM;
		}

		return ST_OK;
	}

	return ST_NO_PROTOCOL;

}


void free_data_structs(void * data_structs[]) {

	size_t i;

	if(! data_structs)
		return;

	for(i = 0; i < DATA_S_TYPES; i++)
		free(data_structs[i]);

}


status_t gpx_process(List *, process_t) {

	return ST_OK;

}