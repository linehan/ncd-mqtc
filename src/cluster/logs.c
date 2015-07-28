#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

FILE *F_cost;
FILE *F_fitness;
FILE *F_alias;
FILE *F_mutate;

void open_logs(void)
{
        F_alias   = fopen("./log/alias.log",   "w+");
        F_mutate  = fopen("./log/mutate.log",  "w+");
        F_fitness = fopen("./log/fitness.log", "w+");
        F_cost    = fopen("./log/cost.log",    "w+");
}

void close_logs(void)
{
        fclose(F_cost);
        fclose(F_alias);
        fclose(F_mutate);
        fclose(F_fitness);
}

void log_cost(const char *fmt, ...)
{
        va_list ap;

        if (F_cost != NULL && ferror(F_cost) == 0) {
                va_start(ap, fmt); 
                vfprintf(F_cost, fmt, ap);
                va_end(ap);
        }
}

void log_fitness(const char *fmt, ...)
{
        va_list ap;

        if (F_fitness != NULL && ferror(F_fitness) == 0) {
                va_start(ap, fmt); 
                vfprintf(F_fitness, fmt, ap);
                va_end(ap);
        }
}

void log_alias(const char *fmt, ...)
{
        va_list ap;

        if (F_alias != NULL && ferror(F_alias) == 0) {
                va_start(ap, fmt); 
                vfprintf(F_alias, fmt, ap);
                va_end(ap);
        }
}

void log_mutate(const char *fmt, ...)
{
        va_list ap;

        if (F_mutate != NULL && ferror(F_mutate) == 0) {
                va_start(ap, fmt); 
                vfprintf(F_mutate, fmt, ap);
                va_end(ap);
        }
}
