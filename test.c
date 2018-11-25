#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define ARG_MAX_LEN 12 // CANTIDAD MAXIMA DE CARACTERES DE LOS ARGUMENTOS
#define ARG_NUM 14 // CANTIDAD DE ARGUMENTOS
#define PROTOCOL_MAX_LEN 5 // CANTIDAD MAXIMA DE CARACTERES DE LOS TIPOS DE PROTOCOLO
#define PROTOCOL_NUM 3 // CANTIDAD DE PROTOCOLOS
#define ARG_TYPES 7 // CANTIDAD DE TIPOS DE ARGUMENTOS
// SEPARO LOS ARGUMENTOS EN 7 TIPOS. SE UTILIZA PARA ASEGURAR QUE NO SE INGRESE UN MISMO TIPO DE ARGUMENTO MAS DE UNA VEZ
#define ARG_TYPE_HELP 0
#define ARG_TYPE_NAME 1
#define ARG_TYPE_PROTOCOL 2
#define ARG_TYPE_INFILE 3
#define ARG_TYPE_OUTFILE 4
#define ARG_TYPE_LOGFILE 5
#define ARG_TYPE_MAXLEN 6

#define DEFAULT_NAME "Default_name"
#define DEFAULT_FILE_STR "-"

#define MODE_READ_TEXT "rt"
#define MODE_READ_BINARY "rb"
#define MODE_WRITE_TEXT "wt"

#define MAX_MAXLEN 100
#define DEFAULT_MAXLEN "100"


typedef enum {FALSE, TRUE} bool_t;
typedef enum {NMEA = 1, UBX = 2, AUTO = 3} protocol_t;
typedef enum {ST_OK, ST_NULL_PTR, ST_INVALID_ARGUMENT, ST_NO_MEM, ST_ERR_OPEN_IN_FILE, ST_ERR_OPEN_OUT_FILE, ST_ERR_OPEN_LOG_FILE, ST_NO_PROTOCOL} status_t;

const char valid_arguments[][ARG_MAX_LEN] = {
	"-h", "--help",
	"-n", "--name",
	"-p", "--protocol",
	"-i", "--infile",
	"-o", "--outfile",
	"-l", "--logfile",
	"-m", "--maxlen"
};

const char protocols[][PROTOCOL_MAX_LEN] = {"nmea", "ubx", "auto"};


struct arg {
	struct tm * time;
	bool_t help;
	char * name;
	protocol_t protocol;
	FILE * infile;
	FILE * outfile;
	FILE * logfile;
	bool_t infile_default;
	bool_t outfile_default;
	bool_t logfile_default;
	size_t maxlen;
};

typedef struct arg arg_s;


arg_s * arg_create(status_t *);
status_t arg_load(int  , const char **, arg_s *);
status_t arg_set_help(arg_s *);
status_t arg_set_name(arg_s *, const char *);
status_t arg_set_protocol(arg_s *, const char *);
status_t arg_set_infile(arg_s *, const char *);
status_t arg_set_outfile(arg_s *, const char *);
status_t arg_set_logfile(arg_s *, const char *);
status_t arg_set_maxlen(arg_s *, const char *);




int main(int argc, const char ** argv) {

	arg_s * metadataaaa;
	status_t stat;

	metadataaaa = arg_create(&stat);

	if(stat != ST_OK) {
		puts("no se creo nada");
		return 0;
	}


	stat = arg_load(argc, argv, metadataaaa);

	printf("%d\n\n", stat);

	if(stat != ST_OK)
		return 0;

	printf("%d/%d/%d\n%d:%d:%d\n", metadataaaa->time->tm_mday, metadataaaa->time->tm_mon, metadataaaa->time->tm_year + 1900, metadataaaa->time->tm_hour, metadataaaa->time->tm_min, metadataaaa->time->tm_sec);

	printf("%s\n", metadataaaa->help ? "Help" : "No help");

	printf("%s\n", metadataaaa->name);

	printf("Protocolo: %s\n", protocols[metadataaaa->protocol - 1]);

	printf("Default input: %s\n", metadataaaa->infile_default ? "ON" : "OFF");
	printf("Default output: %s\n", metadataaaa->outfile_default ? "ON" : "OFF");
	printf("Default error: %s\n", metadataaaa->logfile_default ? "ON" : "OFF");

	printf("Maxlen: %lu\n", metadataaaa->maxlen);

	return 0;

}



status_t arg_load(int argc, const char ** argv, arg_s * metadata_io) {

	size_t i, j;
	status_t st;
	time_t actual_time = time(NULL);

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

						arg_set_help(metadata_io);

						arg_flags[j/2]++;
						break;

					case 2 :
					case 3 :

						if(++i == argc) // si -n esta en el ultimo lugar de argv no puede leer el nombre
							return ST_INVALID_ARGUMENT;

						st = arg_set_name(metadata_io, argv[i]);
						
						if(st == ST_NO_MEM || st == ST_NULL_PTR)
							return st;

						arg_flags[j/2]++;
						break;

					case 4 :
					case 5 :

						if(++i == argc)
							return ST_INVALID_ARGUMENT;

						st = arg_set_protocol(metadata_io, argv[i]);

						if(st == ST_NULL_PTR || st == ST_INVALID_ARGUMENT)
							return st;

						arg_flags[j/2]++;
						break;


					case 6 :
					case 7 :

						if(++i == argc)
							return ST_INVALID_ARGUMENT;

						st = arg_set_infile(metadata_io, argv[i]);
						
						if(st == ST_NULL_PTR || st == ST_ERR_OPEN_IN_FILE || st == ST_NO_PROTOCOL)
							return st;

						arg_flags[j/2]++;
						break;

					case 8 :
					case 9 :

						if(++i == argc)
							return ST_INVALID_ARGUMENT;

						st = arg_set_outfile(metadata_io, argv[i]);
						
						if(st == ST_NULL_PTR || st == ST_ERR_OPEN_OUT_FILE)
							return st;

						arg_flags[j/2]++;
						break;

					case 10 :
					case 11 :

						if(++i == argc)
							return ST_INVALID_ARGUMENT;

						st = arg_set_logfile(metadata_io, argv[i]);
						
						if(st == ST_NULL_PTR || st == ST_ERR_OPEN_LOG_FILE)
							return st;

						arg_flags[j/2]++;
						break;

					case 12 :
					case 13 :

						if(++i == argc)
							return ST_INVALID_ARGUMENT;

						st = arg_set_maxlen(metadata_io, argv[i]);

						if(st == ST_NULL_PTR || st == ST_INVALID_ARGUMENT)
							return st;

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

	metadata_io->time = gmtime(&actual_time);

	if(arg_flags[ARG_TYPE_HELP] == 0)
		arg_flags[ARG_TYPE_HELP]++;

	if(arg_flags[ARG_TYPE_NAME] == 0) {

		st = arg_set_name(metadata_io, DEFAULT_NAME);
						
		if(st == ST_NO_MEM || st == ST_NULL_PTR)
			return st;

		arg_flags[ARG_TYPE_NAME]++;

	}

	if(arg_flags[ARG_TYPE_MAXLEN] == 0) {

		st = arg_set_maxlen(metadata_io, DEFAULT_MAXLEN);

		if(st == ST_NULL_PTR || st == ST_INVALID_ARGUMENT)
			return st;

		arg_flags[ARG_TYPE_MAXLEN]++;
	}

	/*  Debe comprobarse que los campos obligatorios de la estructura se hayan completado. (protocol, infile, outfile, logfile, maxlen)
		Se utiliza el vector de flags del 2do al 6to argumento */

	for(i = 0; i < ARG_TYPES; i++) {

		if(arg_flags[i] != 1) 
			return ST_INVALID_ARGUMENT;
		
	}

	return ST_OK;
}


arg_s * arg_create(status_t * st) {

	arg_s * aux = NULL;

	if(! st)
		return NULL;

	aux = (arg_s *)calloc(1, sizeof(arg_s));

	if(aux)
		*st = ST_OK;
	else
		*st = ST_NO_MEM;

	return aux;
}

status_t arg_set_help(arg_s * metadata_io) {

	if(! metadata_io)
		return ST_NULL_PTR;

	metadata_io->help = TRUE;
	return ST_OK;
}

status_t arg_set_name(arg_s * metadata_io, const char * name) {

	if(! metadata_io)
		return ST_NULL_PTR;

	metadata_io->name = (char *)malloc(strlen(name) * sizeof(char));

	if(metadata_io->name == NULL)
		return ST_NO_MEM;

	strcpy(metadata_io->name, name);

	return ST_OK;
}

status_t arg_set_protocol(arg_s * metadata_io, const char * protocol) {

	size_t i;

	if(! metadata_io)
		return ST_NULL_PTR;

	for(i = 0; i < PROTOCOL_NUM; i++) {

		if(! strcmp(protocols[i], protocol)) {
			metadata_io->protocol = i + 1;
			return ST_OK;
		}
	
	}

	return ST_INVALID_ARGUMENT;
}

status_t arg_set_infile(arg_s * metadata_io, const char * infile) {

	if(! metadata_io)
		return ST_NULL_PTR;

	if(metadata_io->protocol == NMEA) {

		if(! strcmp(infile, DEFAULT_FILE_STR)) {
			
			metadata_io->infile = stdin;
			metadata_io->infile_default = TRUE;
			return ST_OK;
		}

		if((metadata_io->infile = fopen(infile, MODE_READ_TEXT)) == NULL)
			return ST_ERR_OPEN_IN_FILE;

	} else if(metadata_io->protocol == UBX) {

		if(! strcmp(infile, DEFAULT_FILE_STR)) {
			
			freopen(NULL, MODE_READ_BINARY, stdin); // PREGUNTAAAR!!!!!!!!!!!

			metadata_io->infile = stdin;
			metadata_io->infile_default = TRUE;
			return ST_OK;
		}

		if((metadata_io->infile = fopen(infile, MODE_READ_BINARY)) == NULL)
			return ST_ERR_OPEN_IN_FILE;

	} else if(metadata_io->protocol == AUTO) {

	} else {

		return ST_NO_PROTOCOL;

	}

	return ST_OK;
}

status_t arg_set_outfile(arg_s * metadata_io, const char * outfile) {

	if(! metadata_io)
		return ST_NULL_PTR;

	if(! strcmp(outfile, DEFAULT_FILE_STR)) {

		metadata_io->outfile = stdout;
		metadata_io->outfile_default = TRUE;
		return ST_OK;
	}

	if((metadata_io->outfile = fopen(outfile, MODE_WRITE_TEXT)) == NULL)
		return ST_ERR_OPEN_OUT_FILE;

	return ST_OK;
}

status_t arg_set_logfile(arg_s * metadata_io, const char * logfile) {

	if(! metadata_io)
		return ST_NULL_PTR;

	if(! strcmp(logfile, DEFAULT_FILE_STR)) {

		metadata_io->logfile = stderr;
		metadata_io->logfile_default = TRUE;
		return ST_OK;
	}

	if((metadata_io->logfile = fopen(logfile, MODE_WRITE_TEXT)) == NULL)
		return ST_ERR_OPEN_LOG_FILE;

	return ST_OK;
}

status_t arg_set_maxlen(arg_s * metadata_io, const char * maxlen) {

	char * end_ptr;

	if(! metadata_io)
		return ST_NULL_PTR;

	if((metadata_io->maxlen = strtol(maxlen, &end_ptr, 10)) > MAX_MAXLEN)
		return ST_INVALID_ARGUMENT;

	if(! end_ptr)
		return ST_INVALID_ARGUMENT;

	if(*end_ptr != '\0')
		return ST_INVALID_ARGUMENT;

	return ST_OK;
}

/* REQUERIMIENTOS Y CONSECUENCIAS:

* LA ESTRUCTURA SE DEBE CREAR UTILIZANDO LA FUNCION 'arg_create'

* SE DEBE LIBERAR MEMORIA ANTES DE CERRAR EL PROGRAMA

* ES NECESARIO INTGRESAR LOS ARGUMENTOS:
									-PROTOCOL
									-INFILE
									-OUTFILE
									-LOGFILE

* SE DEBE INTGRESAR ARGUMENTO DE PROTOCOLO ANTES QUE EL ARCHIVO DE ENTRADA

* SE ESTABLECEN POR OMISION LOS SIGUIENTES ARGUMENTOS NO INGRESADOS:
									
									-NAME
									-MAXLEN


*/