#pragma once
#include "pch.h"
#include "util/common.h"

typedef enum {
  IO_READ_FILE_SUCCESS = 0,
  IO_READ_FILE_NOT_FOUND,
  IO_READ_FILE_ERROR_WHILE_READING
} IO_ReadFileError;

IO_ReadFileError io_read_file(char const* filepath, char** data, usize* size);

void io_free_file(char* data);

typedef enum {
  IO_WRITE_FILE_SUCCESS = 0,
  IO_WRITE_FILE_COULD_NOT_OPEN,
  IO_WRITE_FILE_ERROR_WHILE_WRITING,
} IO_WriteFileError;

IO_WriteFileError io_write_binary(const char* filepath, void* data, usize size);

#ifdef ZN_DEBUG
void io_print_assertion(
  const char* condition,
  const char* function,
  const char* file,
  i32 line,
  const char* message
);
#endif // ZN_DEBUG

