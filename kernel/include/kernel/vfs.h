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
#ifndef _VFS_H
#define _VFS_H
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <stdarg.h>
#define VFS_TYPE_FILE 0
#define VFS_TYPE_DIR 1
#define VFS_TYPE_SYMLINK 3
#define VFS_TYPE_MOUNTPOINT 4
#define VFS_TYPE_DEV 5
struct vfsnode;
typedef size_t (*__read)(size_t offset, size_t sizeofread, void* buffer, struct vfsnode* this);
typedef size_t (*__write)(size_t offset, size_t sizeofwrite, void* buffer, struct vfsnode* this);
typedef void (*__close)(struct vfsnode* this);
typedef struct vfsnode *(*__open)(struct vfsnode* this, const char *name);
typedef unsigned int (*__getdents)(unsigned int count, struct dirent* dirp, struct vfsnode* this);
typedef unsigned int (*__ioctl)(int request, va_list varg, struct vfsnode* this);
typedef struct vfsnode
{
	ino_t inode;
	int gid;
	int uid;
	int permitions;
	int type;
	size_t size;
	int refcount;
	char *name;
	char *mountpoint;
	struct vfsnode *next;
	struct vfsnode *link;
	__read read;
	__write write;
	__open open;
	__close close;
	__getdents getdents;
	__ioctl ioctl;
}vfsnode_t;

size_t read_vfs(size_t offset, size_t sizeofread, void* buffer, vfsnode_t* this);
size_t write_vfs(size_t offset, size_t sizeofwrite, void* buffer, vfsnode_t* this);
void close_vfs(vfsnode_t* this);
vfsnode_t *open_vfs(vfsnode_t* this, const char*);
int mount_fs(vfsnode_t *node, const char *mp);
unsigned int getdents_vfs(unsigned int count, struct dirent* dirp, vfsnode_t *this);
int ioctl_vfs(int request, va_list args, vfsnode_t *this);
int vfs_init();
vfsnode_t* vfs_findnode(const char *path);
void vfs_register_node(vfsnode_t *toBeAdded);
int vfs_destroy_node(vfsnode_t *toBeRemoved);
extern vfsnode_t* fs_root;
#endif
