/* Prueba de protocol detect */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define OK "Ok"
#include "structs_and_types.h"
#include "arg_load.h"
#include "arg_load.c"

int main(int argc, const char ** argv) {

	arg_s * metadata = NULL;
	status_t st;

	metadata = arg_create(&st);

	if(st != ST_OK) {
		free_metadata(metadata);
		return st;
	}

	puts(OK);

	st = arg_load(argc, argv, metadata);

	puts(OK);
	printf("%d\n", st);

	if(st != ST_OK) {
		close_files(metadata);
		puts(OK);
		free_metadata(metadata);
		return st;
	}

	return 0;

}