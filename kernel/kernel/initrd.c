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
#include <kernel/initrd.h>
#include <kernel/vfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <kernel/panic.h>
#include <assert.h>
tar_header_t *headers[100] = { 0 };
size_t n_files = 0;
size_t tar_parse(uintptr_t address)
{
	size_t i = 0;

	for (i = 0;; i++) {
		tar_header_t *header = (tar_header_t *) address;
		if (header->filename[0] == '\0')
			break;
		size_t size = tar_get_size(header->size);
		headers[i] = header;
		address += ((size / 512) + 1) * 512;
		if (size % 512)
			address += 512;
	}
	return i;
}
size_t tar_read(size_t offset, size_t sizeOfReading, void *buffer, vfsnode_t *this)
{
	char *tempBuffer = (char *) headers[this->inode] + 512 + offset;
	memcpy(buffer, tempBuffer, sizeOfReading);
	return sizeOfReading;
}

size_t tar_write(size_t offset, size_t sizeOfWriting, void *buffer, vfsnode_t *this)
{
	(void) offset;
	(void) sizeOfWriting;
	(void) buffer;
	(void) this;
	/* You can not write to a tar file (usually results in corruption) */
	return errno = EROFS, 0;
}
void tar_close(vfsnode_t *this)
{
	(void) this;
	return;
}
unsigned int tar_getdents(unsigned int count, struct dirent* dirp, vfsnode_t* this)
{
	char *full_path = this->name;
	tar_header_t **iterator = headers;
	unsigned int found = 0;
	for(size_t i = 0; i < n_files; i++)
	{
		if(!strcmp(iterator[i]->filename, full_path))
			continue;
		if(!memcmp(iterator[i]->filename, full_path, strlen(full_path)))
		{
			/* The memcmp above was just a primitive search, this is the real parsing
			   Basically the code after this parses the string using strtok(3) and tries to figure out if it's
			   a direct child of the directory, etc.
			   Note that some parts of the code are specific to tarfs(that's why it's under 'initrd.c')
			*/
			char *l = iterator[i]->filename, *temp = NULL, *before = l;
			l = strtok(l, "/");
			while((temp = strtok(NULL, "/")))
			{
				if(*temp == '/' && temp[1] == '\0')
					break;
				before = l;
				l = temp;	
			}
			/* If the token before the last / isn't equal to the last bytes of the full_path, it's not a 
			   direct child 
			*/
			char *last_tok = full_path+(strlen(full_path) - (size_t) (l+1 - before));
			if(memcmp(before, last_tok, (size_t) (l+1 - before)))
				continue;
			l++;
			dirp[found].d_ino = i;
			strcpy(dirp[found].d_name, l);
			if(iterator[i]->typeflag == TAR_TYPE_DIR)
				dirp[found].d_type = DT_DIR;
			else if(iterator[i]->typeflag == TAR_TYPE_FILE)
				dirp[found].d_type = DT_REG;
			else if(iterator[i]->typeflag == TAR_TYPE_CHAR_SPECIAL)
				dirp[found].d_type = DT_CHR;
			else if(iterator[i]->typeflag == TAR_TYPE_BLOCK_SPECIAL)
				dirp[found].d_type = DT_BLK;
			else if(iterator[i]->typeflag == TAR_TYPE_HARD_LNK || iterator[i]->typeflag == TAR_TYPE_SYMLNK)
				dirp[found].d_type = DT_LNK;
			else
				dirp[found].d_type = DT_UNKNOWN;
			found++;
			count--;
		}
		if(count == 0)
			break;
	}
	return found;
}
vfsnode_t *tar_open(vfsnode_t *this, const char *name)
{
	char *full_path = malloc(strlen(this->name) + strlen(name) + 1);

	assert(full_path);

	strcpy(full_path, this->name);
	strcpy(full_path + strlen(this->name), name);
	full_path[strlen(this->name) + strlen(name)] = 0;

	tar_header_t **iterator = headers;
	for(size_t i = 0; i < n_files; i++)
	{
		if(!strcmp(full_path, iterator[i]->filename))
		{
			// This part of the code seems broken, needs to be looked at
			vfsnode_t *node = malloc(sizeof(vfsnode_t));
			assert(node);
			memset(node, 0, sizeof(*node));
			node->name = malloc(strlen(this->mountpoint) + strlen(full_path));
			strcpy(node->name, this->mountpoint);
			strcpy(node->name + strlen(this->mountpoint), full_path);
			node->open = tar_open;
			node->close = tar_close;
			node->read = tar_read;
			node->write = tar_write;
			node->getdents = tar_getdents;
			node->inode = i;
			node->size = tar_get_size(iterator[i]->size);
			if(iterator[i]->typeflag == TAR_TYPE_DIR)
				node->type = VFS_TYPE_DIR;
			else
				node->type = VFS_TYPE_FILE;
			return node;
		}
	}
	return errno = ENOENT, NULL;
}
void init_initrd(void *initrd)
{
	printf("Found an Initrd at %p\n", initrd);
	n_files = tar_parse((uintptr_t) initrd);
	printf("Found %d files in the Initrd\n", n_files);
	vfsnode_t *node = malloc(sizeof(vfsnode_t));
	assert(node);
	memset(node, 0, sizeof(vfsnode_t));
	node->name = "sysroot/";
	node->open = tar_open;
	node->close = tar_close;
	node->read = tar_read;
	node->write = tar_write;
	node->getdents = tar_getdents;
	node->type = VFS_TYPE_DIR;
	node->inode = 0;
	mount_fs(node, "/");
	printf("Mounted initrd on /\n");
}
