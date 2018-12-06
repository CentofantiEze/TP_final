status_t protocol_detect(arg_s * metadata) {

	unsigned char aux;

	if(!metadata)
		return ST_NULL_PTR;

	if(metadata->protocol == P_NMEA ||metadata->protocol == P_UBX)
		return ST_OK;

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


status_t data_structs_create(arg_s * metadata, data_structs_s ** data_structs) {

	if(! metadata || ! data_structs)
		return ST_NULL_PTR;
	
	if((*data_structs = (data_structs_s *)calloc(1, sizeof(data_structs_s))) == NULL)
		return ST_NO_MEM;

	if(metadata->protocol == P_NMEA) {

		if(((*data_structs)->rmc = (rmc_s *)calloc(1, sizeof(rmc_s))) == NULL)
			return ST_NO_MEM;

		if(((*data_structs)->gga = (gga_s *)calloc(1, sizeof(gga_s))) == NULL) {
			free_data_structs(*data_structs);
			return ST_NO_MEM;
		}

		if(((*data_structs)->zda = (zda_s *)calloc(1, sizeof(zda_s))) == NULL) {
			free_data_structs(*data_structs);
			return ST_NO_MEM;
		}

		if(((*data_structs)->tkpt = (tkpt_s *)calloc(1, sizeof(tkpt_s))) == NULL) {
			free_data_structs(*data_structs);
			return ST_NO_MEM;
		}

		return ST_OK;

	} else if(metadata->protocol == P_UBX) {

		if(((*data_structs)->nav_pvt = (nav_pvt_s *)calloc(1, sizeof(nav_pvt_s))) == NULL)
			return ST_NO_MEM;

		if(((*data_structs)->tim_tos = (tim_tos_s *)calloc(1, sizeof(tim_tos_s))) == NULL) {
			free_data_structs(*data_structs);
			return ST_NO_MEM;
		}

		if(((*data_structs)->nav_posllh = (nav_posllh_s *)calloc(1, sizeof(nav_posllh_s))) == NULL) {
			free_data_structs(*data_structs);
			return ST_NO_MEM;
		}

		if(((*data_structs)->tkpt = (tkpt_s *)calloc(1, sizeof(tkpt_s))) == NULL) {
			free_data_structs(*data_structs);
			return ST_NO_MEM;
		}

		return ST_OK;
	}

	return ST_NO_PROTOCOL;

}


void free_data_structs(data_structs_s * data_structs) {

	if(! data_structs)
		return;

	free(data_structs->gga);
	free(data_structs->rmc);
	free(data_structs->zda);
	free(data_structs->nav_pvt);
	free(data_structs->tim_tos);
	free(data_structs->nav_posllh);
	free(data_structs->tkpt);
	free(data_structs);

}


status_t gpx_process(arg_s * metadata, List * list, process_t proc) {

	status_t st;
	tkpt_s * tkpt = NULL;
	FILE * f;

	if(! list || ! metadata)
		return ST_NULL_PTR;

	f = metadata->outfile;

	if(list->len == 0)
		return ST_OK;

	if(proc == PROCESS_ZERO)
		return ST_OK;

	if(proc == PROCESS_ALL) {

		if((st = popleft(list, &tkpt)) == ST_OK) {
			
			if(gpx_tkpt(f, tkpt) != ST_OK) {
				free(tkpt);
				return ST_CORRUPT_FILE;
			}

			log_print(metadata->logfile, TKPT_PRINT);
			free(tkpt);
			return gpx_process(metadata, list,proc);
		}

		return st;
	}

	if((st = popleft(list, &tkpt)) == ST_OK) {
		
		if(gpx_tkpt(f, tkpt) != ST_OK){
			free(tkpt);
			return ST_CORRUPT_FILE;
		}

		log_print(metadata->logfile, TKPT_PRINT);
		free(tkpt);
		return gpx_process(metadata, list, --proc);
	}

	return st;
}