#include <stdlib.h>
#include <stdio.h>
#include "ncd.h"


struct ncd_t *ncd_create(int count)
{
        int i;
        struct ncd_t *ncd;

        ncd = calloc(1, sizeof(struct ncd_t));

        ncd->count = count;
        ncd->size_single = calloc(1, count * sizeof(int));
        ncd->size_double = calloc(1, count * sizeof(int *));

        for (i=0; i<count; i++) {
                ncd->size_double[i] = calloc(1, sizeof(int));
        }

        return ncd;
}


double ncd_compute(int size_a, int size_b, int size_ab)
{
        if (size_a < size_b) {
                return (double)((double)(size_ab - size_a)/(double)size_b);
        } else {
                return (double)((double)(size_ab - size_b)/(double)size_a);
        }
}


void ncd_test(struct ncd_t *ncd)
{
        int i;
        int j;

        for (i=0; i<ncd->count; i++) {
                printf("[%d]: %d\n", i, ncd->size_single[i]);
                for (j=0; j<ncd->count; j++) {
                        printf("[%d][%d]: %d\n", i, j, ncd->size_double[i][j]);
                }
        }
}


void ncd_print_2files(struct ncd_t *ncd, struct directory_t *dir)
{
        int i;
        int j;

        FILE *key = fopen("ncd.key", "w+");
        FILE *dat = fopen("ncd.dat", "w+");

        for (j=0; j<dir->file_count; j++) {
                fprintf(key, "%-2d %s\n", j, dir->file_name[j]);
        }

        for (i=0; i<dir->file_count; i++) {
                fprintf(dat, "\n");
                for (j=0; j<dir->file_count; j++) {
                        fprintf(dat, "%-20g ",
                                ncd_compute(
                                        ncd->size_single[i], 
                                        ncd->size_single[j], 
                                        ncd->size_double[i][j]
                                )
                        );
                }
        }

        fclose(key);
        fclose(dat);
}

void ncd_print(struct ncd_t *ncd, struct directory_t *dir)
{
        int i;
        int j;

        printf("WORKING PATH: %s__ncd/\n", dir->path);
        printf("FILE_COUNT:%d, NCD_COUNT:%d\n", dir->file_count, ncd->count);

        printf("#>>>\n");
        for (j=0; j<dir->file_count; j++) {
                printf("%-2d %s\n", j, dir->file_name[j]);
        }
        printf("#>>>\n");

        printf("%-2s ", " ");
        for (j=0; j<dir->file_count; j++) {
                printf("%-20d ", j);
        }

        for (i=0; i<dir->file_count; i++) {
                printf("\n");
                for (j=0; j<dir->file_count; j++) {
                        if (j == 0) {
                                printf("%-2d ", i);
                        }

                        printf("%-20g ",
                                ncd_compute(
                                        ncd->size_single[i], 
                                        ncd->size_single[j], 
                                        ncd->size_double[i][j]
                                )
                        );
                }
        }
}

