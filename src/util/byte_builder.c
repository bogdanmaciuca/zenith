#include "byte_builder.h"
#include "debug.h"

void byte_builder__ensure_capacity(ByteBuilder* builder, usize capacity);

ByteBuilderCreateError byte_builder_create(ByteBuilder* builder, usize capacity) {
  ASSERT(capacity != 0, "Capacity must not be 0!");

  builder->ptr = malloc(capacity);
  builder->size = 0;
  builder->capacity = capacity;
  
  if (builder->ptr == 0)
    return BYTE_BUILDER_CREATE_ALLOC_FAIL;

  return BYTE_BUILDER_CREATE_SUCCESS;
}

void byte_builder_free(const ByteBuilder* const builder) {
  free(builder->ptr);
}

void byte_builder_append_bytes(void* data, usize data_size) {
  byte_builder__ensure_capacity(builder, 
}

void byte_builder__ensure_capacity(ByteBuilder* builder) {

}

