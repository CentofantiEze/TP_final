#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "arg_load.c"
#include "structures.c"
#include "trackpoint.c"

int main(int argc, const char ** argv) {

	arg_s * metadata = NULL;
	status_t st;
	List * list = NULL;
	process_t ran;
	status_t (*get_trackpoint[])(FILE *, void **) = {get_trackpoint_nmea, get_trackpoint_ubx};
	int data_protocol;
	void * data_structs[DATA_S_TYPES];

	srand(time(NULL));

	metadata = arg_create(&st);

	if(st != ST_OK) {
		free_metadata(metadata);
		close_files(metadata);
		return st;
	}

	st = arg_load(argc, argv, metadata);

	if(st != ST_OK) {
		free_metadata(metadata);
		close_files(metadata);
		return st;
	}

	if(metadata->protocol == P_AUTO)
		if((st = protocol_detect(metadata)) != ST_OK) {
			free_metadata(metadata);
			close_files(metadata);
			return 0;
		}

	

	if(list_create(list) != ST_OK) {
		free_metadata(metadata);
		close_files(metadata);
		return ST_NO_MEM;
	}

	if((st = data_structs_create(metadata, data_structs)) != ST_OK) {
		free_metadata(metadata);
		close_files(metadata);
		list_delete(list);
		return st;
	}

/***************************************************************************/

	data_protocol = metadata->protocol - 1;

	while(get_trackpoint[data_protocol](metadata->infile, data_structs)) { // preguntar

		if((st = list_append_tkpt(list, data_structs[DATA_TKPT_S], metadata->maxlen)) != ST_OK) {
			free_metadata(metadata);
			close_files(metadata);
			list_delete(list);
			free_data_structs(data_structs);
			return st;
		}

		ran = rand() % MAX_PROCESS_T;

		if((st = gpx_process(list, ran)) != ST_OK) { // ####
			free_metadata(metadata);
			close_files(metadata);
			list_delete(list);
			free_data_structs(data_structs);
			return st;
		}

	}

	gpx_process(list, PROCESS_ALL);

	free_metadata(metadata);
	close_files(metadata);
	list_delete(list);
	free_data_structs(data_structs);
	return EXIT_SUCCESS; 

}


/*

TO DO:

status_t gpx_process(List *, proces_t);

*/




