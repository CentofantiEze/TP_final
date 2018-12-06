#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define HELP_MSJ_START "rerun with -h or --help to view help file."

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

#define MAX_MAXLEN 1000
#define DEFAULT_MAXLEN "1000"

#define STR_PROTOCOL_1 "-p"
#define STR_PROTOCOL_2 "--protocol"

arg_s * arg_create(status_t *);
status_t arg_load(int  , const char **, arg_s *);
status_t arg_set_help(arg_s *);
status_t arg_set_name(arg_s *, const char *);
status_t arg_set_protocol(arg_s *, const char *);
status_t arg_set_infile(arg_s *, const char *);
status_t arg_set_outfile(arg_s *, const char *);
status_t arg_set_logfile(arg_s *, const char *);
status_t arg_set_maxlen(arg_s *, const char *);
void close_files(arg_s * metadata);
void free_metadata(arg_s * metadata);