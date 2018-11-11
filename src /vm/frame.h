#include <stdint.h>
#include <hash.h>
#include "filesys/file.h"
#include "threads/thread.h"

struct frame{
	struct hash_elem hash_elem;
	void *kaddr;
	void *uaddr;
	struct thread *holder;
	struct list_elem l_elem;
};

unsigned frame_hash(const struct hash_elem *, void *);
bool frame_less(const struct hash_elem *, const struct hash_elem *, void *);

void frame_init(void);
bool frame_install(void *, void *, struct thread *);
bool frame_delete(void *);
struct frame *frame_find(void *);
void *frame_allocate(int flag, uint8_t *);
void find_victim(void);
void clear_victim(struct frame *);
