

#define GGA_FIELDS 12
#define ZDA_FIELDS 6
#define RMC_FIELDS 11

#define GGA_TIME_FIELD 0
#define GGA_LAT_FIELD 1
#define GGA_LAT_N_S_FIELD 2
#define GGA_LONG_FIELD 3
#define GGA_LONG_W_E_FIELD 4
#define GGA_FIX_FIELD 5
#define GGA_SAT_FIELD 6
#define GGA_HDOP_FIELD 7
#define GGA_ELEV_FIELD 8
#define GGA_UNDULATION_FIELD 10

#define ZDA_TIME_FIELD 0
#define ZDA_Y_FIELD 3
#define ZDA_M_FIELD 2
#define ZDA_D_FIELD 1
#define ZDA_TIMEZONE_FIELD 4
#define ZDA_DIFF_FIELD 5

#define RMC_TIME_FIELD 0
#define RMC_STATUS_FIELD 1
#define RMC_LAT_FIELD 2
#define RMC_LAT_N_S_FIELD 3
#define RMC_LONG_FIELD 4
#define RMC_LONG_W_E_FIELD 5
#define RMC_SPEED_FIELD 6
#define RMC_ANGLE_FIELD 7
#define RMC_DATE_FIELD 8
#define RMC_MAGNETIC_FIELD 9

#define CHAR_INIT_NMEA '$'
#define CHAR_END_NMEA '*'
#define TALKER_ID_CHAR 2
#define NMEA_PROTOCOL 3
#define CHAR_DELIM ','
#define MAX_STR_NMEA 70
#define MAX_SUBSTR_NMEA 10
#define STR_HEX "0123456789ABCDEF"
#define HEXSTRING_NULL_FIND_INT -1
#define HEX_DIGITS 16
#define GP_INDICATOR "GP"
#define GGA_INDICATOR "GGA"
#define ZDA_INDICATOR "ZDA"
#define RMC_INDICATOR "RMC"


/* macros de tiempo de fix */
#define HOURS_DIGITS 2
#define MINUTES_DIGITS 2
#define SECONDS_DIGITS 2
#define MILISECONDS_DIGITS 3
#define MILISECONDS_DIGITS_ZDA 2
#define DAY_DIGITS 2
#define MONTH_DIGITS 2
#define YEAR_DIGITS 2
#define OFFSET_YEAR 2000

/* macros de latitud y longitud */
#define NMEA_LATITUDE_DEGREES 2
#define NMEA_LATITUDE_MINUTES 5
#define NMEA_LONGITUDE_DEGREES 3
#define NMEA_LONGITUDE_MINUTES 5
#define MAX_QUALITY 8
#define CONVERSION_FACTOR_MINUTES 60
#define ZERO_ASCII_VALUE 48
#define SOUTH_CHAR 'S'
#define WEST_CHAR 'W'

/* macros de calidad de fix */

#define INVALID_FIX_CHAR '0'
#define GPX_FIX_CHAR '1'
#define DGPS_FIX_CHAR '2'
#define PPS_FIX_CHAR '3'
#define RTK_FIX_CHAR '4'
#define FLOAT_RTK_FIX_CHAR '5'
#define ESTIMATED_FIX_CHAR '6'
#define MANUAL_FIX_CHAR '7'
#define SIMULATION_FIX_CHAR '8'

/* macros de cantidad de satelites */
#define MAX_SAT 12
#define MIN_SAT 0

/* el resto */

#define HDOP_DIGITS 2
#define ELEVATION_DIGITS 2
#define UNDULATION_OF_GEOID_DIGITS 2
#define RMC_STATUS_A 0
#define RMC_STATUS_V 1
#define RMC_CHAR_A 'A'
#define RMC_CHAR_V 'V'


status_t read_nmea_gga(FILE *, data_structs_s *, int);
status_t read_nmea_zda(FILE *, data_structs_s *, int);
status_t read_nmea_rmc(FILE *, data_structs_s *, int);
status_t gga2tkpt(tkpt_s *, gga_s *);
status_t zda2tkpt(tkpt_s *, zda_s *);
status_t rmc2tkpt(tkpt_s *, rmc_s *);

status_t get_tkpt_nmea(arg_s * f, data_structs_s *);
status_t hexstring_2_integer(int, int, int *);
status_t get_nmea_data(unsigned int, char s[][MAX_SUBSTR_NMEA], FILE *, int *); 
status_t gga_time_of_fix(char *, data_structs_s *);
status_t latitude(char *, char *, data_structs_s *, void *);
status_t longitude(char *, char *, data_structs_s *, void *);
status_t gga_quality_of_fix(char *, data_structs_s *);
status_t gga_num_of_satellites(char *, data_structs_s *);
status_t gga_hdop(char *, data_structs_s *);
status_t gga_elevation(char *, data_structs_s *);
status_t gga_undulation_of_geoid(char *, data_structs_s *);
status_t zda_time_of_fix(char *, char *, char *, char *, data_structs_s *);
status_t zda_time_zone(char *, data_structs_s *);
status_t zda_time_difference(char *, data_structs_s *);
status_t rmc_time_of_fix(char *, char *, data_structs_s *);
status_t rmc_speed(char *, data_structs_s *);
status_t rmc_angle(char *, data_structs_s *);
status_t rmc_magnetic_deviation(char *, data_structs_s *);
status_t rmc_latitude(char *, char *, data_structs_s *, void *);
status_t rmc_longitude(char *, char *, data_structs_s *, void *);
