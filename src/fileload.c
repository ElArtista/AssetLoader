#include "assets/fileload.h"
#include <stdio.h>
#include <assets/abstractfs.h>

static long stdio_filesize(const char* filepath)
{
    /* Filesize in bytes */
    long int size = -1;

    /* Try open the file */
    FILE* f = 0;
    f = fopen(filepath, "rb");
    if (!f)
        return -1;

    /* Seek to its end */
    fseek(f, 0, SEEK_END);

    /* Tell the size of the current pos */
    size = ftell(f);

    /* Close the file handle and return size */
    fclose(f);
    return size;
}

static int stdio_read_file_to_mem(const char* filename, unsigned char* buf, size_t buf_sz)
{
    /* Try open the file */
    FILE* f = 0;
    f = fopen(filename, "rb");
    if (!f)
        return 0;

    /* Read the file in single IO operation */
    fread(buf, 1, buf_sz, f);

    /* Close the file handle and return success code */
    fclose(f);
    return 1;
}

long filesize(const char* fname)
{
    if (afs_initialized())
        return afs_file_length(fname);
    else
        return stdio_filesize(fname);
}

int read_file_to_mem(const char* fname, unsigned char* buf, size_t buf_sz)
{
    if (afs_initialized())
        return afs_read_file_to_mem(fname, buf, buf_sz);
    else
        return stdio_read_file_to_mem(fname, buf, buf_sz);
}
