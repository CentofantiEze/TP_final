//arg_s * arg_create(status_t *);
//status_t arg_load(int  , const char **, arg_s *);
//status_t arg_set_help(arg_s *);
//status_t arg_set_name(arg_s *, const char *);
//status_t arg_set_protocol(arg_s *, const char *);
//status_t arg_set_infile(arg_s *, const char *);
//status_t arg_set_outfile(arg_s *, const char *);
//status_t arg_set_logfile(arg_s *, const char *);
//status_t arg_set_maxlen(arg_s *, const char *);
//void close_files(arg_s * metadata);
//void free_metadata(arg_s * metadata);

//status_t list_create(List ** l);
status_t node_create(Node ** node, void * data, tkpt_s *(*clone)(tkpt_s *));
tkpt_s * clone_tkpt(tkpt_s * data);
status_t list_append_tkpt(List * l, void * data, size_t maxlen);
List * list_delete(List * list);

//status_t protocol_detect(arg_s * metadata);
//status_t data_structs_create(arg_s * metadata, data_structs_s ** data_structs);
//void free_data_structs(data_structs_s * data_structs);
status_t gpx_process(List *, process_t);