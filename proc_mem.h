#include "sys.h"
#include <linux/ptrace.h>
#include <linux/compat.h>
#include <linux/mm.h>
#include <linux/uio.h>
#include <linux/sched.h>
#include <linux/compat.h>
#include <linux/sched/mm.h>
#include <linux/highmem.h>
#include <linux/ptrace.h>
#include <linux/slab.h>
#include <linux/syscalls.h>

/* Maximum number of entries for process pages array
   which lives on stack */
#define PVM_MAX_PP_ARRAY_COUNT 16

#define PVM_MAX_KMALLOC_PAGES (PAGE_SIZE * 2)

int read_phy_addr(int pid, uint64_t addr, size_t len, char *buf)
{
    struct task_struct *task;
    struct mm_struct *mm;
    struct page *pp_stack[PVM_MAX_PP_ARRAY_COUNT];
    struct page **process_pages = pp_stack;

    unsigned long pa = addr & PAGE_MASK;
    unsigned long offset = addr - pa;

    unsigned int flags = 0;

    unsigned long nr_pages = (addr + len - 1) / PAGE_SIZE - addr / PAGE_SIZE + 1;

    unsigned long max_pages_per_loop = PVM_MAX_KMALLOC_PAGES / sizeof(struct pages *);

    int pinned_pages = min(nr_pages, max_pages_per_loop);
    int locked = 1;

    /* Get process information */
    task = find_task_by_vpid(pid);

    mm = get_task_mm(task);

    mmap_read_lock(mm);
    pinned_pages = pin_user_pages_remote(mm, pa, pinned_pages,
                                         flags, process_pages,
                                         NULL, &locked);
    if (locked)
    {
        mmap_read_unlock(mm);
    }

    rcu_read_lock();
    int copied = 0;
    while (len > 0)
    {
        struct page *page = *process_pages++;
        size_t copy = PAGE_SIZE - offset - len < 0 ? PAGE_SIZE - offset : len;
        void *kaddr = kmap_atomic(page);
        memcpy(buf + copied, kaddr + offset, copy);
        // printk(KERN_EMERG "memcpy_value:%x\n", *(uint64_t *)(kaddr + offset));
        kunmap_atomic(kaddr);
        process_pages++;
        len -= copy;
        copied += copy;
        offset = 0;
    }
    rcu_read_unlock();

    return 0;
}

int write_phy_addr(int pid, uint64_t addr, size_t len, char *buf)
{
    struct task_struct *task;
    struct mm_struct *mm;
    struct page *pp_stack[PVM_MAX_PP_ARRAY_COUNT];
    struct page **process_pages = pp_stack;

    unsigned long pa = addr & PAGE_MASK;
    unsigned long offset = addr - pa;

    unsigned int flags = 0;

    unsigned long nr_pages = (addr + len - 1) / PAGE_SIZE - addr / PAGE_SIZE + 1;

    unsigned long max_pages_per_loop = PVM_MAX_KMALLOC_PAGES / sizeof(struct pages *);

    int pinned_pages = min(nr_pages, max_pages_per_loop);
    int locked = 1;

    /* Get process information */
    task = find_task_by_vpid(pid);

    mm = get_task_mm(task);

    mmap_read_lock(mm);
    pinned_pages = pin_user_pages_remote(mm, pa, pinned_pages,
                                         flags, process_pages,
                                         NULL, &locked);
    if (locked)
    {
        mmap_read_unlock(mm);
    }

    rcu_read_lock();
    int copied = 0;
    while (len > 0)
    {
        struct page *page = *process_pages++;
        size_t copy = PAGE_SIZE - offset - len < 0 ? PAGE_SIZE - offset : len;
        void *kaddr = kmap_atomic(page);
        memcpy(kaddr + offset, buf + copied, copy);
        // printk(KERN_EMERG "memcpy_value:%x\n", *(uint64_t *)(kaddr + offset));
        kunmap_atomic(kaddr);
        process_pages++;
        len -= copy;
        copied += copy;
        offset = 0;
    }
    rcu_read_unlock();

    return 0;
}