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
#ifndef _STDIO_IMPL_H
#define _STDIO_IMPL_H
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
struct _IO_FILE
{
	int fd;
	void *buf;
	size_t buf_size;
	off_t off;
	unsigned int mode;
	volatile long lock;
};
size_t __stdio_write(const void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t __stdio_read(const void *ptr, size_t size, size_t nmemb, FILE *stream);
#endif