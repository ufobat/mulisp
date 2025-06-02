// line_reader.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zos_vfs.h"

#define LINE_BUFFER_SIZE 256 // Adjust as needed

char *fgets(char *s, int n, zos_dev_t stream) {
    static char buffer[LINE_BUFFER_SIZE];
    static int buffer_pos = 0;
    int i = 0;
    uint16_t bytes_read;
    zos_err_t err;

    if (s == NULL || n <= 0 || stream < 0) {
        return NULL; // Handle invalid arguments
    }

    while (i < n - 1) { // Leave space for null terminator
        if (buffer_pos >= LINE_BUFFER_SIZE) {
            buffer_pos = 0; // Reset buffer
        }

        bytes_read = 1;
        err = read(stream, buffer + buffer_pos, &bytes_read);

        if (err != ERR_SUCCESS || bytes_read == 0) {
            if (i > 0) {
                s[i] = '\0';
                return s;
            }
            return NULL;
        }

        s[i++] = buffer[buffer_pos];

        if (buffer[buffer_pos] == '\n') {
            s[i] = '\0';
            buffer_pos++;
            return s;
        }

        buffer_pos++;
    }

    s[i] = '\0'; // Reached max length
    return s;
}
