#include <stdlib.h>
#include "math.h"


static unsigned long gcd_ui(unsigned long x, unsigned long y) 
{
        unsigned long t;

        if (y < x) { 
                t = x; 
                x = y; 
                y = t; 
        }

        /* 
         * y1<-(x0%y0), x1<-y0 
         */
        while (y > 0) {
                t = y;  
                y = x%y;  
                x = t;
        }

        return x;
}
 
unsigned long binomial(unsigned long n, unsigned long k) 
{
        unsigned long d = 1;
        unsigned long g = 1;
        unsigned long r = 1;

        if (k == 0) {
                return 1;
        }
        if (k == 1) {
                return n;
        }
        if (k >= n) {
                return (k == n);
        }
        if (k > n/2) {
                k = n-k;
        }

        for (d=1; d<=k; d++) {
                /* Possible overflow */
                if (r >= ULONG_MAX/n) {  
                        /* Reduced numerator */
                        unsigned long nr;
                        /* Reduced denominator */
                        unsigned long dr; 

                        g  = gcd_ui(n, d);  
                        nr = n/g;  
                        dr = d/g;

                        g  = gcd_ui(r, dr);  
                        r  = r/g;  
                        dr = dr/g;

                        if (r >= ULONG_MAX/nr) {
                                /* Unavoidable overflow */
                                return 0;  
                        }

                        r *= nr;
                        r /= dr;
                        n--;
                } else {
                        r *= n--;
                        r /= d;
                }
        }
        return r;
}




float max_float(int n, ...)
{
        va_list arg_ptr;
        float max;
        float num;
        int i;

        /* Requires the last positional parameter (to get the address) */
        va_start(arg_ptr, n); 

        max = 0;

        for (i=0; i<n; i++) {
                /* 
                 * Requires the type to cast the argument into.
                 *
                 * Also increments the argument pointer up to the 
                 * next argument.
                 */
                num = va_arg(arg_ptr, double); 
                max = (max < num) ? num : max;
        }

        /* Free memory and reset. */
        va_end(arg_ptr);

        return max;
}


float min_float(int n, ...)
{
        va_list arg_ptr;
        float min;
        float num;
        int i;

        /* Requires the last positional parameter (to get the address) */
        va_start(arg_ptr, n); 

        min = 0;

        for (i=0; i<n; i++) {
                /* 
                 * Requires the type to cast the argument into.
                 *
                 * Also increments the argument pointer up to the 
                 * next argument.
                 */
                num = va_arg(arg_ptr, double); 
                min = (min > num) ? num : min;
        }

        /* Free memory and reset. */
        va_end(arg_ptr);

        return min;
}


int max_int(int n, ...)
{
        va_list arg_ptr;
        int max;
        int num;
        int i;

        /* Requires the last positional parameter (to get the address) */
        va_start(arg_ptr, n); 

        max = 0;

        for (i=0; i<n; i++) {
                /* 
                 * Requires the type to cast the argument into.
                 *
                 * Also increments the argument pointer up to the 
                 * next argument.
                 */
                num = va_arg(arg_ptr, int); 
                max = (max < num) ? num : max;
        }

        /* Free memory and reset. */
        va_end(arg_ptr);

        return max;
}


int min_int(int n, ...)
{
        va_list arg_ptr;
        int min;
        int num;
        int i;

        /* Requires the last positional parameter (to get the address) */
        va_start(arg_ptr, n); 

        min = 0;

        for (i=0; i<n; i++) {
                /* 
                 * Requires the type to cast the argument into.
                 *
                 * Also increments the argument pointer up to the 
                 * next argument.
                 */
                num = va_arg(arg_ptr, int); 
                min = (min > num) ? num : min;
        }

        /* Free memory and reset. */
        va_end(arg_ptr);

        return min;
}


