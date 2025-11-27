// TODO:
// clean up
// find out why we need the headers in entry point
#include "elf.h"
#include "pch.h"
#include "common.h"
#include "debug.h"

#define ELF__SEGMENT_NUM 2
#define ELF__SECTION_NUM 4

#define ELF__TEXT_SHDR_STR_IDX 0
#define ELF__DATA_SHDR_STR_IDX 1
#define ELF__STRTAB_SHDR_STR_IDX 2

#define ELF__EHDR_SIZE sizeof(Elf64_Ehdr)
#define ELF__PHDR_SIZE sizeof(Elf64_Phdr)
#define ELF__SHDR_SIZE sizeof(Elf64_Shdr)

const char* elf__strtab_strings = "\0.text\0.data\0.strtab\0";
i32 elf__strtab_length = 24;

u16 elf__get_type(ElfType type);

typedef struct {
  void* elf;
  usize size;
  usize text_size;
  usize data_size;
} ElfBuilder;

void        elf_builder_create(ElfBuilder* builder, const ElfData* const elf_data);
void        elf_builder_free(ElfBuilder* builder);
Elf64_Ehdr* elf_builder_ehdr(const ElfBuilder* const builder);
Elf64_Phdr* elf_builder_phdr(const ElfBuilder* const builder);
Elf64_Shdr* elf_builder_shdr(const ElfBuilder* const builder);
void*       elf_builder_text(const ElfBuilder* const builder);
void*       elf_builder_data(const ElfBuilder* const builder);
const char* elf_builder_strtab(const ElfBuilder* const builder);
usize       elf_builder_phdr_offset(const ElfBuilder* const builder);
usize       elf_builder_text_offset(const ElfBuilder* const builder);
usize       elf_builder_data_offset(const ElfBuilder* const builder);
usize       elf_builder_strtab_offset(const ElfBuilder* const builder);
usize       elf_builder_shdr_offset(const ElfBuilder* const builder);

ElfCreateError elf_create(void** elf, usize* const elf_size, const ElfData* const elf_data) {
  ElfBuilder builder;
  elf_builder_create(&builder, elf_data);

  // ELF header
  Elf64_Ehdr* ehdr = elf_builder_ehdr(&builder);
  ehdr->e_ident[EI_MAG0]    = ELFMAG0;
  ehdr->e_ident[EI_MAG1]    = ELFMAG1;
  ehdr->e_ident[EI_MAG2]    = ELFMAG2;
  ehdr->e_ident[EI_MAG3]    = ELFMAG3;
  ehdr->e_ident[EI_CLASS]   = ELFCLASS64;
  ehdr->e_ident[EI_DATA]    = ELFDATA2LSB;
  ehdr->e_ident[EI_VERSION] = EV_CURRENT;
  ehdr->e_ident[EI_OSABI]   = ELFOSABI_LINUX;
  ehdr->e_type      = elf__get_type(elf_data->type);                 // Binary type
  ehdr->e_machine   = EM_X86_64;                                     // Architecture
  ehdr->e_version   = EV_CURRENT;                                    // File version
  ehdr->e_entry     = 0x400000 + elf_builder_text_offset(&builder);  // Virtual adress to which the system transfers control
  ehdr->e_phoff     = elf_builder_phdr_offset(&builder);             // Program header table offset
  ehdr->e_shoff     = elf_builder_shdr_offset(&builder);             // Section header table offset
  ehdr->e_flags     = 0;                                             // Processor specific flags
  ehdr->e_ehsize    = ELF__EHDR_SIZE;                                // ELF header size
  ehdr->e_phentsize = ELF__PHDR_SIZE;                                // Program header size
  ehdr->e_phnum     = ELF__SEGMENT_NUM;                              // Number of program headers
  ehdr->e_shentsize = ELF__SHDR_SIZE;                                // Section header entry size
  ehdr->e_shnum     = ELF__SECTION_NUM;                              // Number of entries in the section header table
  ehdr->e_shstrndx  = ELF__SECTION_NUM - 1;                          // Section header table index of the section name string table

  // Text segment header
  Elf64_Phdr* phdr_text = &elf_builder_phdr(&builder)[0];
  phdr_text->p_type   = PT_LOAD;
  phdr_text->p_offset = 0;
  phdr_text->p_vaddr  = 0x400000;
  phdr_text->p_paddr  = 0x400000;
  phdr_text->p_filesz = elf_builder_text_offset(&builder) + builder.text_size;
  phdr_text->p_memsz  = elf_builder_text_offset(&builder) + builder.text_size;
  phdr_text->p_flags  = PF_R | PF_X;
  phdr_text->p_align  = 0x1000;

  // Data segment header
  Elf64_Phdr* phdr_data = &elf_builder_phdr(&builder)[1];
  phdr_data->p_type   = PT_LOAD;
  phdr_data->p_offset = elf_builder_data_offset(&builder);
  phdr_data->p_vaddr  = 0x400000 + elf_builder_data_offset(&builder);
  phdr_data->p_paddr  = 0x400000 + elf_builder_data_offset(&builder);
  phdr_data->p_filesz = builder.data_size;
  phdr_data->p_memsz  = builder.data_size;
  phdr_data->p_flags  = PF_R | PF_W;
  phdr_data->p_align  = 0x1000;

  // .text
  memcpy(elf_builder_text(&builder), elf_data->text, elf_data->text_size);
  // .data
  memcpy(elf_builder_data(&builder), elf_data->data, elf_data->data_size);
  // .strtab
  memcpy((void*)elf_builder_strtab(&builder), elf__strtab_strings, elf__strtab_length);

  // Null section header
  Elf64_Shdr* null_shdr = &elf_builder_shdr(&builder)[0];
  memset(null_shdr, 0, ELF__SHDR_SIZE);

  // Text section header
  Elf64_Shdr* text_shdr = &elf_builder_shdr(&builder)[1];
  text_shdr->sh_name      = ELF__TEXT_SHDR_STR_IDX;
  text_shdr->sh_type      = SHT_PROGBITS;
  text_shdr->sh_flags     = SHF_ALLOC | SHF_EXECINSTR;
  text_shdr->sh_addr      = 0x400000 + elf_builder_text_offset(&builder);
  text_shdr->sh_offset    = elf_builder_text_offset(&builder);
  text_shdr->sh_size      = builder.text_size;
  text_shdr->sh_link      = 0;
  text_shdr->sh_info      = 0;
  text_shdr->sh_addralign = 0x10;
  text_shdr->sh_entsize   = 0;

  // Data section header
  Elf64_Shdr* data_shdr = &elf_builder_shdr(&builder)[2];
  data_shdr->sh_name      = ELF__DATA_SHDR_STR_IDX;
  data_shdr->sh_type      = SHT_PROGBITS;
  data_shdr->sh_flags     = SHF_ALLOC | SHF_WRITE;
  data_shdr->sh_addr      = 0x400000 + elf_builder_data_offset(&builder);
  data_shdr->sh_offset    = elf_builder_data_offset(&builder);
  data_shdr->sh_size      = builder.data_size;
  data_shdr->sh_link      = 0;
  data_shdr->sh_info      = 0;
  data_shdr->sh_addralign = 0x10;
  data_shdr->sh_entsize   = 0;

  // Strtab section header
  Elf64_Shdr* strtab_shdr = &elf_builder_shdr(&builder)[3];
  strtab_shdr->sh_name      = ELF__STRTAB_SHDR_STR_IDX;
  strtab_shdr->sh_type      = SHT_STRTAB;
  strtab_shdr->sh_flags     = SHF_ALLOC;
  strtab_shdr->sh_addr      = 0x400000 + elf_builder_strtab_offset(&builder);
  strtab_shdr->sh_offset    = elf_builder_strtab_offset(&builder);
  strtab_shdr->sh_size      = elf__strtab_length;
  strtab_shdr->sh_link      = 0;
  strtab_shdr->sh_info      = 0;
  strtab_shdr->sh_addralign = 0x10;
  strtab_shdr->sh_entsize   = 0;

  *elf      = builder.elf;
  *elf_size = builder.size;

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

usize elf__calculate_data_offset(usize code_size) {
  usize headers_size = sizeof(Elf64_Ehdr) + (ELF__SEGMENT_NUM * sizeof(Elf64_Phdr));
  usize text_segment_end = headers_size + code_size;

  return ALIGN_UP(text_segment_end, 0x1000);
}

void elf_builder_create(ElfBuilder* builder, const ElfData* const elf_data) {
  builder->size =
    sizeof(Elf64_Ehdr) +                          // ELF header
    sizeof(Elf64_Phdr) * ELF__SEGMENT_NUM +       // Program headers
    ALIGN_UP(elf_data->text_size, 0x1000) +       // .text
    elf_data->data_size +                         // .data
    elf__strtab_length +                          // Strings for the section names
    ELF__SHDR_SIZE * ELF__SECTION_NUM;            // Section headers

  builder->elf = malloc(builder->size);

  builder->data_size = elf_data->data_size;
  builder->text_size = elf_data->text_size;
}

void elf_builder_free(ElfBuilder* builder) {
  free(builder->elf);
}

Elf64_Ehdr* elf_builder_ehdr(const ElfBuilder* const builder) {
  return builder->elf;
}

Elf64_Phdr* elf_builder_phdr(const ElfBuilder* const builder) {
  return builder->elf + ELF__EHDR_SIZE;
}

void* elf_builder_data(const ElfBuilder* const builder) {
  return builder->elf + ELF__EHDR_SIZE + ELF__PHDR_SIZE * ELF__SEGMENT_NUM +
  ALIGN_UP(builder->text_size, 0x1000);
}

const char* elf_builder_strtab(const ElfBuilder* const builder) {
  return builder->elf + ELF__EHDR_SIZE + ELF__PHDR_SIZE * ELF__SEGMENT_NUM +
  builder->text_size + ALIGN_UP(builder->data_size, 0x1000);
}

void* elf_builder_text(const ElfBuilder* const builder) {
  return builder->elf + ELF__EHDR_SIZE + ELF__PHDR_SIZE * ELF__SEGMENT_NUM;
}

Elf64_Shdr* elf_builder_shdr(const ElfBuilder* const builder) {
  return builder->elf + ELF__EHDR_SIZE + ELF__PHDR_SIZE * ELF__SEGMENT_NUM +
  ALIGN_UP(builder->text_size, 0x1000) + builder->data_size + elf__strtab_length;
}

usize elf_builder_phdr_offset(const ElfBuilder* const builder) {
  return sizeof(Elf64_Ehdr);
  ((void)builder); // Unused builder warning
}

usize elf_builder_text_offset(const ElfBuilder* const builder) {
  return ELF__EHDR_SIZE + ELF__PHDR_SIZE * ELF__SEGMENT_NUM;
  ((void)builder); // Unused builder warning
}

usize elf_builder_data_offset(const ElfBuilder* const builder) {
  usize offset = ELF__EHDR_SIZE + ELF__PHDR_SIZE * ELF__SEGMENT_NUM + builder->text_size;
  return ALIGN_UP(offset, 0x1000);
}

usize elf_builder_strtab_offset(const ElfBuilder* const builder) {
  return ELF__EHDR_SIZE + ELF__PHDR_SIZE * ELF__SEGMENT_NUM +
  ALIGN_UP(builder->text_size, 0x1000) + builder->data_size;
}

usize elf_builder_shdr_offset(const ElfBuilder* const builder) {
  return ELF__EHDR_SIZE + ELF__PHDR_SIZE * ELF__SEGMENT_NUM +
  ALIGN_UP(builder->text_size, 0x1000) + builder->data_size + elf__strtab_length;
}
