/*
 * =====================================================================================
 *
 *       Filename:  raw_buffer.c
 *
 *    Description:  raw_buffer
 *
 *        Version:  1.0
 *        Created:  06/24/17 03:39:47
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

struct RawBuffer {
	struct list_head head;
	BufferOps *ops;

	size_t size;
	size_t capability;
};
