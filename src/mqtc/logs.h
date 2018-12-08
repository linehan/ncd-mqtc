#ifndef __MQTC_LOGS
#define __MQTC_LOGS
#include <stdarg.h>

void open_logs(void);
void close_logs(void);

void log_cost   (const char *fmt, ...);
void log_fitness(const char *fmt, ...);
void log_alias  (const char *fmt, ...);
void log_mutate (const char *fmt, ...);


#endif
