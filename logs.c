void log_print(FILE * f, log_t current_log) {

	if(! f)
		return;

	fprintf(f, "\n%s", log_dictionary[current_log]);

	if(current_log == UBX_LONG || current_log == UBX_TIME || current_log == UBX_DATE || current_log == INCORRECT_STATEMENT  || current_log == CHECKSUM)
		fprintf(f, "\n%s", log_dictionary[IGNORED_STATEMENT]);	
}
