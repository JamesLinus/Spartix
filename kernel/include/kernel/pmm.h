/*----------------------------------------------------------------------
 * Copyright (C) 2016 Pedro Falcato
 *
 * This file is part of Spartix, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * You can redistribute it and/or modify it under the terms of the GNU
 * General Public License version 2 as published by the Free Software
 * Foundation.
 *----------------------------------------------------------------------*/
#ifndef _PMM_H
#define _PMM_H

#include <stdint.h>
#include <stddef.h>
/* block size (4KiB) */
#define PMM_BLOCK_SIZE	4096
typedef struct stack_entry
{
	uintptr_t base;
	size_t size;
	size_t magic;
}stack_entry_t;
typedef struct stack
{
	stack_entry_t* next;
}stack_t;
namespace PhysicalMemoryManager
{
	size_t GetUsedMem();
	void Push(uintptr_t base,size_t size,size_t kernel_space_size);
	void Pop();
	void Init(size_t memory_size,uintptr_t stack_space);
	void *Alloc(size_t blocks);
	void  Free(size_t blocks,void* ptr);

}
#endif
