#ifndef _M_RUN_H_
#define _M_RUN_H_

#include <sys/types.h>

//Medata data block size
#define META_SIZE (sizeof(struct metadata) - 4)

typedef struct metadata *p_meta;

//Metadata
struct metadata
{
  size_t size;
  p_meta next;
  p_meta prev;
  int free;
  void*ptr;
  // A pointer tto the data section
  char data[1];
};

extern void *base;
void* m_malloc(size_t size);
void m_free(void* ptr);
void* m_realloc(void* ptr, size_t size);
p_meta find_meta(p_meta *last, size_t size);
p_meta extend_heap(p_meta last,size_t s);
void split_block(p_meta p ,size_t s);
p_meta fusion(p_meta p);
void copy_block(p_meta src,p_meta dst);
p_meta get_block(void *p);
int valid_addr(void *p);
void free(void *p);
#endif
