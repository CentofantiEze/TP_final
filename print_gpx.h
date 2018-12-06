#define GPX_OPEN "<?xml version=\"6.66\" encodint=\"UTF-8\"?>\n\n<gpx version=\"9.99\" creator=\"El_Vago\" xmlns=\"http://www.trivago.com.ar\">\n\t<metadata>\n\t\t<name>%s</name>\n\t\t<time>%04d-%02d-%02dT%02d:%02d:%02dZ</time>\n\t</metadata>\n\t<trk>\n\t\t</trkseg>\n"
#define GPX_TKPT "\t\t\t<trkpt lat=\"%02.06f\" lon=\"%02.06f\">\n\t\t\t\t<ele>%01.06f</ele>\n\t\t\t\t<time>%4d-%02d-%02dT%02d:%02d:%02d.%dZ</time>\n\t\t\t</trkpt>\n"
#define GPX_CLOSE "\t\t</trkseg>\n\t</trk>\n</gpx>\n"
#define TIME_YEAR_SHIFT 1900
#define TIME_MONTH_SHIFT 1

status_t gpx_open(arg_s * metadata);
status_t gpx_tkpt(FILE *, tkpt_s * tkpt);
status_t gpx_close(FILE * f);