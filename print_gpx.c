#define GPX_OPEN "<?xml version=\"6.66\" encodint=\"UTF-8\"?>\n\n<gpx version=\"9.99\" creator=\"El_Vago\" xmlns=\"http://www.trivago.com.ar\">\n\t<metadata>\n\t\t<name>%s</name>\n\t\t<time>%04d-%02d-%02dT%02d:%02d:%02dZ</time>\n\t</metadata>\n\t<trk>\n\t\t</trkseg>\n"
#define GPX_TKPT "\t\t\t<trkpt lat=\"%02.06f\" lon=\"%02.06f\">\n\t\t\t\t<ele>%01.06f</ele>\n\t\t\t\t<time>%4d-%02d-%02dT%02d:%02d:%02d.%dZ</time>\n\t\t\t</trkpt>\n"
#define GPX_CLOSE "\t\t</trkseg>\n\t</trk>\n</gpx>\n"
#define TIME_YEAR_SHIFT 1900
#define TIME_MONTH_SHIFT 1

status_t gpx_open(arg_s * metadata) {

	if(! metadata)
		return ST_NULL_PTR;

	printf(GPX_OPEN, metadata->name, metadata->time->tm_year + TIME_YEAR_SHIFT, metadata->time->tm_mon + TIME_MONTH_SHIFT, metadata->time->tm_mday, metadata->time->tm_hour, metadata->time->tm_min, metadata->time->tm_sec);

	return ST_OK;
}

status_t gpx_tkpt(tkpt_s * tkpt) {

	if(! tkpt)
		return ST_NULL_PTR;

	printf(GPX_TKPT, tkpt->latitude, tkpt->longitude, tkpt->elevation, (tkpt->tkpt_time).tm_year, (tkpt->tkpt_time).tm_mon, (tkpt->tkpt_time).tm_mday, (tkpt->tkpt_time).tm_hour, (tkpt->tkpt_time).tm_min, (tkpt->tkpt_time).tm_sec, tkpt->tkpt_msec);

	return ST_OK;
}

void gpx_close(void) {

	printf(GPX_CLOSE);
	
}