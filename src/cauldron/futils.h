// futils.h - File Utilities
// generic place to store standard helper functions for various file operations

#pragma once

#include "tx_types.h"

enum tx_result read_file_to_buffer(const char* filename, char** buffer, size_t* len);
