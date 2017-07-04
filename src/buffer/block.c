/*
 * =====================================================================================
 *
 *       Filename:  block.c
 *
 *    Description:  block
 *
 *        Version:  1.0
 *        Created:  07/02/17 11:49:29
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
#include <unistd.h>
#include "block.h"

struct Block
{
	char *buffer;

	uint8_t ref;
	uint8_t ned;
	uint16_t len;
	uint16_t cap;

	Block *next;
	Block **prev;
};

ssize_t block_read(Block *block, size_t off, char *out, size_t size)
{
	if (!size || !block->len || off >= block->len) {
		return 0;
	}

	size = size + off  > block->len ? block->len - off : size;
	memcpy(out, block->buffer + off, size);

	return size;
}

ssize_t block_write(Block *block, size_t off, const char *in, size_t size)
{
	if (!size || !block->cap) {
		return 0;
	}

	size = size + off > block->cap ? block->cap - off : size;
	memcpy(block->buffer + off, in, size);
	if (off + size > block->len) {
		block->len = off + size;
	}

	return size;
}

ssize_t block_fread(Block *block, int fd, size_t off, size_t size)
{
	if (!size || !block->cap) {
		return 0;
	}
	
	size = size + off > block->cap ? block->cap - off : size;
	ssize_t r = read(fd, block->buffer + off, size);
	if (r > 0 && r + off > block->len) {
		block->len = r + off;
	}
	
	return r;
}

ssize_t block_fwrite(Block *block, int fd, size_t off, size_t size)
{
	if (!size || !block->len || off >= block->len) {
		return 0;
	}

	size = size + off > block->len ? block->len - off : size;
	return write(fd, block->buffer + off, size);
}
