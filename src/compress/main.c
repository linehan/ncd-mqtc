#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "filesystem.h"
#include "ncd.h"
#include "module/mod_zlib.h"
#include "module/mod_bzlib.h"


typedef int (*cmpr1_t)(FILE*, FILE*);
typedef int (*cmpr2_t)(FILE*, FILE*, FILE*);


void run_compression(struct directory_t *dir, cmpr1_t __cmpr1, cmpr2_t __cmpr2, const char *ext)
{
        int i;
        int j;

        FILE *one;
        FILE *two;
        FILE *dst;

        int one_z;
        int two_z;
        int dst_z;

        struct ncd_t *ncd = ncd_create(dir->file_count);

        mkdirf(S_IRWXU|S_IRWXG|S_IRWXO, "%s__ncd", dir->path);

        for (i=0; i<dir->file_count; i++) {

                one = fopen(dir->file_path[i], "r+");
                dst = fopenf("w+", "%s__ncd/%s.%s", dir->path, dir->file_name[i], ext);

                if (one != NULL && dst != NULL) {
                        __cmpr1(one, dst);

                        one_z = (int)file_length(one);
                        dst_z = (int)file_length(dst);

                        ncd->size_single[i] = dst_z;

                        fprintf(stdout,
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
                        fprintf(stdout, "One of the file streams is NULL\n");
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

                                fprintf(stdout,
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
                                fprintf(stdout, "One of the file streams is NULL\n");
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


void shell_compression(struct directory_t *dir)
{
        int i;
        int j;

        FILE *one;
        FILE *two;
        FILE *dst;

        int one_z;
        int two_z;
        int dst_z;

        struct ncd_t *ncd = ncd_create(dir->file_count);

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

                        fprintf(stdout,
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
                        fprintf(stdout, "One of the file streams is NULL\n");
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

                                fprintf(stdout,
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
                                fprintf(stdout, "One of the file streams is NULL\n");
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


void shell_calculate_ncd(struct directory_t *dir)
{
        int i;
        int j;

        FILE *dst;
        int dst_z;

        struct ncd_t *ncd = ncd_create(dir->file_count);

        for (i=0; i<dir->file_count; i++) {

                dst = fopenf("r+", "%s__ncd/%s.zpaq", dir->path, dir->file_name[i]);

                if (dst != NULL) {

                        dst_z = (int)file_length(dst);

                        ncd->size_single[i] = dst_z;

                } else {
                        fprintf(stdout, "One of the file streams is NULL\n");
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
                                fprintf(stdout, "One of the file streams is NULL\n");
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
                        /* Compress and calculate NCD using zlib */
                        run_compression(&dir, zlib_compress, zlib_compress_cat, "gz");
                } else if (!strcmp(argv[1], "--bzlib")) {
                        /* Compress and calculate NCD using bzlib2 */
                        run_compression(&dir, bzlib_compress, bzlib_compress_cat, "bz2");
                } else if (!strcmp(argv[1], "--zpaq")) {
                        /* Compress and calculate NCD using zpaq */
                        shell_compression(&dir);
                } else if (!strcmp(argv[1], "--zpaqncd")) {
                        /* Calculate NCD using zpaq files */
                        shell_calculate_ncd(&dir);
                } else {
                        printf("%s? I don't know that one.\n", argv[1]);
                }
        } else {
                printf("Usage: %s --zlib|--bzlib|--zpaq|--zpaqncd <DIRECTORY>\n", argv[0]);
        }

        return 0;

}


