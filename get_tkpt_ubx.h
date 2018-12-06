#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define IGNORE_BYTES_TIM_TOS_1 6
#define IGNORE_BYTES_TIM_TOS_2 9
#define IGNORE_BYTES_TIM_TOS_3 24

#define IGNORE_BYTES_PVT_1 8
#define IGNORE_BYTES_PVT_2 1
#define IGNORE_BYTES_PVT_3 36
#define IGNORE_BYTES_PVT_4 6
#define IGNORE_BYTES_PVT_5 8

#define CONV_LTL_ENDN_2 256
#define CONV_LTL_ENDN_3 65536
#define CONV_LTL_ENDN_4 16777216

#define MASK_VALID_DATE 0x1
#define SHIFT_VALID_DATE 0
#define MASK_VALID_TIME 0x2
#define SHIFT_VALID_TIME 1
#define MASK_VALID_FIX 0x1
#define SHIFT_VALID_FIX 0

#define PAYLOAD_TIM_TOS 56
#define PAYLOAD_NAV_PVT 92
#define PAYLOAD_NAV_POSLLH 28

#define CLASS_NAV 0x01
#define CLASS_TIM_TOS 0x0D

#define ID_TIM_TOS 0x12
#define ID_NAV_PVT 0x07
#define ID_NAV_POSLLH 0x02

#define SYNC_1 0xB5
#define SYNC_2 0x62

#define MASK_DOUBLE_B0 0x000000FF
#define MASK_DOUBLE_B1 0x0000FF00
#define MASK_DOUBLE_B2 0x00FF0000
#define MASK_DOUBLE_B3 0xFF000000

#define SHIFT_DOUBLE_B0 0
#define SHIFT_DOUBLE_B1 8
#define SHIFT_DOUBLE_B2 16
#define SHIFT_DOUBLE_B3 24

#define LONGITUDE_SCALE 10000000
#define LATITUDE_SCALE 10000000

status_t get_tkpt_ubx(arg_s *, data_structs_s *);
status_t search_sync(FILE * f);
status_t load_tim_tos (arg_s *, tim_tos_s *, size_t, uchar, uchar);
status_t load_nav_pvt(arg_s *, nav_pvt_s *, size_t , uchar, uchar);
status_t load_nav_posllh(arg_s *, nav_posllh_s *, size_t, uchar, uchar);
status_t tim_tos2tkpt(data_structs_s *);
status_t nav_pvt2tkpt(data_structs_s *);
status_t nav_posllh2tkpt(data_structs_s *);
status_t get_length(FILE *, size_t *, uchar *, uchar *);
status_t read_unsigned_4(FILE * f, uchar * cksm1, uchar * cksm2, unsigned int * val);
status_t read_signed_4(FILE * f, uchar * cksm1, uchar * cksm2, long * val);
