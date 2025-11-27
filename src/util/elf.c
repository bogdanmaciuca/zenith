// TODO:
// clean up
// find out why we need the headers in entry point
#include "elf.h"
#include "pch.h"
#include "common.h"
#include "debug.h"

u16 elf__get_type(ElfType type);

ElfCreateError elf_create(void** elf, const usize* size, const ElfData* const elf_data) {
  Elf64_Ehdr elf_header = {
    .e_ident = {
      ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3,      // Magic bytes
      ELFCLASS64,                              // 64bit
      ELFDATA2LSB,                             // Little endian,
      EV_CURRENT,                              // ELF version
      ELFOSABI_LINUX,                          // Target ABI
      0x00,                                    // Target ABI version
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // Padding
    },
    .e_type      = elf__get_type(elf_data->type),                      // Binary type
    .e_machine   = EM_X86_64,                                          // Architecture
    .e_version   = EV_CURRENT,                                         // File version
    .e_entry     = 0x400000 + sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr), // Virtual adress to which the system transfers control
    .e_phoff     = sizeof(Elf64_Ehdr),                                 // Program header table offset
    .e_shoff     = 0,                                                  // Section header table offset
    .e_flags     = 0,                                                  // Processor specific flags
    .e_ehsize    = sizeof(Elf64_Ehdr),                                 // ELF header size
    .e_phentsize = sizeof(Elf64_Phdr),                                 // Program header size
    .e_phnum     = 1,                                                  // Number of program headers
    .e_shentsize = sizeof(Elf64_Shdr),                                 // Section header entry size
    .e_shnum     = 0,                                                  // Number of entries in the section header table
    .e_shstrndx  = SHN_UNDEF                                           // Section header table index of the section name string table
  };

  Elf64_Phdr program_header = {
    .p_type = PT_LOAD,      // Type of segment
    .p_offset = 0,          // Offset of the segment relative to file
    .p_vaddr = 0x400000,    // Virtual address of the segment
    .p_paddr = 0x400000,    // Physical address
    .p_filesz = sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr) + code_size,
    .p_memsz = sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr) + code_size,
    .p_flags = PF_X | PF_R, // Flags for the segment
    .p_align = 8,
  };

 
  *size = sizeof(elf_header) + sizeof(program_header) + code_size;
  *elf = malloc(*size);
  memcpy(*elf,                                               &elf_header,     sizeof(elf_header));
  memcpy(*elf + sizeof(elf_header),                          &program_header, sizeof(program_header));
  memcpy(*elf + sizeof(elf_header) + sizeof(program_header), code,            code_size);

  return ELF_CREATE_SUCCESS;
}

void elf_free(void* elf) {
  free(elf);
}

u16 elf__get_type(ElfType type) {
  switch (type) {
    case ELF_EXECUTABLE: return ET_EXEC;
    case ELF_SHARED: return ET_DYN;
    case ELF_RELOCATABLE: return ET_REL;
    default: ASSERT(false, "Unreachable");
  }
}

