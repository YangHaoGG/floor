/*
 * =====================================================================================
 *
 *       Filename:  buffer.c
 *
 *    Description:  buffer
 *
 *        Version:  1.0
 *        Created:  06/26/17 16:15:01
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

struct Buffer {
	uint32_t ref;

	size_t size;
	size_t capability;

	Block *head;
	Block *tail;
};

ssize_t buffer_read(Buffer *buffer, size_t off, char *out, size_t size)
{

}

ssize_t buffer_write(Buffer *buffer, size_t off, const char *in, size_t size)
{

}

ssize_t buffer_fread(Buffer *buffer, size_t off, int fd, size_t size)
{

}

ssize_t buffer_fwrite(Buffer *buffer, size_t off, int fd, size_t size)
{
	
}

ssize_t buffer_replace(Buffer *buffer, size_t start, size_t end, const char *in, size_t size)
{

}

ssize_t buffer_insert(Buffer *buffer, size_t off, const char *in, size_t size)
{

}

ssize_t buffer_delete(Buffer *buffer, size_t start, ssize_t end)
{

}

ssize_t buffer_match(Buffer *buffer, size_t *start, size_t *end, const char *in, size_t size)
{

}
