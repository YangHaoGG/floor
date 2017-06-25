/*
 * =====================================================================================
 *
 *       Filename:  buffer_head.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/24/17 05:43:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __BUFFER_HEAD_H__
#define __BUFFER_HEAD_H__

#include "buffer.h"

typedef struct BufferOps BufferOps;

struct Buffer {
	BufferOps *ops;
	Buffer *next;
	
	size_t size;
	size_t capability;
};

struct BufferOps {
	ssize_t (*read)(Buffer *head, size_t off, char *out, size_t size);
	ssize_t (*write)(Buffer *head, size_t off, const char *str, size_t size);
	ssize_t (*replace)(Buffer *head, size_t start, size_t end, const char *str, size_t size);
	Buffer* (*iter)(Buffer *buffer);
	int (*range_create)(Buffer *head, BufferRange *range, size_t off, size_t len);
	int (*range_iter)(Buffer *head, BufferRange *in, BufferRange *out, pcall_t iter, void *pattern, void *param);
};

extern void buffer_init(Buffer *head, BufferOps *ops, size_t capability);

#endif 
