


struct page
{

  uint8_t * kpage;
  uint8_t * upage;

  struct thread * t;  
  bool dirty_bit;
  bool swapped;
  bool on_kmem;  

  struct file *file; 
  off_t file_ofs;
  uint32_t read_bytes;

  struct hash_elem hash_elem;
};




bool
page_table_init(struct hash *page_table)
{
  return hash_init(page_table, page_hash, page_less, 0);  
}


struct page *
page_create(void * kaddr, void * uaddr)
{
  struct thread *curr = thread_current();
  struct page * p = (struct page *)malloc(sizeof (struct page));
  
  
  p->kpage = kpage;
  p->upage = upage;
  p->t = curr;

  bool dirty_bit = false;
  bool swapped = false;
  bool on_kmem = true;

  struct hash_elem *he = hash_insert (&curr -> page_table, &p->hash_elem);
  if (he == NULL){
    // insert ok.
    return NULL
  }
  // already there?
  free(p);
  return hash_entry(he, struct page, hash_elem);
}


struct page*
page_find(struct hash *page_table,void* upage)
{

  struct page *p;
  struct hash_elem *e;

  p -> upage = upage;
  e = hash_find (page_table, &(p->hash_elem));
  if (e == NULL)
    return NULL;
  return hash_entry (e, struct page, hash_elem); 


}

static unsigned
page_hash(struct hash_elem *he)
{
  struct page * p = hash_entry(elem, struct page, hash_elem);
  return hash_int((int)p->upage);
}

bool
page_less(struct hash_elem *hea, struct hash_elem *heb, void * aux UNUSED)
{
  return hash_entry(hea, struct page, hash_elem)->upage < hash_entry(heb, struct page, hash_elem)->upage;
}


bool
page_file_load(struct page * p)
{

  void * kpage;
  bool success;
  if (p->read_bytes >0 ){
    kpage = frame_palloc_get_page(NULL, p->upage);
    off_t read_bytes = file_read_at(p -> file, kpage, p->read_bytes, p->file_ofs);  
    
    if(read_bytes != p->read_bytes)
      goto done;
    
    memset (kpage + read_bytes, 0 , PGSIZE - read_bytes);
  }
  else if (p->read_bytes ==0 ){
    kpage = frame_palloc_get_page(PAL_ZERO, p->upage);
    if (kpage == NULL)
      goto done;
  }
  if (!install_page(p->upage, p->kpage, p->writable))
    goto done;
  
  return true;
  done :
    if (kpage != NULL)
      frame_palloc_free_page(kpage);
    return false;
  
}

bool
page_set_zero(struct page * p)
{
  kpage = frame_palloc_get_page(PAL_ZERO, p->upage);
  if (kpage){
    return false;
  }
  if (!install_page(p->upage, p->kpage, p->writable)){
    frame_palloc_free_page9(kpage);
    return false;
  }
  
  return true;
}





