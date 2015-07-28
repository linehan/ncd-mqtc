#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
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

        for (i=0; i<dir->file_count; i++) {

                one = fopen(dir->file_path[i], "r+");
                dst = fopenf("w+", "%s__ncd/%s.%s", dir->path, dir->file_name[i], ext);

                if (one != NULL && dst != NULL) {
                        __cmpr1(one, dst);

                        one_z = (int)file_length(one);
                        dst_z = (int)file_length(dst);

                        ncd->size_single[i] = dst_z;

                        fprintf(stderr,
                                "SOURCE: %s\n" 
                                "TARGET: %s.zpaq\n"
                                "\tSource size: %d bits\n"
                                "\tTarget size: %d bits\n"
                                "\tCompressed: %d bits\n"
                                "\tComp. Ratio: %g bits\n\n",
                                dir->file_name[i],
                                dir->file_name[i],
                                one_z,
                                dst_z,
                                one_z - dst_z,
                                (double)((double)dst_z/(double)one_z)
                        );

                } else {
                        fprintf(stderr, "One of the file streams is NULL\n");
                        /* 
                         * Don't let the size_single array get fucked 
                         * by leaving a gap from the else { condition 
                         */
                        return;
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

                                fprintf(stderr,
                                        "SOURCE: %s(+)%s\n" 
                                        "TARGET: %s%s.zpaq\n"
                                        "\tSource size: %d bits\n"
                                        "\tTarget size: %d bits\n"
                                        "\tCompressed: %d bits\n"
                                        "\tComp. Ratio: %g bits\n\n",
                                        dir->file_name[i], dir->file_name[j],
                                        dir->file_name[i], dir->file_name[j],
                                        one_z + two_z,
                                        dst_z,
                                        (one_z+two_z) - dst_z,
                                        (double)((double)dst_z/(double)((double)one_z+(double)two_z))
                                );

                        } else {
                                fprintf(stderr, "One of the file streams is NULL\n");
                                /* 
                                 * Don't let the size_single array get fucked 
                                 * by leaving a gap from the else { condition 
                                 */
                                return;
                        }
                }
        }

        ncd_print(ncd, dir);
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

        for (i=0; i<dir->file_count; i++) {

                shell("zpaq a %s__ncd/%s.zpaq %s -method 56", 
                        dir->path, 
                        dir->file_name[i],
                        dir->file_path[i]
                );

                one = fopen(dir->file_path[i], "r+");
                dst = fopenf("r+", "%s__ncd/%s.zpaq", dir->path, dir->file_name[i]);

                if (one!=NULL && dst!=NULL) {

                        one_z = (int)file_length(one);
                        dst_z = (int)file_length(dst);

                        ncd->size_single[i] = dst_z;

                        fprintf(stderr,
                                "SOURCE: %s\n" 
                                "TARGET: %s.zpaq\n"
                                "\tSource size: %d bits\n"
                                "\tTarget size: %d bits\n"
                                "\tCompressed: %d bits\n"
                                "\tComp. Ratio: %g bits\n\n",
                                dir->file_name[i],
                                dir->file_name[i],
                                one_z,
                                dst_z,
                                one_z - dst_z,
                                (double)((double)dst_z/(double)one_z)
                        );

                } else {
                        fprintf(stderr, "One of the file streams is NULL\n");
                        /* 
                         * Don't let the size_single array get fucked 
                         * by leaving a gap from the else { condition 
                         */
                        return;
                }
        }

        for (i=0; i<dir->file_count; i++) {
                for (j=0; j<dir->file_count; j++) {

                        shell("zpaq a %s__ncd/%s%s.zpaq %s %s -method 56", 
                                dir->path, 
                                dir->file_name[i], dir->file_name[j],
                                dir->file_path[i], dir->file_path[j]
                        );

                        one = fopen(dir->file_path[i], "r+");
                        two = fopen(dir->file_path[j], "r+");

                        dst = fopenf("r+", "%s__ncd/%s%s.zpaq", 
                                dir->path, 
                                dir->file_name[i], 
                                dir->file_name[j]
                        );

                        if (one!=NULL && two!=NULL && dst!=NULL) {

                                one_z = (int)file_length(one);
                                two_z = (int)file_length(two);
                                dst_z = (int)file_length(dst);

                                ncd->size_double[i][j] = dst_z;

                                fprintf(stderr,
                                        "SOURCE: %s(+)%s\n" 
                                        "TARGET: %s%s.zpaq\n"
                                        "\tSource size: %d bits\n"
                                        "\tTarget size: %d bits\n"
                                        "\tCompressed: %d bits\n"
                                        "\tComp. Ratio: %g bits\n\n",
                                        dir->file_name[i], dir->file_name[j],
                                        dir->file_name[i], dir->file_name[j],
                                        (one_z+two_z),
                                        dst_z,
                                        (one_z+two_z) - dst_z,
                                        (double)((double)dst_z/(double)((double)one_z+(double)two_z))
                                );

                        } else {
                                fprintf(stderr, "One of the file streams is NULL\n");
                                /* 
                                 * Don't let the size_single array get fucked 
                                 * by leaving a gap from the else { condition 
                                 */
                                return;
                        }
                }
        }

        ncd_print(ncd, dir);
}

void just_ncd(struct directory_t *dir)
{
        int i;
        int j;

        FILE *dst;
        int dst_z;

        struct ncd_t *ncd = make_ncd(dir->file_count);

        for (i=0; i<dir->file_count; i++) {

                dst = fopenf("r+", "%s__ncd/%s.zpaq", dir->path, dir->file_name[i]);

                if (dst != NULL) {

                        dst_z = (int)file_length(dst);

                        ncd->size_single[i] = dst_z;

                } else {
                        fprintf(stderr, "One of the file streams is NULL\n");
                        /* 
                         * Don't let the size_single array get fucked 
                         * by leaving a gap from the else { condition 
                         */
                        return;
                }
        }

        for (i=0; i<dir->file_count; i++) {
                for (j=0; j<dir->file_count; j++) {

                        dst = fopenf("r+", "%s__ncd/%s%s.zpaq", dir->path, dir->file_name[i], dir->file_name[j]);

                        if (dst != NULL) {

                                dst_z = (int)file_length(dst);

                                ncd->size_double[i][j] = dst_z;

                        } else {
                                fprintf(stderr, "One of the file streams is NULL\n");
                                /* 
                                 * Don't let the size_single array get fucked 
                                 * by leaving a gap from the else { condition 
                                 */
                                return;
                        }
                }
        }

        ncd_print(ncd, dir);
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


