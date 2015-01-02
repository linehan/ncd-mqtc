#define _JDL_NO_PRINT_DEBUG
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "bnfop.h"

/******************************************************************************
 * HELPER UTILITIES
 ******************************************************************************/

/**
 * is_ws
 * `````
 * Test whether a character is a whitespace character or not.
 *
 * @c    : Character to be tested.
 * Return: TRUE if character is whitespace, else FALSE.
 */
static inline bool is_ws(char c) 
{
        switch (c) {
        case ' ':
        case '\n':
        case '\t':
        case '\f':
        case '\r':
                return true;
        default:
                return false;
        }
}


/**
 * next_word
 * `````````
 * Advance a pointer to the next whitespace-delimited word, or EOL.
 *
 * @ptr  : Pointer to a string.
 * Return: Pointer to the beginning of the next word, or to EOL.
 */
static inline const char *next_word(const char *haystack)
{
        const char *ptr;

        if (haystack == NULL) {
                /*DEBUG("NULL pointer passed to next_word()\n");*/
                return NULL;
        }

        ptr = haystack;

        /* 
         * We're at the NUL byte, the end of the 
         * haystack string. There is no "next word".
         */

        if (*ptr == 0) {
                return ptr; 
        }

        /*
         * If the pointer is already located inside 
         * a word, we must first advance to the end 
         * of it before looking for the next one.
         */

        while (!is_ws(*ptr)) {
                ptr++;

                /*
                 * The word we were inside of turned
                 * out to be the last word in the
                 * haystack, so there is no "next word".
                 */

                if (*ptr == 0) {
                        return ptr;
                }
        }

        /* 
         * If ptr is now whitespace, (we assume words 
         * are generally whitespace-delimited), advance 
         * to the next non-whitespace character, which
         * should be the first character of the 
         * "next word".
         */

        while (is_ws(*ptr)) {
                ptr++;

                /*
                 * Again check if we've reached the end of the
                 * haystack. Not necessary, but included so it's
                 * easier to understand.
                 */

                if (*ptr == 0) {
                        return ptr;
                }
        }

        /* 
         * We are at the beginning of the next word. 
         */

        return ptr;
}


static inline const char next_char(const char *ptr)
{
        const char *c;

        c = next_word(ptr);

        return (c) ? *c : 0;
}


/******************************************************************************
 * BNFOP 
 ******************************************************************************/
        
/**
 * bnfop
 * `````
 * Use a BNF format string to validate arguments and print usage.
 *
 * @argc : Argument count.
 * @argv : Argument vector.
 * @usage: Format string written in mini-BNF.
 * @...  : Variable number of arguments (one for each <foo> in @usage).
 * Return: TRUE if match, else FALSE.
 * 
 * NOTE
 * May abort program execution if a malformed message is detected.
 *
 * USAGE
 *
 *      With a binary infix operator (@):
 *      --------------------------------
 *      const char *name;
 *      const char *host;
 *
 *      if (bnfop("prog <name> @ <host>", &name, &host)) {
 *              my_func(name, host);
 *      }
 *
 *
 *      With a traditional short option flag:
 *      ------------------------------------
 *      const char *name;
 *
 *      if (bnfop("prog -o <name>", &name)) {
 *              my_func(name);
 *      }
 *
 *
 *      With only a program invocation:
 *      ------------------------------
 *      const char *name;
 *
 *      if (bnfop("prog <name> $", &name)) {
 *              my_func(name);
 *      }
 *
 *
 *      With no arguments:
 *      -----------------
 *      if (bnfop("prog $", &name)) {
 *              my_func(name);
 *      }
 *      
 * 
 */
bool bnfop(int argc, char *argv[], const char *usage, ...)
{
        /* Pointer to scan along the @usage string. */
        const char *ptr = NULL;
        /* Flag position inside an optional section */ 
        const char *flg = NULL;

        /* Points to the variable argument list (@...) */
        char **param;
        /* Initialized variable argument list */
        va_list args;           

        /* Length, used for option words */
        size_t len;

        /* If the first (program name) option fits */
        bool onematch = false;
        /* If the second (first option word) fits */
        bool twomatch = false;

        /* If an argument was consumed on the last loop */
        bool consumed_argument = true;

        /* Number of tokens processed (indexes argv[]) */
        int n = 0;

        ptr = usage;

        va_start(args, usage);

        if (is_ws(*ptr)) {
                /*DEBUG("Leading whitespace in option string\n");*/
                abort();
        }


        do {
                /* 
                 * Parse a required argument, enclosed in
                 * '<' and '>'
                 *
                 * If no option flags are used, the parameter 
                 * is positional, in that the ordering is the 
                 * only way we can parse it.
                 *
                 *      e.g. "foo <host> <port> <proto>"
                 *
                 * There is no way for the parser to validate
                 * what is a host, a port, etc, other than the
                 * order.
                 *
                 * Option flags let us associate positions with 
                 * values, so that each flag delimits a "frame"
                 * of arguments. 
                 *
                 *      e.g. "foo -h <host> <port> -p <proto>"
                 *
                 * Within each frame, of course, parameters are 
                 * again parsed positionally.
                 */

                if (*ptr == '<') {

                        /* 
                         * Move the parser pointer past the
                         * parameter token, to ensure it is
                         * properly delimited.
                         */

                        while (*ptr != '>') {
                                ptr++;
                                if (*ptr == 0) {
                                        /*DEBUG("Unterminated '<'\n");*/
                                        abort();
                                }
                        }

                        /* 
                         * Get the next pointer in the variadic
                         * argument list, and set it to the address
                         * of argv[n].
                         */ 

                        param  = va_arg(args, char **);
                        *param = argv[n];

                        /*DEBUG("Got required argument, currently %c\n", *ptr);*/
                        consumed_argument = true;

                        goto next_token;
                }

                /*
                 * Parse an optional argument, enclosed in
                 * '[' and ']'.
                 *
                 * The optional argument must be in one of
                 * two forms. 
                 *
                 *   1. Follow a required argument or option flag,
                 *      with a new option frame immediately after.
                 *      
                 *      e.g. "foo -c <host> [port]"
                 *           "foo -c <host> [port] -p <proto>"
                 *
                 *   2. Contain an option flag itself.
                 *
                 *      e.g. "foo -c <host> [-p port] <proto>"
                 */

                if (*ptr == '[') {

                        /*DEBUG("Detected optional argument\n");*/

                        /* 
                         * Move the parser pointer past the
                         * parameter token, to ensure it is
                         * properly delimited.
                         */

                        while (*ptr != ']') {
                                ptr++;
                                if (*ptr == 0) {
                                        /*DEBUG("Unterminated '['\n");*/
                                        abort();
                                }

                                /*
                                 * An option flag interior to the
                                 * optional argument. Save it for
                                 * later processing.
                                 */

                                if (*ptr == '-') {
                                        flg = ptr;
                                }
                        }

                        /*
                         * We must determine if the optional argument 
                         * is present or not. Using the rules above, 
                         * our three cases are:
                         * 
                         *      0. This is the last token, and an
                         *         unconsumed value exists at that 
                         *         position in argv[].
                         *      
                         *      1. This token is followed by an option
                         *         flag, but the value at this position
                         *         in argv[] does not begin with '-'.
                         *
                         *      2. This token contains an option flag,
                         *         and the value at this position in
                         *         argv[] contains that option flag.
                         */ 

                        param = va_arg(args, char **);

                        /* 0. */
                        if (*(ptr+1) == 0) {
                                /*DEBUG("Got optional argument (0)\n");*/

                                *param = argv[n];

                                consumed_argument = true;
                                goto next_token;
                        }

                        /* 1 */
                        if (*argv[n] != '-' && next_char(ptr) == '-') {
                                /*DEBUG("Got optional argument (1)\n");*/

                                *param = argv[n];

                                consumed_argument = true;
                                goto next_token;
                        }

                        /* 2 */
                        if (*argv[n] == '-' && flg != NULL) {

                                /*
                                 * Does the flag argued match the flag 
                                 * specified in the token?
                                 */

                                len = strlen(argv[n]);

                                if (memcmp(flg, argv[n], len) == 0) {
                                        /*DEBUG("Got optional argument (2)\n");*/
                                        
                                        /* 
                                         * We must advance the argv[]
                                         * counter out-of-band, so 
                                         * we don't get the flag
                                         * instead of its argument.
                                         */
                                        n += 1;

                                        *param = argv[n];

                                        consumed_argument = true;
                                        goto next_token;
                                }
                        }

                        *param = NULL;
                        consumed_argument = false;

                        goto next_token;
                }
 
                /*
                 * If we got here, we need to read a 'bare' word,
                 * perhaps an option word, or flag.
                 */

                len = strlen(argv[n]);

                if (memcmp(ptr, argv[n], len) == 0) {

                        if (n == 0) {
                                /* Program name */
                                onematch = true;
                        } else {
                                /* At least one option */
                                twomatch = true;
                        }

                        /* 
                         * Advance past the word 
                         */

                        ptr += len; 

                        /*DEBUG("WORD MATCH\n");*/
                        consumed_argument = true;
                        goto next_token;
                } else {
                        break;
                }

next_token:
                /*
                 * Move the pointer to the next word in
                 * the usage string.
                 *
                 * Advance the argv[] index if an argument
                 * has been consumed.
                 */

                /*DEBUG("old: %s\n", ptr);*/
                ptr = next_word(ptr);
                /*DEBUG("new: %s\n", ptr);*/

                if (*ptr != 0) {
                        if (consumed_argument == true) {
                                n += 1;
                        }
                } else {
                        break;
                }

        } while (n != argc);

        /*
         * Clean up after the variable argument handlers
         */

        va_end(args); 

        /*
         * Judgement!
         */

        if (twomatch == true) {
                if (*ptr == 0) {
                        return true;
                /* Optional argument omitted */
                } else if (*ptr == '[') {
                        return true;
                } else {
                        fprintf(stderr, "(Usage) %s\n", usage);
                        exit(1); /* malformed */
                }
        } else if (onematch == true) {
                if (*ptr == '$') {
                        return true;
                } else {
                        /*DEBUG("NO MATCH\n");*/
                        return false;
                }
        }

        return false;
}

