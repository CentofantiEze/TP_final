#define MAX_STR 30
#define MSG_ST_OK "Working fine"
#define MSG_ST_NULL_PTR "Null pointer"
#define MSG_ST_INVALID_ARGUMENT "Invalid argument"
#define MSG_ST_NO_MEM "No memory available"
#define MSG_ST_ERR_OPEN_IN_FILE "Could not open file input"
#define MSG_ST_ERR_OPEN_OUT_FILE "Could not open file output"
#define MSG_ST_ERR_OPEN_LOG_FILE "Could not open log file"
#define MSG_ST_NO_PROTOCOL "No protocol"
#define MSG_ST_ERROR_EOF "End of File"
#define MSG_ST_CORRUPT_FILE "File has been corrupted"
#define MSG_ST_FULL_LIST "Full list"
#define MSG_ST_IGNORE_STATEMENT "One statement has been ignored"
#define MSG_ST_NUMERICAL_ERROR "Invalid number"


typedef enum {ST_OK, ST_NULL_PTR, ST_INVALID_ARGUMENT, ST_NO_MEM, ST_ERR_OPEN_IN_FILE, ST_ERR_OPEN_OUT_FILE, ST_ERR_OPEN_LOG_FILE, ST_NO_PROTOCOL, ST_ERROR_EOF, ST_CORRUPT_FILE, ST_FULL_LIST, ST_IGNORE_STATEMENT, ST_NUMERICAL_ERROR} status_t;

const char status_dictionary[][MAX_STR] = {MSG_ST_OK, MSG_ST_NULL_PTR, MSG_ST_INVALID_ARGUMENT, MSG_ST_NO_MEM, MSG_ST_ERR_OPEN_IN_FILE, MSG_ST_ERR_OPEN_OUT_FILE, MSG_ST_ERR_OPEN_LOG_FILE, MSG_ST_NO_PROTOCOL, MSG_ST_ERROR_EOF, MSG_ST_CORRUPT_FILE, MSG_ST_FULL_LIST, MSG_ST_IGNORE_STATEMENT, MSG_ST_NUMERICAL_ERROR};

void log_print(FILE *, status_t);

void log_print(FILE * f, status_t st) {

	fprintf(f, "%s\n", status_dictionary[st]);
}


/* SE DEBE INFORMAR
-Actualizar hora
-Trackpoint generado
-Impresión GPX exitosa
-No se recibió GP NMEA [warn]
-Fix inválido NMEA
-Sentencia mal formada
-No se recibió ID válido UBX [warn]
-No se pudo abrir archivo
-Checksum
-Largos incorrectos
-Sentencias descartadas */
