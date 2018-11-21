#define FIRST_CHAR_NMEA '$' // ya estaba en el TP1 

#define SYNC_1 0xB5
#define SYNC_2 0x62
#define CLASS_NAV 0x01
#define CLASS_TIM_TOS 0x0D
#define ID_TIM_TOS 0x12
#define ID_NAV_POSLLH 0x02
#define ID_NAV_PVT 0x07
#define LENGHT_TIM_TOS 56
#define LENGHT_NAV_POSLLH 28
#define LENGTH_NAV_PVT 92


#define DATE_MASK 0x01
#define DATE_SHIFT 0
#define TIME_MASK 0x02
#define TIME_SHIFT 1
#define FULLY_RESOLVED_MASK 0x04
#define FULLY_RESOLVED_SHIFT 2
#define MAG_MASK 0x08
#define MAG_SHIFT 3
#define GNSS_FIX_OK_MASK 0x01
#define GNSS_FIX_OK_SHIFT 0


status_t read_ubx(FILE *, bool_t *);
status_t read_tim_tos(FILE *, bool_t *, tim_tos_s *); //desde ID
status_t read_nav_pvt(FILE *, bool_t *, nav_pvt_s *, size_t); //desde l o payload?
status_t read_nav_posllh(FILE *, bool_t *, nav_posllh_s *, size_t); //desde l o payload?


status_t read_ubx(FILE * f, bool_t * eof) { // el puntero está en class

	unsigned char aux;
	size_t l; // ?
	status_t st;
	tim_tos_s * tim;
	nav_pvt_s * pvt;
	nav_posllh * posllh;

	if(!f || !eof)
		return ST_NULL_PTR;

	if(feof(f)) {
		*eof = TRUE;
		return ST_ERROR_EOF; // AGREGAR A status_t !!!
	}

	/* lee CLASS */
	if((fread(&aux, sizeof(unsigned char), 1, f)) != 1) { 
		if(feof(f)) {
			*eof = TRUE;
			return ST_ERROR_EOF;
		}
		return ST_CORRUPT_FILE; // AGREGAR A status_t !!!
	}

	if(aux == CLASS_TIM_TOS)
 		if((st = read_tim_tos(f, eof, tim)) != ST_OK) 
 			return st;
 		else
 			return ST_OK;
 	
 	if(aux != CLASS_NAV)
 		return //???? sentencia invalida

 	/* lee ID */
 	if((fread(&aux, sizeof(unsigned char), 1, f)) != 1) {
		if(feof(f)) {
			*eof = TRUE;
			return ST_ERROR_EOF;
		}
		return ST_CORRUPT_FILE; // AGREGAR A status_t !!!
	}

	if(aux == ID_NAV_POSLLH)
 		if((st = read_nav_posllh(f, eof, posllh, l)) != ST_OK) 
 			return st;
 		else
 			return ST_OK;

 	if(aux == ID_NAV_PVT)
 		if((st = read_nav_pvt(f, eof, pvt)) != ST_OK) 
 			return st;
 		else
 			return ST_OK;

 	/* Sigo leyendo... pero warning porque el ID seria invalido */

 	/* leo el largo */
 	if((fread(&l, sizeof(size_t), 1, f)) != 1) {
		if(feof(f)) {
			*eof = TRUE;
			return ST_ERROR_EOF;
		}
		return ST_CORRUPT_FILE; // AGREGAR A status_t !!!
	}

	if(l == LENGHT_NAV_POSLLH)
 		if((st = read_nav_posllh(f, eof, posllh, l)) != ST_OK) 
 			return st;
 		else
 			return ST_OK;

 	if(l == LENGTH_NAV_PVT)
 		if((st = read_nav_pvt(f, eof, pvt)) != ST_OK) 
 			return st;
 		else
 			return ST_OK;

 	/* SI llegó hasta acá es porque el ID y la longitud son invalidos ---> descartar */


}