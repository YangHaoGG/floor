/*
 * =====================================================================================
 *
 *       Filename:  buffer.c
 *
 *    Description:  basic buffer
 *
 *        Version:  1.0
 *        Created:  06/22/17 15:40:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include "buffer_common.h"

struct StringBuffer {
	Buffer head;
	char *addr;
	char buffer[0];
};

static 
ssize_t string_buffer_read(Buffer *head, size_t off, char *out, size_t size)
{
	if (off >= head->size) {
		return -1;
	}

	if (off + size > head->size) {
		size = head->size - off;
	}

	StringBuffer *buffer = (StringBuffer*)head;
	memcpy(out, buffer->addr + off, size);
	return size;
}

static 
ssize_t string_buffer_write(Buffer *head, size_t off, const char *str, size_t size)
{
	if (off + size >= head->capability) {
		return -1;
	}

	StringBuffer *buffer = (StringBuffer*)head;
	memcpy(buffer->addr + off, str, size);
	if (off > head->size) {
		head->size = off + size;
	} else if (off + size > head->size) {
		head->size = off + size;
	}

	return size;
}

static 
ssize_t string_buffer_replace(Buffer *head, size_t start, size_t end, const char *str, size_t size)
{
	if (start > end || end > head->size) {
		return -1;
	}

	StringBuffer *buffer = (StringBuffer*)head;
	size_t old = end - start;
	if (old != size) {
		if ((ssize_t)(size - old) > (ssize_t)(head->capability - head->size)) {
			return -1;
		}
		if (end < head->size) {
			memmove(buffer->addr + start + size, buffer->addr + end, head->size - end);
		}
		head->size += (ssize_t)(size - old);
	}

	memcpy(buffer->addr + start, str, size);
	return size;
}

static 
int string_buffer_range_create(Buffer *head, BufferRange *range, size_t off, size_t len)
{
	if (off + len > head->size) {
		return -1;
	}

	BufferPos *pos = &range->left;
	pos->pos = head;
	pos->off = off;

	pos = &range->right;
	pos->pos = head;
	pos->off = off + len;

	range->size = len;
	return 0;
}

static 
int string_buffer_range_iter(Buffer *head, BufferRange *in, BufferRange *out, pcall_t iter, void *pattern, void *param)
{
	if (in->left.pos != in->right.pos || in->left.pos != head) {
		return -1;
	}

	memset(out, 0, sizeof(BufferRange));
	StringBuffer *buffer = (StringBuffer*)head;
	return iter(head, out, buffer->addr + in->left.off, in->size, pattern, param);
}

BufferOps g_buf_ops = {
	.read = string_buffer_read,
	.write = string_buffer_write,
	.replace = string_buffer_replace,
	.range_create = string_buffer_range_create,
	.range_iter = string_buffer_range_iter,
};

Buffer* get_string_buffer(StringBuffer *buffer)
{
	return &buffer->head;
}

StringBuffer* string_buffer_init(void *data, size_t size)
{
	StringBuffer *buffer = (StringBuffer*)data;
	buffer_init(&buffer->head, &g_buf_ops, size - sizeof(StringBuffer));
	buffer->addr = buffer->buffer;

	return buffer;
}
