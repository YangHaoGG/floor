/*
 * =====================================================================================
 *
 *       Filename:  http_parser.c
 *
 *    Description:  parse http header
 *
 *        Version:  1.0
 *        Created:  06/21/17 13:49:00
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "../state.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

enum {
	PARSE_INIT,
	PARSE_REQUEST_LINE,
	PARSE_RESPONSE_LINE,
	PARSE_HEADER_LINE,
	PARSE_HEADER_END,
	PARSE_NORMAL_BODY,
	PARSE_CHUNK_BODY,
	PARSE_END,
	PARSE_FAILED,
};

enum {
	ON_REQUEST,
	ON_RESPONSE,
	RCV_DATA,
};

int parse_init(State *state, event_t ev, void *data)
{
	printf("in state PARSE_INIT, on event %d\n", ev);

	(void)data;
	if (ev == ON_REQUEST) {
		state_set_state(state, PARSE_REQUEST_LINE);
		state_set_event(state, RCV_DATA);
		//printf("here 1\n");
		return 0;
	} else if (ev == ON_RESPONSE) {
		state_set_state(state, PARSE_RESPONSE_LINE);
		state_set_event(state, RCV_DATA);
		//printf("here 2\n");
		return 0;
	}
	return -1;
}

int parse_request_line(State *state, event_t ev, void *data)
{
	printf("in state PARSE_REQUEST_LINE, on event %d\n", ev);
	if (ev != RCV_DATA) {
		return -1;
	}

	const char **param = (const char **)data;
	const char *begin = *param;
	const char *end = strstr(begin, "\r\n");
	if (!end) {
		//printf("here 3\n");
		return 0;
	}

	const char *method = begin;
	const char *method_end = strchr(method, ' ');
	if (!method_end || method_end >= end) {
		return -1;
	}

	const char *path = method_end + 1;
	const char *path_end = strchr(path, ' ');
	if (!path_end || path_end >= end) {
		return -1;
	}

	const char *proto = path_end + 1;
	const char *proto_end = end;
	if (proto >= proto_end) {
		return -1;
	}

	printf("method:%.*s, path:%.*s, proto:%.*s\n", (int)(method_end - method), method, (int)(path_end - path), path, (int)(proto_end - proto), proto);
	*param = end + 2;
	state_set_state(state, PARSE_HEADER_LINE);
	return 0;
}

int parse_header_line(State *state, event_t ev, void *data)
{
	printf("in state PARSE_HEADER_LINE, on event %d\n", ev);
	if (ev != RCV_DATA) {
		return -1;
	}

	const char **param = (const char **)data;
	while (**param) {
		const char *begin = *param;
		const char *end = strstr(begin, "\r\n");
		if (!end) {
			//printf("here 4\n");
			return 0;
		}
		if (end == begin) {
			*param += 2;
			state_set_state(state, PARSE_HEADER_END);
			//printf("here 5\n");
			return 0;
		}

		const char *pos = strchr(begin, ':');
		if (!pos || pos > end) {
			return -1;
		}

		const char *key = begin;
		const char *key_end = pos;
		if (key >= key_end) {
			return -1;
		}

		const char *value = pos + 1;
		const char *value_end = end;
		if (value >= value_end) {
			return -1;
		}
		*param = end + 2;
		printf("key:%.*s, value:%.*s\n", (int)(key_end - key), key, (int)(value_end - value), value);
	}

	return 0;
}

int parse_header_end(State *state, event_t ev, void *data)
{
	printf("in state PARSE_HEADER_END, on event %d\n", ev);
	(void)state;(void)data;
	return 0;
}

int (*parse_trigger_caller[PARSE_FAILED])(State *state, event_t ev, void *data) = {
	[PARSE_INIT] = parse_init,
	[PARSE_REQUEST_LINE] = parse_request_line,
	[PARSE_HEADER_LINE] = parse_header_line,
	[PARSE_HEADER_END] = parse_header_end,
};

int parse_run(State *state, state_t cur, event_t event, void *data)
{
	//printf("call state:%d, event %d\n", cur, event);
	if (parse_trigger_caller[cur]) {
		return parse_trigger_caller[cur](state, event, data);
	}

	return 0;
}

StateTrigger parse_trigger = {
	.run = parse_run,
};

int main()
{
	char buf[4096] = {0};

	char *head = buf;

	State *state = state_new("http_parser", &parse_trigger, 0);
	if (!state) {
		return -1;
	}

	state_trigger(state, ON_REQUEST, (void*)&head);
	strcat(head, "GET /index.html HTTP/1.1");
	state_trigger(state, RCV_DATA, (void*)&head);
	strcat(head, "\r\nHost:www.baidu.com\r\n\r\n");
	state_trigger(state, RCV_DATA, (void*)&head);
	return 0;

}
