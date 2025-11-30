#pragma once
#include "common.h"

typedef struct {
  void* ptr;
  usize size;
  usize capacity;
} ByteBuilder;

typedef enum {
  BYTE_BUILDER_CREATE_SUCCESS,
  BYTE_BUILDER_CREATE_ALLOC_FAIL
} ByteBuilderCreateError;

ByteBuilderCreateError byte_builder_create(ByteBuilder* builder, usize capacity);
void byte_builder_free(const ByteBuilder* const builder);

void byte_builder_append_bytes(void* data, usize data_size);
void byte_builder_append_cstring(const char* str);
void byte_builder_append_i32();

