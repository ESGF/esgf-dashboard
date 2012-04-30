#include "debug.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

extern int msglevel; /* the higher, the more messages... */

#if defined(NDEBUG) && defined(__GNUC__)
/* Nothing. pmesg has been "defined away" in debug.h already. */
#else
void pmesg(int level, char* source, long int line_number, char* format, ...) {
#ifdef NDEBUG
	/* Empty body, so a good compiler will optimise calls
	   to pmesg away */
#else
        va_list args;
	char log_type[10];
	int new_msglevel=msglevel % 10;
        if (level>new_msglevel)
                return;
		
	switch ( level ) {
		case 1:
		  sprintf(log_type,"ERROR");
		  break;
		case 2:
		  sprintf(log_type,"WARNING");
		  break;
		case 3:
		  sprintf(log_type,"DEBUG");
		  break;
		default:
		  sprintf(log_type,"UNKNOWN");
		  break;
		}
	if (msglevel>10) {
		time_t t1=time(NULL);
		char *s=ctime(&t1);
             	s[strlen(s)-1]=0;        // remove \n
		fprintf(stderr,"[%s][%s][%s][%d] ",s,log_type, source,line_number);
	} else {
		fprintf(stderr,"[%s][%s][%d] ",log_type, source,line_number);
	}
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
#endif /* NDEBUG */
#endif /* NDEBUG && __GNUC__ */
}
        
