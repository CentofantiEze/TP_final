#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "arg_load.c"
#include "structures.c"
#include "trackpoint.c"

#define SYNC_1 0
#define SYNC_2 0
#define CHAR_INIT_NMEA '$'

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


status_t protocol_detect(arg_s * metadata);
status_t data_structs_create(arg_s * metadata, void * data_structs[]);
void close_files(arg_s * metadata);
void free_metadata(arg_s * metadata);
List * list_delete(List * list);
void free_data_structs(void * data_structs[]);
status_t gpx_process(List *, process_t);


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

void close_files(arg_s * metadata) {

	if(! metadata)
		return;

	if(! metadata->infile_default)
		fclose(metadata->infile);
	else if(metadata->protocol == P_UBX)
		freopen(NULL, "rt", stdin);

	if(! metadata->outfile_default)
		fclose(metadata->outfile);

	if(! metadata->logfile_default)
		fclose(metadata->logfile);

}

void free_metadata(arg_s * metadata) {

	if(! metadata)
		return;

	free(metadata->name);
	free(metadata->infile_name);
	free(metadata);
	metadata = NULL;

}

List * list_delete(List * list) {

	Node * aux1, * aux2;

	if(! list)
		return NULL;

	if(! list->first_node) {
		free(list);
		return NULL;
	}

	aux1 = list->first_node;

	while(aux1->next) {
		aux2 = aux1;
		aux1 = aux1->next;
	}

	free(aux1->data);
	free(aux2->next);
	aux2->next = NULL;

	return list_delete(list);

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




