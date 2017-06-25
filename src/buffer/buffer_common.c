/*
 * =====================================================================================
 *
 *       Filename:  buffer_common.c
 *
 *    Description:  api interface
 *
 *        Version:  1.0
 *        Created:  06/24/17 06:36:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "buffer.h"
#include "buffer_common.h"

void buffer_init(Buffer *head, BufferOps *ops, size_t capability)
{
	head->ops = ops;
	head->next = 0;
	head->size = 0;
	head->capability = capability;
}

ssize_t buffer_read(Buffer *head, size_t off, char *out, size_t size)
{
	return head->ops->read(head, off, out, size);
}

ssize_t buffer_write(Buffer *head, size_t off, const char *str, size_t size)
{
	return head->ops->write(head, off, str, size);
}

ssize_t buffer_replace(Buffer *head, size_t start, size_t end, const char *str, size_t size)
{
	return head->ops->replace(head, start, end, str, size);
}

ssize_t buffer_insert(Buffer *head, size_t off, const char *str, size_t size)
{
	return buffer_replace(head, off, off, str, size);
}

ssize_t buffer_delete(Buffer *head, size_t off, size_t size)
{
	return buffer_replace(head, off, off + size, 0, 0);
}

int string_find(Buffer *buffer, BufferRange *out, char *src, size_t src_len, void *pattern, void *param)
{
	const char *find = (const char*)pattern;
	size_t find_len = (size_t)param;

	if (find_len == 0 || find_len > src_len) {
		return 0;
	}

	size_t i = 0, j = 0, loop = src_len - find_len;
	for (; i <= loop; i++) {
		for (j = 0; j < find_len; j++) {
			if (src[i + j] != find[j]) {
				break;
			}
		}
		if (j == find_len) {
			break;
		}
	}

	if (i > loop) {
		return 0;
	}

	if (out->left.pos == 0) {
		out->left.pos = buffer;
		out->left.off = i;
	}
	out->right.pos = buffer;
	out->right.off = i + find_len;
	out->size += find_len;

	return 1;
}

int buffer_find(Buffer *head, BufferRange *out, size_t off, const char *str, size_t size)
{
	if (head->size < off) {
		return -1;
	}

	BufferRange in;

	in.left.pos = head;
	in.left.off = off;
	in.right.pos = head;
	in.right.off = head->size;
	in.size = head->size - off;

	return head->ops->range_iter(head, &in, out, string_find, (void*)str, (void*)size);
}
