#include "devices/disk.h"
#include "threads/vaddr.h"
#include "vm/swap.h"

static size_t swap_size;

void swap_init(){
	swap_disk = disk_get(1,1);
	swap_size = disk_size(swap_disk)*DISK_SECTOR_SIZE/PGSIZE;
	swap_map = bitmap_create(swap_size);

}

void swap_in(size_t index, void *kpage){
	int i;
	for(i=0; i < PGSIZE/DISK_SECTOR_SIZE;i++)
		disk_read(swap_disk,index * PGSIZE/DISK_SECTOR_SIZE + i, kpage+(PGSIZE/DISK_SECTOR_SIZE*i));

	bitmap_set(swap_map,index,false);
}

size_t swap_out(void *kpage){
	size_t index=bitmap_scan_and_flip(swap_map,0,1,false);
	int i;
	for(i=0; i < PGSIZE/DISK_SECTOR_SIZE;i++)
		disk_write(swap_disk,index * PGSIZE/DISK_SECTOR_SIZE + i, kpage+(PGSIZE/DISK_SECTOR_SIZE*i));
	return index;
}

void swap_free(size_t index){
	bitmap_set(swap_map,index,false);
}
