#include "io.h"
#include "pch.h"

IO_ReadFileError io_read_file(char const* filepath, char** data, usize* size) {
  FILE* file = fopen(filepath, "r");
  if (file == NULL) {
    *data = NULL;
    *size = 0;
    return IO_READ_FILE_NOT_FOUND;
  }

  fseek(file, 0, SEEK_END);
  *size = ftell(file);


  *data = (char*)malloc(*size + 1);

  fseek(file, 0, SEEK_SET);
  usize bytes_read = fread(*data, sizeof(char), *size, file);
  if (bytes_read < *size) {
    return IO_READ_FILE_ERROR_WHILE_READING;
  }

  return IO_READ_FILE_SUCCESS;
}

void io_free_file(char *data) {
  free((void*)data);
}

IO_WriteFileError io_write_binary(const char* filepath, void* data, usize size) {
  i32 file_descriptor = open(filepath, O_WRONLY | O_CREAT, 0777);
  if (file_descriptor == -1) {
    return IO_WRITE_FILE_COULD_NOT_OPEN;
  }

  FILE* file = fdopen(file_descriptor, "wb");
  if (file == NULL) {
    return IO_WRITE_FILE_COULD_NOT_OPEN;
  }

  usize bytes_written = fwrite(data, sizeof(char), size, file);
  if (bytes_written < size) {
    return IO_WRITE_FILE_ERROR_WHILE_WRITING;
  }
  fclose(file);
  close(file_descriptor);

  return IO_WRITE_FILE_SUCCESS;
}

#ifdef ZN_DEBUG
void io_print_assertion(
  const char* condition,
  const char* function,
  const char* file,
  i32 line,
  const char* message
) {
  puts("=== ASSERTION FAILED ===");
  printf("In %s:%d: %s():\n\t%s => %s\n\n", file, line, function, condition, message);
}
#endif

