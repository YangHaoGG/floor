/*
 * =====================================================================================
 *
 *       Filename:  buffer_test.c
 *
 *    Description:  test for buffer
 *
 *        Version:  1.0
 *        Created:  06/24/17 07:02:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../buffer.h"

int main()
{
	void *data = malloc(1024);
	StringBuffer *buffer = string_buffer_init(data, 1024);
	if (!buffer) {
		printf("buffer init error!\n");
		return -1;
	}

	const char *str[] = {
		"hello world!",
		"hello Michelle!",
		"hello yhao!",
		"hello earth!",
	};
	
	int i = 0;
	ssize_t totle = 0;
	Buffer *head = get_string_buffer(buffer);
	for (;i < 3; i++) {
		totle += buffer_write(head, totle, str[i], strlen(str[i]));
	}

	char buf[512];
	if (buffer_read(head, 0, buf, 512) != totle) {
		printf("read from buffer failed:%s\n", buf);
		return -1;
	}
	buf[totle] = 0;
	printf("read from buffer:%s\n", buf);

	buffer_insert(head, 12, str[3], strlen(str[3]));
	if (buffer_read(head, 0, buf, 512) != totle + (ssize_t)strlen(str[3])) {
		printf("read from buffer failed:%s\n", buf);
		return -1;
	}
	buf[totle + strlen(str[3])] = 0;
	printf("read from buffer:%s\n", buf);

	buffer_delete(head, 12, strlen(str[3]));
	if (buffer_read(head, 0, buf, 512) != totle) {
		printf("read from buffer failed:%s\n", buf);
		return -1;
	}
	buf[totle] = 0;
	printf("read from buffer:%s\n", buf);

	buffer_replace(head, 0, 5, "HELLO", strlen("HELLO"));
	if (buffer_read(head, 0, buf, 512) != totle) {
		printf("read from buffer failed:%s\n", buf);
		return -1;
	}
	buf[totle] = 0;
	printf("read from buffer:%s\n", buf);

	BufferRange out;
	if (buffer_find(head, &out, 0, "world", 5) == 1) {
		printf("{ { %p, %lu }, { %p, %lu }, %lu }\n", out.left.pos, out.left.off, out.right.pos, out.right.off, out.size);
		buffer_read(out.left.pos, out.left.off, buf, out.size);
		printf("%.*s\n", (int)out.size, buf);
	}

	return 0;
}
