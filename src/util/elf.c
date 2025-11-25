#include "elf.h"
#include "common.h"
#include "debug.h"

u16 elf__get_type(ElfType type);

ElfCreateError elf_create(ElfType type) {
  Elf64_Ehdr elf = {
    .e_ident = {
      ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3,      // Magic bytes
      ELFCLASS64,                              // 64bit
      ELFDATA2LSB,                             // Little endian,
      EV_CURRENT,                              // ELF version
      ELFOSABI_LINUX,                          // Target ABI
      0x00,                                    // Target ABI version
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // Padding
    },
    .e_type = elf__get_type(type),             // Binary type
    .e_machine = EM_X86_64,                    // Architecture
    .e_version = EV_CURRENT,                   // File version
    .e_entry = 0x400000,                       // Virtual adress to which the system transfers control
    .e_phoff = 0,                              // Program header table offset
    .e_shoff = 0,                              // Section header table offset
    .e_flags = 0,                              // Processor specific flags
    .e_ehsize = sizeof(Elf64_Ehdr),            //
    .e_phentsize = 56,
    .e_phnum = 0,
    .e_shentsize = 0,
    .e_shnum = 0,
    .e_shstrndx = SHN_UNDEF
  };

  return ELF_CREATE_SUCCESS;
}

u16 elf__get_type(ElfType type) {
  switch (type) {
    case ELF_EXECUTABLE: return ET_EXEC;
    case ELF_SHARED: return ET_DYN;
    case ELF_RELOCATABLE: return ET_REL;
    default: ASSERT(false, "Unreachable");
  }
}

