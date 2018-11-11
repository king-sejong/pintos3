#include "vm/frame.h"
#include <hash.h>
#include <stdio.h>
#include <stdint.h>
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/malloc.h"
#include "threads/palloc.h"

static struct hash frame_table;
static struct lock frame_lock;
static struct list frame_list;


unsigned frame_hash(const struct hash_elem *fr, void *aux UNUSED){
	struct frame *f = hash_entry(fr, struct frame, hash_elem);
	return hash_bytes (&f->kaddr, sizeof(f->kaddr));
}

bool frame_less(const struct hash_elem *first, const struct hash_elem *second, void *aux UNUSED){
	const struct frame *f1 = hash_entry(first, struct frame, hash_elem);
	const struct frame *f2 = hash_entry(second, struct frame, hash_elem);
	return f1->kaddr < f2->kaddr;
}

void frame_init(void){
	hash_init(&frame_table, frame_hash, frame_less, NULL);
	lock_init(&frame_lock);
	list_init(&frame_list);
}

bool frame_install(void *kaddr, void *uaddr, struct thread *t){
	struct frame *f= malloc(sizeof(struct frame));
	f->kaddr = kaddr;
	f->uaddr = uaddr;
	f->holder = t;

	lock_acquire(&frame_lock);
	struct hash_elem *e =  hash_insert(&frame_table, &f->hash_elem);
	lock_release(&frame_lock);

	list_push_back(&frame_list, &f->l_elem);
	if(!e)
		return true;

	free(f);
	return false;
}

bool frame_delete(void *kaddr){
	/*struct frame *f;
	f = frame_find(kaddr);
	if(f==NULL)
		return false;

	lock_acquire(&frame_lock);
	struct hash_elem *e = hash_delete(&frame_table, &f->hash_elem);
	list_remove(&f->l_elem);
	lock_release(&frame_lock);

	if(!e)
		return false;

	free(hash_entry(e,struct frame, hash_elem));
	return true;
	*/
	struct frame * f = (struct frame *)malloc(sizeof(struct frame));
  	f -> kaddr = kaddr;
  	struct hash_elem *e = hash_delete(&frame_table, &(f->hash_elem));
  
  	palloc_free_page (kaddr);
  		free (f);
  	if (!e)
    	return false;
  	free(hash_entry(e, struct frame, hash_elem));
  	return true;
}

struct frame* frame_find(void *kaddr){
	struct frame f;
	f.kaddr = kaddr;
	lock_acquire(&frame_lock);
	struct hash_elem *e = hash_find(&frame_table, &f.hash_elem);
	lock_release(&frame_lock);	

	if(e == NULL)
		return NULL;
	return hash_entry(e, struct frame, hash_elem);
}

void *frame_allocate(int flag, uint8_t *uaddr){
	void *new_frame = palloc_get_page(PAL_USER | flag);
	if(new_frame==NULL){

		find_victim();
		/*struct frame *victim = find_victim();

		pagedir_clear_page(victim->holder->pagedir, victim->uaddr);
		size_t index = swap_out(victim->kaddr);
		struct page p1= page_find(victim->holder->page_table,victim->uaddr);
		p1->swapped =true;
		frame_delete(victim->kaddr);
		*/
		new_frame = palloc_get_page(PAL_USER | flag);
	}
	
	frame_install(new_frame, uaddr, thread_current());
	return new_frame;
}

void find_victim(void){

	struct list_elem *elem;
	struct frame *f1;
	bool done =false;
	/*
	while(!done){
		for(elem=list_begin(&frame_list);elem!=list_end(&frame_list);elem=list_next(elem)){
			f1=list_entry(elem, struct frame, l_elem);
			if(!pagedir_is_accessed(f1->holder->pagedir, f1->uaddr)){
				done =true;
				break;
			}
			pagedir_set_accessed(f1->holder->pagedir,f1->uaddr,false);
		}
	}*/
	int i;
	for(i=0;i<2;i++){
		for(elem=list_begin(&frame_list);elem!=list_end(&frame_list);elem=list_next(elem)){
			f1=list_entry(elem, struct frame, l_elem);
			if(!pagedir_is_accessed(f1->holder->pagedir, f1->uaddr)){
				done =true;
				break;
			}
			pagedir_set_accessed(f1->holder->pagedir,f1->uaddr,false);
		}
	}
	if(done)
		clear_victim(f1);
}

void clear_victim(struct frame *victim){
	pagedir_clear_page(victim->holder->pagedir, victim->uaddr);
	size_t index = swap_out(victim->kaddr);
	struct page *p1= page_find(victim->holder->page_table,victim->uaddr);
	//p1->swapped =true;
	frame_delete(victim->kaddr);
}
