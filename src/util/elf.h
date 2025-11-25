#pragma once

typedef enum {
  ELF_CREATE_SUCCESS,
} ElfCreateError;

typedef enum {
  ELF_EXECUTABLE,
  ELF_SHARED,
  ELF_RELOCATABLE,
} ElfType;

ElfCreateError elf_create(ElfType type);

void elf_free();

