#ifndef __NCD_H
#define __NCD_H
#include "filesystem.h"

struct ncd_t {
        int count;
        int *size_single;
        int **size_double;
};

struct ncd_t *ncd_create (int count);
void          ncd_test   (struct ncd_t *ncd);
void          ncd_print  (struct ncd_t *ncd, struct directory_t *dir);
double        ncd_compute(int size_a, int size_b, int size_ab);
void          ncd_print_2files(struct ncd_t *ncd, struct directory_t *dir);


#endif
