

#define MAX_PROCESS_T 3


typedef enum {PROCESS_ZERO, PROCESS_ONE, PROCESS_TWO, PROCESS_ALL} process_t;


int main(int argc, const char argv **) {

	arg_s * metadata;
	status_t st;
	tkpt_s * tkpt;
	List * list;
	process_t ran;

	srand(time(NULL));

	metadata = arg_create(&st);

	if(st != ST_OK) {
		/* REPORTAR ERRORES */
		return 0; /* EXIT_FAILURE/SUCCESS LIBERAR MEMORIA Y CERRAR ARCHIVOS */
	}

	st = arg_load(argc, argv, metadata);

	if(st != ST_OK) {
		/* REPORTAR ERRORES */
		return 0; /* EXIT_FAILURE/SUCCESS LIBERAR MEMORIA Y CERRAR ARCHIVOS */
	}

	status_t protocol_detect(metadata_t *); // ####

	if(list_create(list) != ST_OK)
		return ST_NO_MEM; /* LIBERAR MEMORIA Y CERRAR ARCHIVOS */

	while(! (tkpt = get_trackpoint(metadata))) {

		if(list_append(list, tkpt, metadata->maxlen) != ST_OK) // ####
			return ERROR; /* LIBERAR MEMORIA Y CERRAR ARCHIVOS */

		ran = rand() % MAX_PROCESS_T;

		if(gpx_process(list, ran) != ST_OK) // ####
			return ERROR; /* LIBERAR MEMORIA Y CERRAR ARCHIVOS */

	}

	gpx_process(list, PROCESS_ALL)

	close_all(metadata)

	free_all(list, tkpt, )

	return EXIT_SUCCESS;

}


tkpt_t * get_trackpoint(arg_s * metadata) {

	if(metadata->protocol = P_NMEA)
		return get_trackpoint_nmea(metadata); // ###

	return get_trackpoint_ubx(metadata); // ###

}
