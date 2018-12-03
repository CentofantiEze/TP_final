/* Prueba de protocol detect */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define OK "Ok"

#include "structs_and_types.h"

#include "arg_load.h"
#include "arg_load.c"

#include "add_functions.h"
#include "add_functions.c"

#include "lists.h"
#include "lists.c"

int main(int argc, const char ** argv) {

	arg_s * metadata = NULL;
	status_t st;
	List * list = NULL;
	data_structs_s * data_structs = NULL;



	metadata = arg_create(&st);

	if(st != ST_OK) {
		free_metadata(metadata);
		return st;
	}

	st = arg_load(argc, argv, metadata);

	printf("arg_load: %d\n", st);

	if(st != ST_OK) {
		close_files(metadata);
		free_metadata(metadata);
		return st;
	}

	st = protocol_detect(metadata);

	printf("protocol_detect:%d\n", st);

	if(st != ST_OK) {
		close_files(metadata);
		free_metadata(metadata);
		return st;
	}

	printf("%d\n", metadata->protocol);

 // ultimo ok

	if((st = list_create(list)) != ST_OK) {
		close_files(metadata);
		free_metadata(metadata);
		printf("%d\n",st);
		return ST_NO_MEM;
	}

	puts("listaok");

	printf("%d\n", metadata->protocol);

	if((st = data_structs_create(metadata, data_structs)) != ST_OK) {
		close_files(metadata);
		free_metadata(metadata);
		list_delete(list);
		return st;
	}

	data_structs->gga->latitude = 24;
	printf("%f\n", data_structs->rmc->latitude);
	printf("%f\n", data_structs->rmc->longitude);

	list->len = 24;

	close_files(metadata);
	free_metadata(metadata);
	list_delete(list);
	return 0;
}