/*
 * =====================================================================================
 *
 *       Filename:  buffer.h
 *
 *    Description:  kinds of buffer
 *
 *        Version:  1.0
 *        Created:  06/22/17 15:33:44
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Buffer Buffer;
typedef struct RawBuffer RawBuffer;
typedef struct StringBuffer StringBuffer;
typedef struct BufferIndex BufferIndex;
typedef struct BufferRange BufferRange;

typedef int (*pcall_t)(Buffer *buffer, BufferRange *out, char *src, size_t len, void *pattern, void *param);

struct BufferIndex {
	Buffer *pos;
	size_t off;
};

struct BufferRange {
	BufferIndex left;
	BufferIndex right;
	size_t size;
};

extern Buffer* string_buffer_get_head(StringBuffer *buffer);
extern StringBuffer* string_buffer_next(StringBuffer *buffer);
extern StringBuffer* string_buffer_init(void *data, size_t size);
extern ssize_t buffer_read(Buffer *head, size_t off, char *out, size_t size);
extern ssize_t buffer_write(Buffer *head, size_t off, const char *str, size_t size);
extern ssize_t buffer_replace(Buffer *head, size_t start, size_t end, const char *str, size_t size);
extern ssize_t buffer_insert(Buffer *head, size_t off, const char *str, size_t size);
extern ssize_t buffer_delete(Buffer *head, size_t off, size_t size);
extern int buffer_find(Buffer *head, BufferRange *out, size_t off, const char *str, size_t size);

#ifdef __cplusplus
}
#endif

#endif
