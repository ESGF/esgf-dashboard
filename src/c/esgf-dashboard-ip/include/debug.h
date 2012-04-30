#ifndef DEBUG_H
#define DEBUG_H
#include <stdarg.h>

#define LOG_ERROR 	1
#define LOG_WARNING 	2
#define LOG_DEBUG	3

#if defined(NDEBUG) && defined(__GNUC__)
/* gcc's cpp has extensions; it allows for macros with a variable number of
   arguments. We use this extension here to preprocess pmesg away. */
#define pmesg(level, source, line_number, format, args...) ((void)0)
#else
void pmesg(int level, char *source, long int line_number, char *format, ...);
/* print a message, if it is considered significant enough.
      Adapted from [K&R2], p. 174 */
#endif

#endif /* DEBUG_H */

