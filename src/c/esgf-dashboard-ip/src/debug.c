#include "debug.h"
#include <stdio.h>

extern int msglevel; /* the higher, the more messages... */

#if defined(NDEBUG) && defined(__GNUC__)
/* Nothing. pmesg has been "defined away" in debug.h already. */
#else
void pmesg(int level, char* format, ...) {
#ifdef NDEBUG
	/* Empty body, so a good compiler will optimise calls
	   to pmesg away */
#else
        va_list args;
	char log_type[10];
        if (level>msglevel)
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
	
	fprintf(stderr,"[%s][%s][%d] ",log_type, __FILE__,__LINE__);

        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
#endif /* NDEBUG */
#endif /* NDEBUG && __GNUC__ */
}
        
