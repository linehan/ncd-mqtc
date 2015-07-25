#ifndef _BNFOP_H
#define _BNFOP_H
#include <stdbool.h>
#include "util.h"

extern bool bnfop(int argc, char *argv[], const char *usage, ...);

/******************************************************************************
 * Private stuff
 ******************************************************************************/
static const char * UNUSED(__BNF_NOARGS);

#define BNF_TERM " $ " 

/*
 * Assumes bnf() is being called within main(), and that argc and argv
 * are present.
 */
#define __bnf(usage, ...) \
        bnfop(argc, argv, usage, __VA_ARGS__)

/* 
 * Appends the dummy value NOARGS to the variable argument list, to
 * handle cases where no argument is otherwise supplied. Because the
 * number of arguments taken from the list is restricted first by the
 * value of argc, NOARGS will never be assigned when there truly are
 * "no args."
 *
 * We also append, to every usage string, a hidden ' $ ' value that
 * denotes the end of the line, so we can tell the difference between 
 * a malformed and non-malformed string. I'll explain later.
 */
#define __bnf_append_noargs(usage, ...) \
        __bnf(usage, __VA_ARGS__ VA_COMMA(__VA_ARGS__) __BNF_NOARGS)


/******************************************************************************
 * Public stuff
 ******************************************************************************/
/*
 * The macro to be used.
 */
#define bnf(usage, ...) \
        __bnf_append_noargs(usage, __VA_ARGS__) 


#define program_invoked_as(name) ((strcmp(name, argv[0]) == 0))


#endif
