#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define ARG_MAX_LEN 12
#define ARG_NUM 14
#define ARG_TYPES 7
#define ARG_TYPE_HELP 0
#define ARG_TYPE_NAME 1
#define ARG_TYPE_PROTOCOL 2
#define ARG_TYPE_INFILE 3
#define ARG_TYPE_OUTFILE 4
#define ARG_TYPE_LOGFILE 5
#define ARG_TYPE_MAXLEN 6

typedef enum {FALSE, TRUE} bool_h;
typedef enum {NMEA, UBX, AUTO} protocol_t;
typedef enum {ST_OK, ST_NULL_PTR, ST_INVALID_ARGUMENT} status_t;

const char valid_arguments[][ARG_MAX_LEN] = {
	"-h", "--help",
	"-n", "--name",
	"-p", "--protocol",
	"-i", "--infile",
	"-o", "--outfile",
	"-l", "--logfile",
	"-m", "--maxlen"
};


struct arg {
	struct tm time;
	bool_h help;
	char * name;
	protocol_t protocol;
	FILE * infile;
	FILE * outfile;
	FILE * logfile;
	size_t maxlen;
};

typedef struct arg arg_s;


status_t arg_load(int  , const char **, arg_s *);
status_t set_help(arg_s *);
status_t set_name(arg_s *, char *);
status_t set_protocol(arg_s *, char *);
status_t set_infile(arg_s *, char *);
status_t set_outfile(arg_s *, char *);
status_t set_logfile(arg_s *, char *);
status_t set_maxlen(arg_s *, char *);



status_t arg_load(int argc, const char ** argv, arg_s * metadata_io) {

	size_t i, j;

	/* Se utiliza para saber que argumentos ya se procesaron, para evitar repeticiones */
	
	int arg_flags[ARG_TYPES] = {0};


	if(! metadata_io)
		return ST_NULL_PTR;

	/* Para cada argumento en argv se compara con los argumentos validos */

	for(i = 1; i < argc; i++) {

		for(j = 0; j < ARG_NUM; j++) {

			if(! strcmp(argv[i], valid_arguments[j])) {

				switch(j) {

					case 0 :
					case 1 :

						/* Crear: status_t set_help(arg_s *); */
						set_help(metadata_io);

						arg_flags[j/2]++;
						break;

					case 2 :
					case 3 :

						if(++i == argc) // si -n esta en el ultimo lugar de argv no puede leer el nombre
							return ST_INVALID_ARGUMENT;

						/* Crear: status_t set_name(arg_s *, char *); */
						if(set_name(metadata_io, argv[i]) == ESTADOS_DE_ERROR)
							return ERROR;

						arg_flags[j/2]++;
						break;

					case 4 :
					case 5 :

						if(++i == argc)
							return ST_INVALID_ARGUMENT;

						/* Crear: status_t set_protocol(arg_s *, char *); */
						if(set_protocol(metadata_io, argv[i]) == ESTADOS_DE_ERROR)
							return ERROR;

						arg_flags[j/2]++;
						break;

					case 6 :
					case 7 :

						if(++i == argc)
							return ST_INVALID_ARGUMENT;

						/* Crear: status_t set_infile(arg_s *, char *); */
						if(set_infile(metadata_io, argv[i]) == ESTADOS_DE_ERROR)
							return ERROR;

						arg_flags[j/2]++;
						break;

					case 8 :
					case 9 :

						if(++i == argc)
							return ST_INVALID_ARGUMENT;

						/* Crear: status_t set_outfile(arg_s *, char *); */
						if(set_outfile(metadata_io, argv[i]) == ESTADOS_DE_ERROR)
							return ERROR;

						arg_flags[j/2]++;
						break;

					case 10 :
					case 11 :

						if(++i == argc)
							return ST_INVALID_ARGUMENT;

						/* Crear: status_t set_logfile(arg_s *, char *); */
						if(set_logfile(metadata_io, argv[i]) == ESTADOS_DE_ERROR)
							return ERROR;

						arg_flags[j/2]++;
						break;

					case 12 :
					case 13 :

						if(++i == argc)
							return ST_INVALID_ARGUMENT;

						if(set_maxlen(metadata_io, argv[i]) == ESTADOS_DE_ERROR)
							return ERROR;

						arg_flags[j/2]++;
						break;

					default :

						return ST_INVALID_ARGUMENT;

				}

				/* Si se encuentra el tipo de argumento no se sigue buscando en valid_arguments */
				j = ARG_NUM;

			}
		}
	}

	/* Crear: status_t set_time(arg_s *) */
	if(set_time(metadata_io) == ESTADOS_DE_ERROR)
		return ERROR;

	if(arg_flags[ARG_TYPE_HELP] == 0)
		arg_flags[ARG_TYPE_HELP]++;

	if(arg_flags[ARG_TYPE_NAME] == 0) {

		if(set_name(metadata_io, DEFAULT_NAME) == ESTADOS_DE_ERROR)
			return ERROR;

		arg_flags[ARG_TYPE_NAME]++;

	}

	/*  Debe comprobarse que los campos obligatorios de la estructura se hayan completado. (protocol, infile, outfile, logfile, maxlen)
		Se utiliza el vector de flags del 2do al 6to argumento */

	for(i = 0; i < ARG_TYPES; i++) {

		if(arg_flags[i] != 1)
			return ST_INVALID_ARGUMENT;
		
	}

	return ST_OK;
}