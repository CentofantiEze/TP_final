

#define MAX_PROCESS_T 3

#define DATA_S_TYPES 4
#define DATA_GGA_S 0
#define DATA_ZDA_S 1
#define DATA_RMC_S 2
#define DATA_NAV_PVT_S 0
#define DATA_TIM_TOS_S 1
#define DATA_NAV_POSLLH_S 2 
#define DATA_TKPT_S 3

typedef enum {PROCESS_ZERO, PROCESS_ONE, PROCESS_TWO, PROCESS_ALL} process_t;


int main(int argc, const char argv **) {

	arg_s * metadata;
	status_t st;
	List * list;
	process_t ran;
	status_t (*get_trackpoint[])(arg_s *, void **) = {get_trackpoint_nmea, get_trackpoint_ubx};
	int data_protocol;
	void * data_structs[]

	srand(time(NULL));

	metadata = arg_create(&st);

	if(st != ST_OK) {
		/* LIBERAR METADATA Y ->NAME, ->INFILE_NAME, CERRAR ARCHIVOS */
		return 0;
	}

	st = arg_load(argc, argv, metadata);

	if(st != ST_OK) {
		/* LIBERAR METADATA Y ->NAME, ->INFILE_NAME, CERRAR ARCHIVOS */
		return 0;
	}

	status_t protocol_detect(metadata_t *); // ####

	if(list_create(list) != ST_OK)
		return ST_NO_MEM; /* LIBERAR: METADATA Y ->NAME, ->INFILE_NAME, CERRAR ARCHIVOS */

	if(status_t data_structs_create(arg_s *, void ***) != ST_OK) // ####
		return 0; /* LIBERAR: METADATA, ->NAME, ->INFILE_NAME, LIST, CERRAR ARCHIVOS */

	data_protocol = metadata->protocol - 1;

	while(get_trackpoint[data_protocol](metadata, data_structs)) { // preguntar

		if(list_append(list, data_structs[DATA_TKPT_S], metadata->maxlen) != ST_OK) // ####
			return ERROR; /* LIBERAR: METADATA, ->NAME, ->INFILE_NAME, LIST, DATA_STRUCTS Y CERRAR ARCHIVOS */

		ran = rand() % MAX_PROCESS_T;

		if(gpx_process(list, ran) != ST_OK) // ####
			return ERROR; /* LIBERAR: METADATA, ->NAME, ->INFILE_NAME, LIST, DATA_STRUCTS Y CERRAR ARCHIVOS */

	}

	gpx_process(list, PROCESS_ALL)

	return EXIT_SUCCESS; /* LIBERAR: METADATA, ->NAME, ->INFILE_NAME, LIST, DATA_STRUCTS Y CERRAR ARCHIVOS */

}


/*

TO DO:

status_t protocol_detect(metadata_t *);










*/