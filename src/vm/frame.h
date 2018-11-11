#ifndef VM_FRAME_H
#define VM_FRAME_H


#include <hash.h>
#include "threads/palloc.h"




struct frame {
  uint8_t * kpage;
  uint8_t * upage;

  struct thread * holder;
  struct hash_elem hash_elem;
};


void frame_init (void);
void * frame_palloc_get_page (enum palloc_flags flags, uint8_t * upage);
void frame_palloc_free_page (uint8_t * kpage);
struct frame * frame_lookup(void * kpage);
struct frame * find_victim(void * kpage);

unsigned frame_hash(const struct hash_elem *, void *);
bool frame_less(const struct hash_elem *, const struct hash_elem *, void *);
//void ** frame_palloc_get_multiple (enum palloc_flags flags, size_t page_cnt); // it wouldn't used

#endif /* vm/frame.h */

