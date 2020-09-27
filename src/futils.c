#include "futils.h"

#include <stdio.h>
#include <stdlib.h>

enum tx_result read_file_to_buffer(const char* filename, char** buffer, size_t* len)
{
    if (!(buffer && len)) {
        return TX_INVALID_PARAMTER;
    }

    enum tx_result result = TX_INVALID;
    size_t string_size = 0, read_size = 0;
    FILE* file = fopen(filename, "r");

    if (file) {
        fseek(file, 0, SEEK_END);
        long tell = ftell(file);

        if (tell < 0) {
            result = TX_FILE_ERROR;
            goto exit;
        }

        string_size = (size_t)tell;

        rewind(file);
        char* mem = (char*)malloc(string_size + 1);
        if (mem == NULL) {
            result = TX_ALLOCATION_ERROR;
            goto exit;
        }

        read_size = fread(mem, sizeof(char), string_size, file);
        mem[read_size] = '\0';

        *buffer = mem;
        *len = read_size;

        return TX_SUCCESS;
    } else {
        return TX_FILE_ERROR;
    }

exit:
    fclose(file);
    return result;
}
