/* Prueba de protocol detect */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define OK "Ok"

#include "structs_and_types.h"

#include "logs.c"

#include "arg_load.h"
#include "arg_load.c"

#include "add_functions.h"
#include "add_functions.c"

#include "lists.h"
#include "lists.c"

#include "get_tkpt_nmea.h"
#include "get_tkpt_nmea.c"

#include "get_tkpt_ubx.c"

int main(int argc, const char ** argv) {

	arg_s * metadata = NULL;
	status_t st;
	List * list = NULL;
	data_structs_s * data_structs = NULL;
	status_t (*get_tkpt[])(arg_s *, data_structs_s *) = {get_tkpt_nmea, get_tkpt_ubx};
	int i;

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

	printf("protocoool_detect:%d\n", st);

	if(st != ST_OK) {
		close_files(metadata);
		free_metadata(metadata);
		return st;
	}

	printf("%d\n", metadata->protocol);

 // ultimo ok

	if((st = list_create(&list)) != ST_OK) {
		close_files(metadata);
		free_metadata(metadata);
		printf("%d\n",st);
		return ST_NO_MEM;
	}

	puts("listaok");

	printf("%d\n", metadata->protocol);

	if((st = data_structs_create(metadata, &data_structs)) != ST_OK) {
		close_files(metadata);
		free_metadata(metadata);
		list_delete(list);
		return st;
	}

	for(i = 0; i < 20; i++) {
		st = get_tkpt[metadata->protocol - 1](metadata, data_structs);
		if(st != ST_OK) {
			printf("Descartado");
			printf("%d\n", st);
			i--;
			if(st == ST_ERROR_EOF) {
				puts("EOF");
				break;
			}
		} else {
			st = list_append_tkpt(list, data_structs->tkpt, metadata->maxlen);
			if(st != ST_OK) {
				puts("algomalanda");
				printf("%d\n", st);
				return 0;
			}
			printf("nodo %d %d\n",i,st);
		}
	}

	puts("fin");

	close_files(metadata);
	free_metadata(metadata);
	free_data_structs(data_structs);
	list_delete(list);
	return 0;
}