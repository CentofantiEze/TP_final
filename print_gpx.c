status_t gpx_open(arg_s * metadata) {

	if(! metadata)
		return ST_NULL_PTR;

	if(fprintf(metadata->outfile, GPX_OPEN, metadata->name, metadata->time->tm_year + TIME_YEAR_SHIFT, metadata->time->tm_mon + TIME_MONTH_SHIFT, metadata->time->tm_mday, metadata->time->tm_hour, metadata->time->tm_min, metadata->time->tm_sec) <= 0)
		return ST_CORRUPT_FILE;

	return ST_OK;
}

status_t gpx_tkpt(FILE * f, tkpt_s * tkpt) {

	if(! tkpt || ! f)
		return ST_NULL_PTR;

	if(fprintf(f, GPX_TKPT, tkpt->latitude, tkpt->longitude, tkpt->elevation, (tkpt->tkpt_time).tm_year, (tkpt->tkpt_time).tm_mon, (tkpt->tkpt_time).tm_mday, (tkpt->tkpt_time).tm_hour, (tkpt->tkpt_time).tm_min, (tkpt->tkpt_time).tm_sec, tkpt->tkpt_msec) <= 0)
		return ST_CORRUPT_FILE;

	return ST_OK;
}

status_t gpx_close(FILE * f) {

	if(! f)
		return ST_NULL_PTR;

	if(fprintf(f ,GPX_CLOSE) <= 0)
		return ST_CORRUPT_FILE;

	return ST_OK;
}