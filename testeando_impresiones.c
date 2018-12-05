#include <stdio.h>
#define GPX_OPEN "<?xml version=\"6.66\" encodint=\"UTF-8\"?>\n\n<gpx version=\"9.99\" creator=\"El_Vago\" xmlns=\"http://www.trivago.com.ar\">\n\t<metadata>\n\t\t<name>%s</name>\n\t\t<time>%04d-%02d-%02dT%02d:%02d:%02dZ</time>\n\t</metadata>\n\t<trk>\n"
#define GPX_TKPT "\t\t\t<trkpt lat=\"%02.06f\" lon=\"%02.06f\">\n\t\t\t\t<ele>%01.06f</ele>\n\t\t\t\t<time>%4d-%02d-%02dT%02d:%02d:%02d.%dZ</time>\n\t\t\t</trkpt>\n"
#define

int main(void) {

	printf(GPX_OPEN, "Pablo el Vago",2018,3,4,12,50,47);
	printf(GPX_TKPT, 34.21,-35.241,0.000045161, 1997,12,6,3,11,25,18);
	return 0;

}