#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <zlib.h>

#define SET_BINARY_MODE(file)
#define ZLIB_BLOCK 16384

#define ZLIB_COMPRESSION_LEVEL Z_BEST_COMPRESSION

/**
 * zlib_compress()
 * ```````````````
 * Compress a source file to a target file using zlib
 *
 * @src  : Source file (decompressed)
 * @dst  : Target file (compressed)
 * Return: 
 *      Z_OK            success 
 *      Z_MEM_ERROR     memory could not be allocated
 *      Z_STREAM_ERROR  invalid compression level
 *      Z_VERSION_ERROR linked library and zlib.h don't match
 *      Z_ERRNO         error reading or writing the files
 */
int zlib_compress(FILE *src, FILE *dst)
{
        int status;
        int flush;
        unsigned have;
        z_stream stream;
        unsigned char in[ZLIB_BLOCK];
        unsigned char out[ZLIB_BLOCK];

        /* Allocate the compressor stream state */
        stream.zalloc  = Z_NULL;
        stream.zfree   = Z_NULL;
        stream.opaque  = Z_NULL;
        status         = deflateInit(&stream, ZLIB_COMPRESSION_LEVEL);

        if (status != Z_OK) {
                return status;
        }

        /* Compress until EOF is reached (flush == Z_FINISH) */
        do {
                stream.avail_in = fread(in, 1, ZLIB_BLOCK, src);

                if (ferror(src)) {
                        deflateEnd(&stream);
                        return Z_ERRNO;
                }

                flush = feof(src) ? Z_FINISH : Z_NO_FLUSH;
                stream.next_in = in;

                /* 
                 * Run deflate() on input until the output
                 * buffer is no longer full. Terminate if
                 * all of the source has been read.
                 */
                do {
                        stream.avail_out = ZLIB_BLOCK;
                        stream.next_out  = out;

                        status = deflate(&stream, flush);

                        /* State not clobbered */
                        assert(status != Z_STREAM_ERROR);  

                        have = ZLIB_BLOCK - stream.avail_out;

                        if (fwrite(out, 1, have, dst) != have || ferror(dst)) {
                                deflateEnd(&stream);
                                return Z_ERRNO;
                        }

                } while (stream.avail_out == 0);

                /* All input should be used */
                assert(stream.avail_in == 0);     

        } while (flush != Z_FINISH);

        assert(status == Z_STREAM_END);

        /* Clean up */

        deflateEnd(&stream);

        return Z_OK;
}


/**
 * zlib_compress_cat()
 * ```````````````````
 * Compress two source files to a target file using zlib
 *
 * @one  : Source file one (decompressed)
 * @two  : Source file two (decompressed)
 * @dst  : Target file     (compressed)
 * Return: 
 *      Z_OK            success 
 *      Z_MEM_ERROR     memory could not be allocated
 *      Z_STREAM_ERROR  invalid compression level
 *      Z_VERSION_ERROR linked library and zlib.h don't match
 *      Z_ERRNO         error reading or writing the files
 */
int zlib_compress_cat(FILE *one, FILE *two, FILE *dst)
{
        int status;
        int flush;
        unsigned have;
        z_stream stream;
        unsigned char in[ZLIB_BLOCK];
        unsigned char out[ZLIB_BLOCK];

        /* Allocate the compressor stream state */
        stream.zalloc  = Z_NULL;
        stream.zfree   = Z_NULL;
        stream.opaque  = Z_NULL;
        status         = deflateInit(&stream, ZLIB_COMPRESSION_LEVEL);

        if (status != Z_OK) {
                return status;
        }

        /* Compress file 'one' until EOF is reached (flush == Z_FINISH) */
        do {
                stream.avail_in = fread(in, 1, ZLIB_BLOCK, one);

                if (ferror(one)) {
                        deflateEnd(&stream);
                        return Z_ERRNO;
                }

                flush = feof(one) ? Z_FINISH : Z_NO_FLUSH;

                stream.next_in = in;

                /* 
                 * Run deflate() on input until the output
                 * buffer is no longer full. Terminate if
                 * all of the source has been read.
                 */
                do {
                        stream.avail_out = ZLIB_BLOCK;
                        stream.next_out  = out;

                        /* Lie about flush, don't flush until 'two' */ 
                        /*status = deflate(&stream, flush);*/
                        status = deflate(&stream, Z_NO_FLUSH);

                        /* State not clobbered */
                        assert(status != Z_STREAM_ERROR);  

                        have = ZLIB_BLOCK - stream.avail_out;

                        if (fwrite(out, 1, have, dst) != have || ferror(dst)) {
                                deflateEnd(&stream);
                                return Z_ERRNO;
                        }

                } while (stream.avail_out == 0);

                /* All input should be used */
                assert(stream.avail_in == 0);     

        } while (flush != Z_FINISH);


        /* Compress file 'two' until EOF is reached (flush == Z_FINISH) */
        do {
                stream.avail_in = fread(in, 1, ZLIB_BLOCK, two);

                if (ferror(two)) {
                        deflateEnd(&stream);
                        return Z_ERRNO;
                }

                flush = feof(two) ? Z_FINISH : Z_NO_FLUSH;
                stream.next_in = in;

                /* 
                 * Run deflate() on input until the output
                 * buffer is no longer full. Terminate if
                 * all of the source has been read.
                 */
                do {
                        stream.avail_out = ZLIB_BLOCK;
                        stream.next_out  = out;

                        status = deflate(&stream, flush);

                        /* State not clobbered */
                        assert(status != Z_STREAM_ERROR);  

                        have = ZLIB_BLOCK - stream.avail_out;

                        if (fwrite(out, 1, have, dst) != have || ferror(dst)) {
                                deflateEnd(&stream);
                                return Z_ERRNO;
                        }

                } while (stream.avail_out == 0);

                /* All input should be used */
                assert(stream.avail_in == 0);     

        } while (flush != Z_FINISH);

        assert(status == Z_STREAM_END);

        /* Clean up */

        deflateEnd(&stream);

        return Z_OK;
}



/**
 * zlib_decompress()
 * `````````````````
 * Decompress a source file to a target file using zlib
 *
 * @src  : Source file (compressed)
 * @dst  : Target file (decompressed) 
 * Return: 
 *      Z_OK            success 
 *      Z_MEM_ERROR     memory could not be allocated
 *      Z_DATA_ERROR    compressed data is invalid or incomplete
 *      Z_VERSION_ERROR linked library and zlib.h don't match
 *      Z_ERRNO         error reading or writing the files
 *
 */
int zlib_decompress(FILE *src, FILE *dst)
{
        int status;
        unsigned have;
        z_stream stream;
        unsigned char in[ZLIB_BLOCK];
        unsigned char out[ZLIB_BLOCK];

        /* Allocate the inflate state */
        stream.zalloc   = Z_NULL;
        stream.zfree    = Z_NULL;
        stream.opaque   = Z_NULL;
        stream.avail_in = 0;
        stream.next_in  = Z_NULL;
        status          = inflateInit(&stream);
        
        if (status != Z_OK) {
                return status;
        }

        /* Decompress until EOF (status == Z_STREAM_END) */
        do {
                stream.avail_in = fread(in, 1, ZLIB_BLOCK, src);

                if (ferror(src)) {
                        inflateEnd(&stream);
                        return Z_ERRNO;
                }

                if (stream.avail_in == 0) { 
                        break;
                }

                stream.next_in = in;

                /* 
                 * Run inflate() on input until the output
                 * buffer is no longer full. Terminate if
                 * all of the source has been read.
                 */
                do {
                        stream.avail_out = ZLIB_BLOCK;
                        stream.next_out  = out;

                        status = inflate(&stream, Z_NO_FLUSH);

                        /* State not clobbered */
                        assert(status != Z_STREAM_ERROR);  

                        switch (status) {
                        case Z_NEED_DICT:
                                status = Z_DATA_ERROR;
                                /* fallthrough */
                        case Z_DATA_ERROR:
                        case Z_MEM_ERROR:
                                inflateEnd(&stream);
                                return status;
                        }

                        have = ZLIB_BLOCK - stream.avail_out;

                        if (fwrite(out, 1, have, dst) != have || ferror(dst)) {
                                inflateEnd(&stream);
                                return Z_ERRNO;
                        }

                } while (stream.avail_out == 0);

        } while (status != Z_STREAM_END);

        /* Clean up */
        inflateEnd(&stream);
        
        return (status == Z_STREAM_END) ? Z_OK : Z_DATA_ERROR;
}


/**
 * zlib_strerror()
 * ```````````````
 * Report a zlib error from the return value.
 *
 * @status: Status value from zlib_compress/zlib_decompress
 * Return : nothing
 **/
void zlib_strerror(int status)
{
        fputs("zpipe: ", stderr);

        switch (status) {
        case Z_ERRNO:
                if (ferror(stdin)) {
                        fputs("error reading stdin\n", stderr);
                }
                if (ferror(stdout)) {
                        fputs("error writing stdout\n", stderr);
                }
                break;
        case Z_STREAM_ERROR:
                fputs("invalid compression level\n", stderr);
                break;
        case Z_DATA_ERROR:
                fputs("invalid or incomplete deflate data\n", stderr);
                break;
        case Z_MEM_ERROR:
                fputs("out of memory\n", stderr);
                break;
        case Z_VERSION_ERROR:
                fputs("zlib version mismatch!\n", stderr);
        }
}

