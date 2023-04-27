#include "memory_helpers.h"

//TODO some locking for theese 2?
void inline reserve_current_address_space(){
	current->mm = current->active_mm;
}

void inline release_current_address_space(){
	current->mm = NULL;
}


struct mm_struct* change_mm(void) {

    struct mm_struct *old_mm, *new_mm; 

    new_mm = kmalloc(sizeof(struct mm_struct), GFP_KERNEL);

    //just copy the whole thing - don't care
    memcpy(new_mm, current->active_mm, sizeof(struct mm_struct));

    //need a new page table tough - we don't want to mess with the other processes
    new_mm->pgd = alloc_page_table();
    new_mm->mmap = NULL;
    new_mm->mm_rb = RB_ROOT;
    mmap_init_lock(new_mm);
    
    old_mm = current->active_mm;
    current->active_mm = new_mm;

    return old_mm;
}

void restore_mm(struct mm_struct* old_mm) {

    struct mm_struct* to_free;
    to_free = current->active_mm;
    current->active_mm = old_mm;
    current->mm = old_mm;

    free_page_table(to_free->pgd);
    kfree(to_free);

}

