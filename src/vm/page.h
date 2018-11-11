
#include <stdint.h>
#include <hash.h>
#include "filesys/file.h"
#include "threads/synch.h"

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
  bool writable;

  struct hash_elem hash_elem;
};

bool page_table_init(struct hash *);
struct page * page_create(void *, void*);
struct page * page_find(struct hash *, void*);
static unsigned page_hash(const struct hash_elem *, void * aux);
bool page_less (const struct hash_elem*, const struct hash_elem*, void * aux);
bool page_file_load(struct page *);
bool page_set_zero(struct page *);

