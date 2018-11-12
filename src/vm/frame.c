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


static struct hash frame_table;
static struct lock frame_lock;



void
frame_init(void)
{
  lock_init (&frame_lock);
  hash_init (&frame_table, frame_hash, frame_less, NULL);

}



void *
frame_palloc_get_page(enum palloc_flags flags, uint8_t *upage)
{


  void * allocated_frame;
  struct frame * f = malloc(sizeof(struct frame));
  allocated_frame = palloc_get_page(PAL_USER | flags);
  if(allocated_frame == NULL){

    // there is no frame left. should do swap.
    return NULL;
  }
  f -> kpage = allocated_frame;
  f -> upage = upage;
  f -> holder = thread_current();
  
  struct hash_elem *e =  hash_insert(&frame_table, &f->hash_elem);
  if (!e){
    return allocated_frame;
  }
  free(f);
  return hash_entry(e, struct frame, hash_elem);
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
frame_palloc_free_page (uint8_t *kpage)
{

  ASSERT(kpage != NULL);

  struct frame * f = (struct frame *)malloc(sizeof(struct frame));
  f -> kpage = kpage;
  struct hash_elem *e = hash_delete(&frame_table, &(f->hash_elem));
  
  palloc_free_page (kpage);
  free (f);
  if (!e)
    return;
  free(hash_entry(e, struct frame, hash_elem));
  
}

struct frame *
frame_lookup(void *kpage){
	struct frame *f = (struct frame *)malloc(sizeof (struct frame));
	f->kpage = kpage;
	lock_acquire(&frame_lock);
	struct hash_elem *e = hash_find(&frame_table, &(f->hash_elem));
	lock_release(&frame_lock);	

	if(e == NULL)
		return NULL;
	return hash_entry(e, struct frame, hash_elem);
}

struct frame *
find_victim(void *kpage){
	return NULL;
}


unsigned frame_hash(const struct hash_elem *fr, void *aux UNUSED){
  struct frame* f = hash_entry(fr, struct frame, hash_elem);
  return hash_bytes (&f->kpage, sizeof(f->kpage));
}
bool frame_less(const struct hash_elem *first, const struct hash_elem *second, void *aux UNUSED){
  const struct frame *f1 = hash_entry(first, struct frame, hash_elem);
  const struct frame *f2 = hash_entry(second, struct frame, hash_elem);
  return f1->kpage < f2->kpage;
}




void
frame_lock_acquire(void){
  lock_acquire(&frame_lock);
}

void
frame_lock_release(void){
  lock_release(&frame_lock);
}
