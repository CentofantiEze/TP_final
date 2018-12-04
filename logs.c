#define MAX_STR 30
#define MSG_TIME_UPDATED "The current time has been updated" 
#define MSG_DATE_UPDATED "The current date has been updated" 
#define MSG_TKPT_GEN "Trackpoint generated successfully"
#define MSG_TKPT_PRINT "Trackpoint printed successfully" 
#define MSG_NMEA_GP "Warning: No GPS found" 
#define MSG_NMEA_FIX "Warning: invalid fix"
#define MSG_UBX_ID "Warning: invalid ID"
#define MSG_UBX_LONG "Error: invalid payload length" 
#define MSG_UBX_TIME "Error: invalid time" 
#define MSG_UBX_DATE "Error: invalid date" 
#define MSG_INCORRECT_STATEMENT "Error: Statement has unexpected format" 
#define MSG_INTERNAL_ERROR "Error: internal error"
#define MSG_CHECKSUM "Error: wrong checksum" 
#define MSG_IGNORED_STATEMENT "Ignored statement" 
#define MSG_SUCCESSFUL_EXIT "The program has ended successfully"
#define MSG_READ_FILE "Fatal error: could not read file"
#define MSG_OPEN_FILE "Fatal error: could not open file"

typedef enum {TIME_UPDATED, DATE_UPDATED, TKPT_GEN, TKPT_PRINT, NMEA_GP, NMEA_FIX, UBX_ID, UBX_LONG, UBX_TIME, UBX_DATE, INCORRECT_STATEMENT, INTERNAL_ERROR, CHECKSUM, IGNORED_STATEMENT, SUCCESSFUL_EXIT, READ_FILE, OPEN_FILE} log_t;

const char log_dictionary[][MAX_STR] = {MSG_TIME_UPDATED, MSG_DATE_UPDATED, MSG_TKPT_GEN, MSG_TKPT_PRINT, MSG_NMEA_GP, MSG_NMEA_FIX, MSG_UBX_ID, MSG_UBX_LONG, MSG_UBX_TIME, MSG_UBX_DATE, MSG_INCORRECT_STATEMENT, MSG_INTERNAL_ERROR, MSG_CHECKSUM, MSG_IGNORED_STATEMENT, MSG_SUCCESSFUL_EXIT, MSG_READ_FILE, MSG_OPEN_FILE};


void log_print(FILE *, log_t);

void log_print(FILE * f, log_t current_log) {

	fprintf(f, "%s\n", log_dictionary[current_log]);

	if(current_log >= UBX_LONG && CHECKSUM >= current_log)
}
