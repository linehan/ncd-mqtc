#include <stdio.h>
#include <string.h>
#include <bzlib.h>

#define BZLIB_BLOCK 16384


/**
 * bzlib_compress()
 * ````````````````
 * Compress a source file to a target file using bzlib 
 *
 * @src  : Source file (decompressed)
 * @dst  : Target file (compressed)
 * Return: 1 (success) or -1 (error)
 */
int bzlib_compress(FILE *src, FILE *dst)
{
        BZFILE*      bzdst;
        char         in[BZLIB_BLOCK];
        int          err;
        ssize_t      bytes_read = 0;
        unsigned int bytes_in   = 0;
        unsigned int bytes_out  = 0;

        if (src == NULL && dst == NULL) {
                return -1;
        }

        bzdst = BZ2_bzWriteOpen(&err, dst, 9, 0, 30);
        /*                                /   |   \
         *                         blocksize  |    work factor
         *                                verbosity
         *
         * block_size: [1-9]   9=best, block size is 100,000*block_size.
         * verbosity : [0-4]   0=silent, 1-4 increase debug verbosity
         * workfactor: [0-250] default=30, 0=30, amount of effort before
         *                     changing sorting algorithm.
         */

        if (err != BZ_OK) {
                BZ2_bzWriteClose(&err, bzdst, 1, &bytes_in, &bytes_out);
                /*                           /       |           \
                 *                        abandon   input size   output size
                 *                                   
                 * abandon  : 0=compress and write to target, 1=don't compress
                 * bytes_in : length before compression
                 * bytes_out: length after compression
                 */
                fprintf(stderr, "bzlib_compress: got error %d.\n", err);
                return -1;
        }

        while ((bytes_read = fread(in, 1, BZLIB_BLOCK, src)) > 0) {
                BZ2_bzWrite(&err, bzdst, in, bytes_read);
                if (err == BZ_IO_ERROR) {
                        BZ2_bzWriteClose(&err, bzdst, 1, &bytes_in, &bytes_out);
                        fprintf(stderr, "bzlib_compress: got error %d.\n", err);
                        return -1;
                }
        } 

        BZ2_bzWriteClose(&err, bzdst, 0, &bytes_in, &bytes_out);

        if (err == BZ_IO_ERROR) {
                fprintf(stderr, "bzlib_compress: got BZ_IO_ERROR %d.\n", err);
                return -1;
        }

        return 1;
}


/**
 * bzlib_compress_cat()
 * ````````````````````
 * Compress two source files to a target file using bzlib 
 *
 * @one  : Source file one (decompressed)
 * @two  : Source file two (decompressed)
 * @dst  : Target file (compressed)
 * Return: 1 (success) or -1 (error)
 */
int bzlib_compress_cat(FILE *one, FILE *two, FILE *dst)
{
        BZFILE*      bzdst;
        char         in[BZLIB_BLOCK];
        int          err;
        ssize_t      bytes_read = 0;
        unsigned int bytes_in   = 0;
        unsigned int bytes_out  = 0;

        if (one == NULL && two == NULL && dst == NULL) {
                return -1;
        }

        bzdst = BZ2_bzWriteOpen(&err, dst, 9, 0, 30);
        /*                                /   |   \
         *                         blocksize  |    work factor
         *                                verbosity
         *
         * block_size: [1-9]   9=best, block size is 100,000*block_size.
         * verbosity : [0-4]   0=silent, 1-4 increase debug verbosity
         * workfactor: [0-250] default=30, 0=30, amount of effort before
         *                     changing sorting algorithm.
         */

        if (err != BZ_OK) {
                BZ2_bzWriteClose(&err, bzdst, 1, &bytes_in, &bytes_out);
                /*                           /       |           \
                 *                        abandon   input size   output size
                 *                                   
                 * abandon  : 0=compress and write to target, 1=don't compress
                 * bytes_in : length before compression
                 * bytes_out: length after compression
                 */
                fprintf(stderr, "bzlib_compress_cat: got error %d.\n", err);
                return -1;
        }

        while ((bytes_read = fread(in, 1, BZLIB_BLOCK, one)) > 0) {
                BZ2_bzWrite(&err, bzdst, in, bytes_read);
                if (err == BZ_IO_ERROR) {
                        BZ2_bzWriteClose(&err, bzdst, 1, &bytes_in, &bytes_out);
                        fprintf(stderr, "bzlib_compress_cat: got error %d.\n", err);
                        return -1;
                }
        } 

        while ((bytes_read = fread(in, 1, BZLIB_BLOCK, two)) > 0) {
                BZ2_bzWrite(&err, bzdst, in, bytes_read);
                if (err == BZ_IO_ERROR) {
                        BZ2_bzWriteClose(&err, bzdst, 1, &bytes_in, &bytes_out);
                        fprintf(stderr, "bzlib_compress_cat: got error %d.\n", err);
                        return -1;
                }
        } 

        BZ2_bzWriteClose(&err, bzdst, 0, &bytes_in, &bytes_out);

        if (err == BZ_IO_ERROR) {
                return -1;
        }

        return 1;
}


/**
 * bzlib_decompress()
 * ``````````````````
 * Decompress a source file to a target file using bzlib 
 *
 * @src  : Source file (compressed)
 * @dst  : Target file (decompressed) 
 * Return: 1 (success) or -1 (error)
 */
int bzlib_decompress(FILE *src, FILE *dst)
{
        BZFILE* bzsrc;
        char    out[BZLIB_BLOCK];
        int     err;
        ssize_t bytes_read    = 0;

        if (src == NULL && dst == NULL) {
                return -1;
        }

        bzsrc = BZ2_bzReadOpen(&err, src, 0, 1, NULL, 0);
        /*                               /   |    |    \
         *                         verbosity |  unused  nUnused
         *                                 small 
         *
         * verbosity : [0-4]   0=silent, 1-4 increase debug verbosity
         * small     : [0-1] 0=normal, 1=use limited memory
         * unused    : Pointer to memory region 
         * nUnused   : # bytes to decompress starting at @unused, before
         *             decompressing @src.
         */

        if (err != BZ_OK) {
                BZ2_bzReadClose(&err, bzsrc);
                fprintf(stderr, "bzlib_decompress: got error %d.\n", err);
                return -1;
        }

        err = BZ_OK;

        while (err == BZ_OK) {
                bytes_read = BZ2_bzRead(&err, bzsrc, out, BZLIB_BLOCK);
                if (err == BZ_OK) {
                        fwrite(out, 1, bytes_read, dst);
                }
        }

        if (err != BZ_STREAM_END) {
                BZ2_bzReadClose(&err, bzsrc);
                fprintf(stderr, "bzlib_decompress: got error %d.\n", err);
                return -1;
        } else {
                BZ2_bzReadClose(&err, bzsrc);
        }

        return 1;
}

