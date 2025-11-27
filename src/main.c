#include "util/io.h"
#include "util/elf.h"
#include "util/common.h"
#include "util/debug.h"

int main() {
  void* elf;
  usize elf_size;

  char data[] = {
    0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0xa, // Hello!\n
  };

  char text[] = {
    0xb8,                                    // mov rax (32bit)
    1, 0, 0, 0,                              // write syscall 1
    0xbf,                                    // mov rdi (32 bit)
    1, 0, 0, 0,                              // stdout
    0x48, 0xbe,                              // mov rsi, 64 bit pointer
    0xb0, 0x10, 0x40, 0, 0, 0, 0, 0,         // Hello strings address in virtual memory
    0xba,                                    // mov rdx (32bit)
    7, 0, 0, 0,                              // number of bytes in Hello!\n
    0xf, 0x5,                                // syscall
    0xb8,                                    // mov rax (32 bit)
    0x3c, 0, 0, 0,                           // 60 = exit syscall
    0x48, 0x31, 0xff,                        // xor rdi, rdi
    0xf, 0x5                                 // syscall
  };

  //char obj_code[] = {
  //  0x48, 0xc7, 0xc0, 0x3c, 0x00, 0x00, 0x00,    // mov    $0x3c,%rax
  //  0x48, 0xc7, 0xc7, 0x45, 0x00, 0x00, 0x00,    // mov    $0x45,%rdi
  //  0x0f, 0x05,                                  // syscall
  //};

  ElfData elf_data = {
    .type = ELF_EXECUTABLE,
    .text = &text, .text_size = sizeof(text),
    .data = &data, .data_size = sizeof(data)
  };

  elf_create(&elf, &elf_size, &elf_data);

  ASSERT(
    SUCCEEDED(io_write_binary("../test_elf", elf, elf_size)),
    "Could not write ELF to file!"
  );

  elf_free(elf);
  return 0;
}

