#include <hash.h>
#include <list.h>
#include <stdio.h>
#include "lib/kernel/list.h"

#include "vm/frame.h"
#include "threads/thread.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "userprog/pagedir.h"
#include "threads/vaddr.h"


static struct list frame_table;
static struct lock frame_lock;



void
init_frame()
{
  lock_init (&frame_lock);
  list_init (&frame_table);

}

void *
frame_palloc_get_page(enum palloc_flags flags, uint8_t *upage)
{

  lock_acquire(&frame_lock);

  void * allocated_frame;
  struct frame_table_entry * frame = malloc(sizeof(struct frame_table_entry));
  allocated_frame = palloc_get_page(PAL_USER | flags);
  if(allocated_frame == NULL){

    // there is no frame left. should do swap.
    return NULL;
  }
  frame -> kpage = allocated_frame;
  frame -> upage = upage;
  frame -> allocated_thread = thread_current();
  
  list_push_back(&frame_table, &frame->e);
  
  lock_release(&frame_lock);
  return allocated_frame;
}
/*
void **
frame_palloc_get_multiple (enum palloc_flags flags ,size_t page_cnt)

{
  void * kpage;
  void * kpages = malloc(sizeof(&kpage) * page_cnt);
  void ** kpages_esp;
  for (int i=0;i < page_cnt;i++){
    kpage = frame_palloc_get_page(flags);
    if(kpage == NULL)
      return NULL;
    *kpages_esp = kpage;
    kpages_esp += sizeof(&kpage);
    
  } 
  return kpages_esp;
  
}*/

void
frame_palloc_free_page (uint8_t *page)
{

  ASSERT(page != NULL);

  struct frame_table_entry * f = lookup_frame(page);
  list_remove (&f->e);
  palloc_free_page (page);
  free (f);
  
}

struct frame_table_entry *
lookup_frame(uint8_t *page){
  struct list_elem *e;
  for (e = list_end(&frame_table); e = list_begin(&frame_table); e = list_prev(e)){
    struct frame_table_entry * fte = list_entry(e,struct frame_table_entry, e);
    if (fte -> kpage == page)
      return fte;
  }
  return NULL;
}
