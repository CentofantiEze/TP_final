status_t list_create(List ** l);
status_t node_create(Node ** node, void * data, tkpt_s *(*clone)(tkpt_s *));
tkpt_s * clone_tkpt(tkpt_s * data);
status_t list_append_tkpt(List * l, void * data, size_t maxlen);
status_t list_delete(List * list);
status_t popleft(List * list, tkpt_s ** tkpt);