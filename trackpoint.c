status_t list_create(List * l);
status_t node_create(Node ** node, void * data, tkpt_s *(*clone)(tkpt_s *));
status_t list_append_tkpt(List * l, void * data, size_t maxlen);
void * list_pop_left(List * l, status_t *);