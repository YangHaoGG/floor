/*
 * =====================================================================================
 *
 *       Filename:  buffer.h
 *
 *    Description:  interface of buffer
 *
 *        Version:  1.0
 *        Created:  06/26/17 16:10:38
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

#ifdef __cplusplus
extern "C" {
#endif

#include "block.h"

typedef struct Buffer Buffer;
typedef struct BufferIndex BufferIndex;
typedef struct BufferRange BufferRange;

struct BufferIndex {
	Block *block;
	size_t off;
};

struct BufferRange {
	BufferIndex left;
	BufferIndex right;
	size_t size;
};

ssize_t buffer_read(Buffer *buffer, size_t off, char *out, size_t size);
ssize_t buffer_write(Buffer *buffer, size_t off, const char *in, size_t size);
ssize_t buffer_fread(Buffer *buffer, size_t off, int fd, size_t size);
ssize_t buffer_fwrite(Buffer *buffer, size_t off, int fd, size_t size);
ssize_t buffer_replace(Buffer *buffer, size_t start, size_t end, const char *in, size_t size);
ssize_t buffer_insert(Buffer *buffer, size_t off, const char *in, size_t size);
ssize_t buffer_delete(Buffer *buffer, size_t start, ssize_t end);
ssize_t buffer_match(Buffer *buffer, size_t *start, size_t *end, const char *in, size_t size);

#ifdef __cplusplus
}
#endif

#endif
