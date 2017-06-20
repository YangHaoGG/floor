/*
 * =====================================================================================
 *
 *       Filename:  state.h
 *
 *    Description:  state header
 *
 *        Version:  1.0
 *        Created:  06/20/17 14:06:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __STATE_H__
#define __STATE_H__

typedef unsigned char state_t;
typedef unsigned char event_t;
typedef unsigned char error_t;
typedef unsigned char flag_t;

typedef struct State State;
typedef struct StateTrigger StateTrigger;

enum StateError
{
	STATE_ENTER_FAILED,
	STATE_EXIT_FAILED,
	STATE_RUN_FAILED,
};

struct StateTrigger
{
	int (*enter)(State *state, state_t s);
	int (*run)(State *state, state_t s, event_t e, void *data);
	int (*exit)(State *state, state_t s);
};

struct State
{
	const char *name;

	state_t state;
	event_t event;
	error_t error;
	flag_t  flag;

	StateTrigger *trigger;

	void *ctx;
};

State* state_new(const char *name, StateTrigger *trigger, void *ctx);
void state_destroy(State *state);

int state_trigger(State *state, event_t event, void *data);
int state_reset(State *state);

void* state_get_ctx(State *state);
void* state_set_ctx(State *state);

StateTrigger* state_set_trigger(State *state, StateTrigger *trigger);
StateTrigger* state_get_trigger(State *state, StateTrigger *trigger);

const char* state_get_name(State *state);
void state_set_state(State *state, state_t next);
void state_set_event(State *state, event_t event);

#endif
