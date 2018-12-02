typedef enum {FALSE, TRUE} bool_t;
typedef enum {P_NMEA = 1, P_UBX = 2, P_AUTO = 3} protocol_t;
typedef enum {ST_OK, ST_NULL_PTR, ST_INVALID_ARGUMENT, ST_NO_MEM, ST_ERR_OPEN_IN_FILE, ST_ERR_OPEN_OUT_FILE, ST_ERR_OPEN_LOG_FILE, ST_NO_PROTOCOL, ST_ERROR_EOF, ST_CORRUPT_FILE, ST_FULL_LIST} status_t;
typedef enum {FIX_INVALID, FIX_GPS, FIX_DGPS, FIX_PPS, FIX_RTK, FIX_FLOAT_RTK, FIX_ESTIMATED, FIX_MANUAL, FIX_SIMULATION} fix_quality_gga_t;
typedef enum {FIX_NO, FIX_DEAD_RECKONING, FIX_2D, FIX_3D, FIX_COMBINED, FIX_TIME_ONLY} fix_pvt_t;
typedef enum {A, V} rmc_status_t;
typedef enum {PROCESS_ZERO, PROCESS_ONE, PROCESS_TWO, PROCESS_ALL} process_t;


struct arg {
	struct tm * time;
	bool_t help;
	char * name;
	protocol_t protocol;
	FILE * infile;
	char * infile_name;
	FILE * outfile;
	FILE * loprotocolsgfile;
	bool_t infile_default;
	bool_t outfile_default;
	bool_t logfile_default;
	size_t maxlen;
};
typedef struct arg arg_s;

struct tkpt {
	struct tm * tkpt_time;
	int tkpt_msec;
	double latitude;
	double longitude;
	double elevation;
};
typedef struct tkpt tkpt_s;

struct node {
	void * data;
	struct node * next;
};
typedef struct node Node;

struct list {
	struct node * first_node;
	size_t len;
};
typedef struct list List;


struct gga {
	struct tm gga_time;
	int gga_time_milisec; // miliseconds agregados!
	double latitude;
	double longitude;
	fix_quality_gga_t quality;
	unsigned int n_sat;
	double hdop;
	double elevation;
	double undulation_of_geoid;
};
typedef struct gga gga_s;


struct rmc {
	struct tm rmc_time;	
	rmc_status_t status;
	double latitude; // el indicador (N o S) se expresa según la latitud sea pos o neg
	double longitude;
	double speed;
	double tracking_angle;
	double magnetic_deviation; // ver la W

};
typedef struct rmc rmc_s;


struct zda {
	struct tm zda_time;
	int zda_time_milisec;
	int time_zone;
	int diff_minutes;
	bool_t time_flag;
};
typedef struct zda zda_s;


struct nav_pvt {
	int pvt_elapsed_time;
	struct tm pvt_time;
	fix_pvt_t fix;
	unsigned int n_sat;
	double latitude;
	double longitude;
	double elipsode_height;
	double oversea_height;
	double pdop;
};
typedef struct nav_pvt nav_pvt_s;


struct tim_tos {
	unsigned char version; // ????
	// GNSS utilizado ???
	struct tm tim_time;
	unsigned int week_number;
	unsigned long segs_elapsed;
};
typedef struct tim_tos tim_tos_s;


struct nav_posllh {
	int nav_elapsed_time;
	double latitude;
	double longitude;
	double elipsode_height;
	double oversea_height;
	double hor_precision;
	double ver_precision;

};
typedef struct nav_posllh nav_posllh_s;
