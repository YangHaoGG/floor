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
typedef ssize_t (*reader)(int fd, void *dst, size_t size);
typedef ssize_t (*writer)(int fd, const void *src, size_t size);

struct Buffer {
	BufferOps *ops;
	Buffer *next;
	
	size_t size;
	size_t capability;
};

struct BufferOps {
	ssize_t (*read)(Buffer *head, size_t off, char *out, size_t size);
	ssize_t (*write)(Buffer *head, size_t off, const char *str, size_t size);
	ssize_t (*fread)(Buffer *head, size_t off, writer call, int fd, size_t size);
	ssize_t (*fwrite)(Buffer *head, size_t off, reader call, int fd, size_t size);
	StringBuffer* (*first)(Buffer *buffer);
	//StringBuffer* (*next)(Buffer *buffer);
	ssize_t (*replace)(Buffer *head, size_t start, size_t end, const char *str, size_t size);
	int (*range_create)(Buffer *head, BufferRange *range, size_t off, size_t len);
	//int (*range_iter)(Buffer *head, BufferRange *in, BufferRange *out, pcall_t iter, void *pattern, void *param);
};

extern void buffer_init(Buffer *head, BufferOps *ops, size_t capability);
extern int string_buffer_head_match(StringBuffer *buffer, const char *str, size_t size);
extern int string_buffer_tail_match(StringBuffer *buffer, const char *str, size_t size);
/* @return: index of substring, the length of substirng is the smallest of 
 * 			(buffer->size - index, size).
 * 			[error] -1
 */
extern ssize_t string_buffer_off_match(StringBuffer *buffer, size_t off, const char *str, size_t size);
/* @return: length of matched string, [0, length) means the buffer's and the
 * 			str's  previous length of chars are matched, if length is little
 * 			than size, it must equal buffer's size
 * 			[error] -1
 */
extern ssize_t string_buffer_match(StringBuffer *buffer, const char *str, size_t size);

#endif 
