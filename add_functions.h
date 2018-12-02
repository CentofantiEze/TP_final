#define MAX_PROCESS_T 3

#define DATA_S_TYPES 4
#define DATA_GGA_S 0
#define DATA_ZDA_S 1
#define DATA_RMC_S 2
#define DATA_NAV_PVT_S 0
#define DATA_TIM_TOS_S 1
#define DATA_NAV_POSLLH_S 2 
#define DATA_TKPT_S 3


status_t protocol_detect(arg_s * metadata);
status_t data_structs_create(arg_s * metadata, void * data_structs[]);
void free_data_structs(void * data_structs[]);
status_t gpx_process(List *, process_t);
