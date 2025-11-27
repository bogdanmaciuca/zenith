#pragma once
#include "common.h"

typedef enum {
  ELF_CREATE_SUCCESS,
} ElfCreateError;

typedef enum {
  ELF_EXECUTABLE,
  ELF_SHARED,
  ELF_RELOCATABLE,
} ElfType;

typedef struct {
  ElfType type;
  void*   text;
  usize   text_size;
  void*   data;
  usize   data_size;
} ElfData;

ElfCreateError elf_create(void** elf, const usize* size, const ElfData* const elf_data);

void elf_free(void* elf);

