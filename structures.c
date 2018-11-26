typedef enum {INVALID_FIX, GPS_FIX, DGPS_FIX, PPS_FIX, RTK_FIX, FLOAT_RTK_FIX, ESTIMATED_FIX, MANUAL_FIX, SIMULATION_FIX} fix_quality_gga_t;
typedef enum {NO_FIX, DEAD_RECKONING_FIX, 2D_FIX, 3D_FIX, COMBINED_FIX, TIME_ONLY_FIX} fix_pvt_t;
typedef enum {A, V} rmc_status_t;

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


struct rmc {
	struct tm rmc_time;	
	rmc_status_t status;
	double latitude; // el indicador (N o S) se expresa según la latitud sea pos o neg
	double longitude;
	double speed;
	double tracking_angle;
	double magnetic_deviation; // ver la W

};

struct zda {
	struct tm zda_time;
	int zda_time_milisec;
	int time_zone;
	int diff_minutes;
	bool_t time_flag;
};

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

struct tim_tos {
	unsigned char version; // ????
	// GNSS utilizado ???
	struct tm tim_time;
	unsigned int week_number;
	unsigned long segs_elapsed;
};

struct nav_posllh {
	int nav_elapsed_time;
	double latitude;
	double longitude;
	double elipsode_height;
	double oversea_height;
	double hor_precision;
	double ver_precision;

};

struct tkpt {
	struct tm tkpt_time;
	int tkpt_msec;
	double latitude;
	double longitude;
	double elevation;
}

typedef struct gga gga_s;
typedef struct rmc rmc_s;
typedef struct zda zda_s;
typedef struct nav_pvt nav_pvt_s;
typedef struct tim_tos tim_tos_s;
typedef struct nav_posllh nav_posllh_s;
typedef struct tkpt tkpt_s;