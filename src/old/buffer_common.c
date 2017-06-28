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
#include <unistd.h>
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

	//skip offset when do left match
	StringBuffer* left = head->ops->first(head);
	if (!left) {
		return -1;
	}

	Buffer *buffer = string_buffer_get_head(left);
	while (buffer->size < off) {
		off -= buffer->size;
		left = string_buffer_next(left);
		if (!left) {
			return -1;
		}
		buffer = string_buffer_get_head(left);
	}
	//start left match
	ssize_t index, len;
	while (1) {
		index = string_buffer_off_match(left, off, str, size);
		if (index < 0) {
			goto next;
		}

		//record left pos
		out->left.pos = buffer;
		out->left.off = index;

		len = buffer->size - index;
		if ((size_t)len < size) {
			StringBuffer *right = left;
			const char *sub = str;
			size_t sub_size = size;

			while (1) {
				right = string_buffer_next(right);
				if (!right) {
					return -1;
				}

				sub_size -= len;
				sub += len;

				len = string_buffer_match(right, sub, sub_size);
				if (len < 0) {
					goto forward;
				} else if ((size_t)len == sub_size) {
					//record right pos
					out->right.pos = string_buffer_get_head(right);
					out->right.off = sub_size;
					out->size = size;
					return 0;
				}
			}
		} else {
			//record right pos
			out->right.pos = buffer;
			out->right.off = off + size;
			out->size = size;
			return 0;
		}

forward:
		if (++off < buffer->size) {
			continue;
		}
next:
		off = 0;
		left = string_buffer_next(left);
		if (!left) {
			return -1;
		}
		buffer = string_buffer_get_head(left);
	}

	return -1;
}

ssize_t buffer_file_read(Buffer *head, size_t off, int fd, size_t size)
{
	return head->ops->fwrite(head, off, read, fd, size);
}

ssize_t buffer_file_write(Buffer *head, size_t off, int fd, size_t size)
{
	return head->ops->fread(head, off, write, fd, size);
}
