#ifndef VM_FRAME_H
#define VM_FRAME_H



#include "threads/palloc.h"




struct frame_table_entry {
  uint8_t * kpage;
  uint8_t * upage;

  struct thread * allocated_thread;
  struct list_elem e;
};


void init_frame (void);
void * frame_palloc_get_page (enum palloc_flags flags, uint8_t * page);
void frame_palloc_free_page (uint8_t * page);
struct frame_table_entry * lookup_frame(uint8_t * page);

//void ** frame_palloc_get_multiple (enum palloc_flags flags, size_t page_cnt); // it wouldn't used

#endif /* vm/frame.h */

