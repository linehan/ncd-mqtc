#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "filesystem.h"
#include "module/mod_zlib.h"
#include "module/mod_bzlib.h"

struct ncd_t {
        int count;
        int *size_single;
        int **size_double;
};

struct ncd_t *make_ncd(int count)
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

double ncd_compute(int size_a, int size_b, int size_ab)
{
        if (size_a < size_b) {
                return (double)((double)(size_ab - size_a)/(double)size_b);
        } else {
                return (double)((double)(size_ab - size_b)/(double)size_a);
        }
}

typedef int (*cmpr1_t)(FILE*, FILE*);
typedef int (*cmpr2_t)(FILE*, FILE*, FILE*);


void run_compress(struct directory_t *dir, cmpr1_t __cmpr1, cmpr2_t __cmpr2, const char *ext)
{
        int i;
        int j;

        FILE *one;
        FILE *two;
        FILE *dst;

        int one_z;
        int two_z;
        int dst_z;

        struct ncd_t *ncd = make_ncd(dir->file_count);

        mkdirf(S_IRWXU|S_IRWXG|S_IRWXO, "%s__ncd", dir->path);

        printf("WORKING PATH: %s__ncd/\n\n", dir->path);

        printf("file_count:%d, ncd_count:%d\n", dir->file_count, ncd->count);

        for (i=0; i<dir->file_count; i++) {

                one = fopen(dir->file_path[i], "r+");
                dst = fopenf("w+", "%s__ncd/%s.%s", dir->path, dir->file_name[i], ext);

                if (one != NULL && dst != NULL) {
                        __cmpr1(one, dst);

                        one_z = (int)file_length(one);
                        dst_z = (int)file_length(dst);

                        ncd->size_single[i] = dst_z;

                        fclose(one);
                        fclose(dst);

                        printf("SOURCE: %s\n", 
                                dir->file_name[i]
                        );

                        printf("TARGET: %s.%s\n", 
                                dir->file_name[i],
                                ext
                        );

                        printf("\tSource size: %d bits\n", 
                                one_z 
                        );
                        printf("\tTarget size: %d bits\n", 
                                dst_z 
                        );

                        printf("\tCompressed: %d bits\n", 
                                one_z - dst_z
                        );

                        printf("\tComp. Ratio: %g\n", 
                                (double)((double)dst_z/(double)one_z)
                        );

                        printf("\n");
                } else {
                        printf("One of the file streams is NULL\n");
                        /* 
                         * Don't let the size_single array get fucked 
                         * by leaving a gap from the else { condition 
                         */
                        abort();
                }
        }

        for (i=0; i<dir->file_count; i++) {
                for (j=0; j<dir->file_count; j++) {

                        one = fopen(dir->file_path[i], "r+");
                        two = fopen(dir->file_path[j], "r+");

                        dst = fopenf("w+", "%s__ncd/%s%s.%s", dir->path, dir->file_name[i], dir->file_name[j], ext);

                        if (one != NULL && two != NULL && dst != NULL) {
                                __cmpr2(one, two, dst);

                                one_z = (int)file_length(one);
                                two_z = (int)file_length(two);
                                dst_z = (int)file_length(dst);

                                ncd->size_double[i][j] = dst_z;

                                fclose(one);
                                fclose(two);
                                fclose(dst);

                                printf("SOURCE: %s(+)%s\n", 
                                        dir->file_name[i], 
                                        dir->file_name[j]
                                );

                                printf("TARGET: %s%s.%s\n", 
                                        dir->file_name[i], 
                                        dir->file_name[j],
                                        ext
                                );

                                printf("\tSource size: %d bits\n", 
                                        one_z + two_z
                                );
                                printf("\tTarget size: %d bits\n", 
                                        dst_z 
                                );

                                printf("\tCompressed:  %d bits\n", 
                                        (one_z+two_z) - dst_z
                                );

                                printf("\tComp. Ratio: %g\n", 
                                        (double)((double)dst_z/(double)((double)one_z+(double)two_z))
                                );

                                printf("\n");
                        } else {
                                printf("One of the file streams is NULL\n");
                                /* 
                                 * Don't let the size_single array get fucked 
                                 * by leaving a gap from the else { condition 
                                 */
                                abort();
                        }
                }
        }

        /* Blank place */
        printf("%-20s ", " ");
        for (j=0; j<ncd->count; j++) {
                printf("%-20s ", dir->file_name[j]);
        }

        for (i=0; i<ncd->count; i++) {
                printf("\n");
                for (j=0; j<ncd->count; j++) {
                        if (j == 0) {
                                printf("%-20s ", dir->file_name[i]);
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

void run_zpaq(struct directory_t *dir)
{
        int i;
        int j;

        FILE *one;
        FILE *two;
        FILE *dst;

        int one_z;
        int two_z;
        int dst_z;

        struct ncd_t *ncd = make_ncd(dir->file_count);

        mkdirf(S_IRWXU|S_IRWXG|S_IRWXO, "%s__ncd", dir->path);

        printf("WORKING PATH: %s__ncd/\n\n", dir->path);

        for (i=0; i<dir->file_count; i++) {

                shell("zpaq a %s__ncd/%s.zpaq %s -method 56", 
                        dir->path, 
                        dir->file_name[i],
                        dir->file_path[i]
                );

                one = fopen(dir->file_path[i], "r+");
                dst = fopenf("r+", "%s__ncd/%s.zpaq", dir->path, dir->file_name[i]);

                if (one != NULL && dst != NULL) {

                        one_z = (int)file_length(one);
                        dst_z = (int)file_length(dst);

                        ncd->size_single[i] = dst_z;

                        fclose(one);
                        fclose(dst);

                        printf("SOURCE: %s\n", 
                                dir->file_name[i]
                        );

                        printf("TARGET: %s.zpaq\n", 
                                dir->file_name[i]
                        );

                        printf("\tSource size: %d bits\n", 
                                one_z 
                        );
                        printf("\tTarget size: %d bits\n", 
                                dst_z 
                        );

                        printf("\tCompressed: %d bits\n", 
                                one_z - dst_z
                        );

                        printf("\tComp. Ratio: %g\n", 
                                (double)((double)dst_z/(double)one_z)
                        );

                        printf("\n");
                } else {
                        /*printf("One of the file streams is NULL\n");*/
                        printf("One of the file streams is NULL for %s\n", dir->file_name[i]);
                        /* 
                         * Don't let the size_single array get fucked 
                         * by leaving a gap from the else { condition 
                         */
                        abort();
                }
        }

        for (i=0; i<dir->file_count; i++) {
                for (j=0; j<dir->file_count; j++) {

                        shell("zpaq a %s__ncd/%s%s.zpaq %s %s -method 56", 
                                dir->path, 
                                dir->file_name[i],
                                dir->file_name[j],
                                dir->file_path[i],
                                dir->file_path[j]
                        );

                        one = fopen(dir->file_path[i], "r+");
                        two = fopen(dir->file_path[j], "r+");

                        dst = fopenf("r+", "%s__ncd/%s%s.zpaq", dir->path, dir->file_name[i], dir->file_name[j]);

                        if (one != NULL && two != NULL && dst != NULL) {
                                /*__cmpr2(one, two, dst);*/

                                one_z = (int)file_length(one);
                                two_z = (int)file_length(two);
                                dst_z = (int)file_length(dst);

                                ncd->size_double[i][j] = dst_z;

                                fclose(one);
                                fclose(two);
                                fclose(dst);

                                printf("SOURCE: %s(+)%s\n", 
                                        dir->file_name[i], 
                                        dir->file_name[j]
                                );

                                printf("TARGET: %s%s.zpaq\n", 
                                        dir->file_name[i], 
                                        dir->file_name[j]
                                );

                                printf("\tSource size: %d bits\n", 
                                        one_z + two_z
                                );
                                printf("\tTarget size: %d bits\n", 
                                        dst_z 
                                );

                                printf("\tCompressed:  %d bits\n", 
                                        (one_z+two_z) - dst_z
                                );

                                printf("\tComp. Ratio: %g\n", 
                                        (double)((double)dst_z/(double)((double)one_z+(double)two_z))
                                );

                                printf("\n");
                        } else {
                                printf("One of the file streams is NULL for %s, %s\n", dir->file_name[i], dir->file_name[j]);
                                /* 
                                 * Don't let the size_single array get fucked 
                                 * by leaving a gap from the else { condition 
                                 */
                                abort();
                        }
                }
        }

        /* Blank place */
        printf("%-20s ", " ");
        for (j=0; j<ncd->count; j++) {
                printf("%-20s ", dir->file_name[j]);
        }

        for (i=0; i<ncd->count; i++) {
                printf("\n");
                for (j=0; j<ncd->count; j++) {
                        if (j == 0) {
                                printf("%-20s ", dir->file_name[i]);
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

void just_ncd(struct directory_t *dir)
{
        int i;
        int j;

        FILE *one;
        FILE *two;
        FILE *dst;

        int one_z;
        int two_z;
        int dst_z;

        struct ncd_t *ncd = make_ncd(dir->file_count);

        printf("WORKING PATH: %s__ncd/\n", dir->path);
        printf("FILE_COUNT:%d, NCD_COUNT:%d\n", dir->file_count, ncd->count);
        printf("#>>>\n");

        for (i=0; i<dir->file_count; i++) {

                if (!dir->file_path[i]) {
                        printf("Missing file_path[i:%d]\n", i);
                        return;
                }
                if (!dir->file_name[i]) {
                        printf("Missing file_name[i:%d]\n", i);
                        return;
                }

                one = fopen(dir->file_path[i], "r+");
                dst = fopenf("r+", "%s__ncd/%s.zpaq", dir->path, dir->file_name[i]);

                if (one != NULL && dst != NULL) {

                        one_z = (int)file_length(one);
                        dst_z = (int)file_length(dst);

                        ncd->size_single[i] = dst_z;

                        if (fclose(one) != 0) {
                                printf("Error on one: %s\n", strerror(errno));
                                return;
                        }
                        if (fclose(dst) != 0) {
                                printf("Error on dst\n");
                                return;
                        }

                        /*printf("[%d]\n", i);*/

                        /*printf("SOURCE: %s\n", */
                                /*dir->file_name[i]*/
                        /*);*/

                        /*printf("TARGET: %s.zpaq\n", */
                                /*dir->file_name[i]*/
                        /*);*/

                        /*printf("\tSource size: %d bits\n", */
                                /*one_z */
                        /*);*/
                        /*printf("\tTarget size: %d bits\n", */
                                /*dst_z */
                        /*);*/

                        /*printf("\tCompressed: %d bits\n", */
                                /*one_z - dst_z*/
                        /*);*/

                        /*printf("\tComp. Ratio: %g\n", */
                                /*(double)((double)dst_z/(double)one_z)*/
                        /*);*/

                        /*printf("\n");*/
                } else {
                        printf("One of the file streams is NULL for %s\n", dir->file_name[i]);
                        /* 
                         * Don't let the size_single array get fucked 
                         * by leaving a gap from the else { condition 
                         */
                        return;
                        /*exit(1);*/
                }
        }

        for (i=0; i<dir->file_count; i++) {
                for (j=0; j<dir->file_count; j++) {

                        if (!dir->file_path[i]) {
                                printf("Missing file_path[i:%d]\n", i);
                                return;
                        }
                        if (!dir->file_path[j]) {
                                printf("Missing file_path[j:%d]\n", j);
                                return;
                        }
                        if (!dir->file_name[i]) {
                                printf("Missing file_name[i:%d]\n", i);
                                return;
                        }
                        if (!dir->file_name[j]) {
                                printf("Missing file_name[j:%d]\n", j);
                                return;
                        }

                        one = fopen(dir->file_path[i], "r+");
                        two = fopen(dir->file_path[j], "r+");

                        dst = fopenf("r+", "%s__ncd/%s%s.zpaq", dir->path, dir->file_name[i], dir->file_name[j]);

                        if (one != NULL && two != NULL && dst != NULL) {

                                one_z = (int)file_length(one);
                                two_z = (int)file_length(two);
                                dst_z = (int)file_length(dst);

                                ncd->size_double[i][j] = dst_z;

                                /* Doesn't like this ONE fclose call... */

                                /*if (fclose(one) != 0) {*/
                                        /*printf("Error on one\n");*/
                                        /*return;*/
                                /*} else {*/
                                        /*printf("closed one!\n");*/
                                /*}*/

                                if (fclose(two) != 0) {
                                        printf("Error on two\n");
                                        return;
                                }
                                if (fclose(dst) != 0) {
                                        printf("Error on dst\n");
                                        return;
                                }

                                /*printf("[%d][%d]\n", i, j);*/

                                /*printf("SOURCE: %s(+)%s\n", */
                                        /*dir->file_name[i], */
                                        /*dir->file_name[j]*/
                                /*);*/

                                /*printf("TARGET: %s%s.zpaq\n", */
                                        /*dir->file_name[i], */
                                        /*dir->file_name[j]*/
                                /*);*/

                                /*printf("\tSource size: %d bits\n", */
                                        /*one_z + two_z*/
                                /*);*/
                                /*printf("\tTarget size: %d bits\n", */
                                        /*dst_z */
                                /*);*/

                                /*printf("\tCompressed:  %d bits\n", */
                                        /*(one_z+two_z) - dst_z*/
                                /*);*/

                                /*printf("\tComp. Ratio: %g\n", */
                                        /*(double)((double)dst_z/(double)((double)one_z+(double)two_z))*/
                                /*);*/

                                /*printf("\n");*/
                        } else {
                                printf("One of the file streams is NULL for %s, %s\n", dir->file_name[i], dir->file_name[j]);
                                /* 
                                 * Don't let the size_single array get fucked 
                                 * by leaving a gap from the else { condition 
                                 */
                                return;
                                /*exit(1);*/
                                /*abort();*/
                        }
                }
        }

        /* Blank place */
        printf("KEY\n");
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


int main(int argc, char **argv)
{
        struct directory_t dir;

        if (argc == 3) {
                directory_scan(argv[2], &dir);

                if (!strcmp(argv[1], "--zlib")) {
                        run_compress(&dir, zlib_compress, zlib_compress_cat, "gz");
                } else if (!strcmp(argv[1], "--bzlib")) {
                        run_compress(&dir, bzlib_compress, bzlib_compress_cat, "bz2");
                } else if (!strcmp(argv[1], "--zpaq")) {
                        run_zpaq(&dir);
                } else if (!strcmp(argv[1], "--zpaqncd")) {
                        just_ncd(&dir);
                } else {
                        printf("%s? I don't know that one.\n", argv[1]);
                }
        } else {
                printf("Usage: %s --zlib|--bzlib|--zpaq|--zpaqncd <DIRECTORY>\n", argv[0]);
        }

        return 0;

}


