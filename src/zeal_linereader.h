// line_reader.h
#ifndef LINE_READER_H
#define LINE_READER_H

#include "zos_vfs.h"
#include <stdio.h> // For char* and zos_dev_t

char *fgets(char *s, int n, zos_dev_t stream);

#endif
