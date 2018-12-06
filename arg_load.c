status_t arg_load(int argc, const char ** argv, arg_s * metadata_io) {

	size_t i, j;
	status_t st;
	time_t actual_time = time(NULL);
	int arg_flags[ARG_TYPES] = {0}; // Se utiliza para saber que argumentos ya se procesaron, para evitar repeticiones
	const char valid_arguments[][ARG_MAX_LEN] = {
	"-h", "--help",
	"-n", "--name",
	"-p", "--protocol",
	"-i", "--infile",
	"-o", "--outfile",
	"-l", "--logfile",
	"-m", "--maxlen"
	};


	if(! metadata_io)
		return ST_NULL_PTR;

	for(i = 1; i < argc; i++) {

		if(! strcmp(argv[i], STR_PROTOCOL_1) || ! strcmp(argv[i], STR_PROTOCOL_2)) {

			if(++i == argc)
				return ST_INVALID_ARGUMENT;

			st = arg_set_protocol(metadata_io, argv[i]);

			if(st == ST_NULL_PTR || st == ST_INVALID_ARGUMENT)
				return st;
		}
	}

	for(i = 1; i < argc; i++) { // Para cada argumento en argv se compara con los argumentos validos

		for(j = 0; j < ARG_NUM; j++) {

			if(! strcmp(argv[i], valid_arguments[j])) {

				switch(j/2) {

					case ARG_TYPE_HELP :

						arg_set_help(metadata_io);

						arg_flags[j/2]++; // SE CARGA LA LECTURA EN EL VECTOR DE FLAGS
						break;

					case ARG_TYPE_NAME :

						if(++i == argc) // si -n esta en el ultimo lugar de argv no puede leer el nombre porque no hay nada
							return ST_INVALID_ARGUMENT;

						st = arg_set_name(metadata_io, argv[i]);
						
						if(st == ST_NO_MEM || st == ST_NULL_PTR)
							return st;

						arg_flags[j/2]++;
						break;

					case ARG_TYPE_PROTOCOL :

						arg_flags[j/2]++;
						break;


					case ARG_TYPE_INFILE :

						if(++i == argc)
							return ST_INVALID_ARGUMENT;

						st = arg_set_infile(metadata_io, argv[i]);

						if((metadata_io->infile_name = (char *)malloc((strlen(argv[i]) + 1) * sizeof(char))) == NULL)
							return ST_NO_MEM;

						if(! strcpy(metadata_io->infile_name, argv[i]))
							return ST_INVALID_ARGUMENT;
						
						if(st == ST_NULL_PTR || st == ST_ERR_OPEN_IN_FILE || st == ST_NO_PROTOCOL)
							return st;

						arg_flags[j/2]++;
						break;

					case ARG_TYPE_OUTFILE :

						if(++i == argc)
							return ST_INVALID_ARGUMENT;

						st = arg_set_outfile(metadata_io, argv[i]);
						
						if(st == ST_NULL_PTR || st == ST_ERR_OPEN_OUT_FILE)
							return st;

						arg_flags[j/2]++;
						break;

					case ARG_TYPE_LOGFILE :

						if(++i == argc)
							return ST_INVALID_ARGUMENT;

						st = arg_set_logfile(metadata_io, argv[i]);
						
						if(st == ST_NULL_PTR || st == ST_ERR_OPEN_LOG_FILE)
							return st;

						arg_flags[j/2]++;
						break;

					case ARG_TYPE_MAXLEN :

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

				j = ARG_NUM; //Si se encuentra el tipo de argumento no se sigue buscando en valid_arguments

			}
		}
	}

	metadata_io->time = gmtime(&actual_time); // CARGA LA ESTRUCTURA APUNTADA POR METADATA_IO->TIME

	if(arg_flags[ARG_TYPE_HELP] == 0) // SE PONE EL FLAG DEL HELP EN 1 PARA LUEGO VERIFICAR LOS FLAGS
		arg_flags[ARG_TYPE_HELP]++;

	if(arg_flags[ARG_TYPE_NAME] == 0) { // SI NO SE INGRESO EL NOMBRE SE CARGA EL DEFAULT

		st = arg_set_name(metadata_io, DEFAULT_NAME);
						
		if(st == ST_NO_MEM || st == ST_NULL_PTR)
			return st;

		arg_flags[ARG_TYPE_NAME]++;

	}

	if(arg_flags[ARG_TYPE_MAXLEN] == 0) { // SI NO SE INGRESO MAXLEN SE CARGA EL DEFAULT

		st = arg_set_maxlen(metadata_io, DEFAULT_MAXLEN);

		if(st == ST_NULL_PTR || st == ST_INVALID_ARGUMENT)
			return st;

		arg_flags[ARG_TYPE_MAXLEN]++;
	}

	for(i = 0; i < ARG_TYPES; i++) { // TODOS LOS FLAGS DEBEN ESTAR EN 1

		if(arg_flags[i] != 1) 
			return ST_INVALID_ARGUMENT;
		
	}

	return ST_OK;
}


arg_s * arg_create(status_t * st) { // CREA UNA ESTRUCTURA Y DEVUELVE UN PUNTERO A LA MISMA (INICIALIZADA EN 0)

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

status_t arg_set_help(arg_s * metadata_io) { // ASIGNA TRUE AL HELP DE LA ESTRUCTURA

	if(! metadata_io)
		return ST_NULL_PTR;

	metadata_io->help = TRUE;
	return ST_OK;
}

status_t arg_set_name(arg_s * metadata_io, const char * name) { // PIDE MEMORIA PARA EL NOMBRE Y LO CARGA EN LA ESTRUCTURA

	if(! metadata_io)
		return ST_NULL_PTR;

	metadata_io->name = (char *)malloc((strlen(name) + 1) * sizeof(char));

	if(metadata_io->name == NULL)
		return ST_NO_MEM;

	strcpy(metadata_io->name, name);

	return ST_OK;
}

status_t arg_set_protocol(arg_s * metadata_io, const char * protocol) { // CARGA EL PROTOCOLO EN LA ESTRUCTURA 

	size_t i;
	const char protocols[][PROTOCOL_MAX_LEN] = {"nmea", "ubx", "auto"};

	if(! metadata_io)
		return ST_NULL_PTR;

	for(i = 0; i < PROTOCOL_NUM; i++) {

		if(! strcmp(protocols[i], protocol)) {
			metadata_io->protocol = i + 1; // LOS PROTOCOLOS VALIDOS SON 1, 2 Y 3. DEBE SUMARSE 1
			return ST_OK;
		}
	
	}

	return ST_INVALID_ARGUMENT;
}

status_t arg_set_infile(arg_s * metadata_io, const char * infile) { // ABRE EL ARCHIVO DE LECTURA Y GUARDA EL PUNTERO EN LA ESTRUCTURA

	if(! metadata_io)
		return ST_NULL_PTR;

	if(metadata_io->protocol == P_NMEA) { // SI EL PROTOCOLO ES NMEA SE DEBE ABRIR PARA LECTURA DE TEXTO

		if(! strcmp(infile, DEFAULT_FILE_STR)) { // SI EL NOMBRE DEL ARCHIVO ES EL RESERVADO PARA DEFAULT '-' EL ARCHIVO DE LECTURA ES STDIN
			
			metadata_io->infile = stdin; // SI SE ABRE EL ARCHIVO DEFAULT INFILE_DEFAULT TOMA EL VALOR TRUE
			metadata_io->infile_default = TRUE;
			return ST_OK;
		}

		if((metadata_io->infile = fopen(infile, MODE_READ_TEXT)) == NULL)
			return ST_ERR_OPEN_IN_FILE;

	} else if(metadata_io->protocol == P_UBX) { // SI EL PROTOCOLO ES UBX SE DEBE ABRIR PARA LECTURA BINARIA

		if(! strcmp(infile, DEFAULT_FILE_STR)) { // SI EL NOMBRE ES '-' SE REABRE STDIN PARA LECTURA BINARIA (PREGUNTAR)
			
			freopen(NULL, MODE_READ_BINARY, stdin); // PREGUNTAAAR!!!!!!!!!!!

			metadata_io->infile = stdin;
			metadata_io->infile_default = TRUE;
			return ST_OK;
		}

		if((metadata_io->infile = fopen(infile, MODE_READ_BINARY)) == NULL)
			return ST_ERR_OPEN_IN_FILE;

	} else if(metadata_io->protocol == P_AUTO) { // SI EL PROTOCOLO ES AUTO NO SE REALIZA NINGUNA ACCION, EL ARCHIVO DEBE ABRIRSE UNA VEZ DETECTADO EL PROTOCOLO.

	} else { // SI EL PROTOCOLO NO FUE ASIGNADO SE DEVUELVE ERROR

		return ST_NO_PROTOCOL;

	}

	return ST_OK;
}

status_t arg_set_outfile(arg_s * metadata_io, const char * outfile) { 

	if(! metadata_io)
		return ST_NULL_PTR;

	if(! strcmp(outfile, DEFAULT_FILE_STR)) { // SI EL NOMBRE DEL ARCHIVO ES '-' (RESERVADO) EL ARCHIVO DE SALIDA ES STDOUT

		metadata_io->outfile = stdout;
		metadata_io->outfile_default = TRUE;
		return ST_OK;
	}

	if((metadata_io->outfile = fopen(outfile, MODE_WRITE_TEXT)) == NULL)
		return ST_ERR_OPEN_OUT_FILE;

	return ST_OK;
}

status_t arg_set_logfile(arg_s * metadata_io, const char * logfile) { // IDEM OUTFILE

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

status_t arg_set_maxlen(arg_s * metadata_io, const char * maxlen) { // SE CARGA EL MAXIMO DE LINEAS EN LA ESTRUCTURA

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

void close_files(arg_s * metadata) {

	if(! metadata)
		return;

	if(! metadata->infile_default) {
		if(metadata->infile)
			fclose(metadata->infile);
	} else if(metadata->protocol == P_UBX)
		freopen(NULL, "rt", stdin);

	if(! metadata->outfile_default)
		if(metadata->outfile)
			fclose(metadata->outfile);

	if(! metadata->logfile_default)
		if(metadata->logfile)
			fclose(metadata->logfile);

}


void free_metadata(arg_s * metadata) {

	if(! metadata)
		return;

	free(metadata->name);
	free(metadata->infile_name);
	free(metadata);
	metadata = NULL;

}



/* REQUERIMIENTOS Y CONSECUENCIAS:

* LA ESTRUCTURA SE DEBE CREAR UTILIZANDO LA FUNCION 'arg_create'

* SE DEBE LIBERAR MEMORIA ANTES DE CERRAR EL PROGRAMA (!!!)

* SE DEBE VERIFICAR SI LOS ARCHIVOS IN/OUT SON STANDARD O EXTERNOS (.txt)

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