#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "help.h"
#include "help.c"

#include "structs_and_types.h"

#include "logs.h"
#include "logs.c"

#include "arg_load.h"
#include "arg_load.c"

#include "lists.h"
#include "lists.c"

#include "print_gpx.h"
#include "print_gpx.c"

#include "add_functions.h"
#include "add_functions.c"

#include "get_tkpt_nmea.h"
#include "get_tkpt_nmea.c"

#include "get_tkpt_ubx.h"
#include "get_tkpt_ubx.c"


int main(int argc, const char ** argv) {

	arg_s * metadata = NULL;
	status_t st;
	List * list = NULL;
	data_structs_s * data_structs = NULL;
	status_t (*get_tkpt[])(arg_s *, data_structs_s *) = {get_tkpt_nmea, get_tkpt_ubx};
	int i;
	bool_t flag_end = FALSE;
	process_t ran;
	srand(time(NULL));

	printf("%s\n", HELP_MSJ_START);

	metadata = arg_create(&st);

	if(st != ST_OK) {
		free_metadata(metadata);
		return EXIT_FAILURE;
	}

	st = arg_load(argc, argv, metadata);

	if(st != ST_OK) {
		if(st == ST_ERR_OPEN_IN_FILE || st == ST_ERR_OPEN_OUT_FILE || st == ST_ERR_OPEN_LOG_FILE)
			log_print(stdin, OPEN_FILE);
		close_files(metadata);
		free_metadata(metadata);
		print_help();
		return EXIT_FAILURE;
	}

	if(metadata->help == TRUE) {
		close_files(metadata);
		free_metadata(metadata);
		print_help();
		return EXIT_SUCCESS;
	}

	st = protocol_detect(metadata);

	if(st != ST_OK) {
		log_print(metadata->logfile, INTERNAL_ERROR);
		close_files(metadata);
		free_metadata(metadata);
		return EXIT_FAILURE;
	}

	if((st = list_create(&list)) != ST_OK) {
		log_print(metadata->logfile, INTERNAL_ERROR);
		close_files(metadata);
		free_metadata(metadata);
		printf("%d\n",st);
		return EXIT_FAILURE;
	}

	if((st = data_structs_create(metadata, &data_structs)) != ST_OK) {
		log_print(metadata->logfile, INTERNAL_ERROR);
		close_files(metadata);
		free_metadata(metadata);
		list_delete(list);
		return EXIT_FAILURE;
	}

	gpx_open(metadata);

	while(! flag_end) {
		st = get_tkpt[metadata->protocol - 1](metadata, data_structs);
		if(st != ST_OK) {
			i--;
			if(st == ST_ERROR_EOF) {
				flag_end = TRUE;
			if(st == ST_CORRUPT_FILE) {
				log_print(metadata->logfile, READ_FILE);
				flag_end = TRUE;
			}

			}
		} else {
			st = list_append_tkpt(list, data_structs->tkpt, metadata->maxlen);
			if(st != ST_OK) {
				log_print(metadata->logfile, INTERNAL_ERROR);
				close_files(metadata);
				free_metadata(metadata);
				free_data_structs(data_structs);
				list_delete(list);
				return EXIT_FAILURE;
			}

			ran = (process_t)((int)rand() % MAX_PROCESS_T);

			if((st = gpx_process(metadata, list, ran)) != ST_OK) {
				log_print(metadata->logfile, INTERNAL_ERROR);
				close_files(metadata);
				free_metadata(metadata);
				free_data_structs(data_structs);
				list_delete(list);
				return EXIT_FAILURE;
			}
		}
	}

	if((st = gpx_process(metadata, list, PROCESS_ALL)) != ST_OK) {
		log_print(metadata->logfile, INTERNAL_ERROR);
		close_files(metadata);
		free_metadata(metadata);
		free_data_structs(data_structs);
		list_delete(list);
		return EXIT_FAILURE;
	}

	gpx_close(metadata->outfile);

	log_print(metadata->logfile, SUCCESSFUL_EXIT);
	close_files(metadata);
	free_metadata(metadata);
	free_data_structs(data_structs);
	list_delete(list);
	return EXIT_SUCCESS;
}