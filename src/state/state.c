/*
 * =====================================================================================
 *
 *       Filename:  state.c
 *
 *    Description:  state.c
 *
 *        Version:  1.0
 *        Created:  06/20/17 14:26:44
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
#include "state.h"

struct State
{
	const char *name;

	state_t state;
	event_t event;
	error_t error;

	flag_t  available:1;
	flag_t  noright:1;

	StateTrigger *trigger;

	void *ctx;
};

static
State* state_malloc(size_t size)
{
	return (State*)malloc(size);
}

static
void state_free(State *state)
{
	free(state);
}

static
void state_set_error(State *state, error_t error)
{
	state->error = error;
}

static
error_t state_get_error(State *state)
{
	return state->error;
}

static
void state_set_available(State *state)
{
	state->available = 1;
}

static
void state_set_unavailable(State *state)
{
	state->available = 0;
}

static
int state_is_available(State *state)
{
	if (state->available) {
		return 1;
	}

	return 0;
}

static
void state_set_noright(State *state)
{
	state->noright = 1;
}

static 
void state_set_right(State *state)
{
	state->noright = 0;
}

static
int state_has_right(State *state)
{
	if (state->noright) {
		return 0;
	}

	return 1;
}

StateTrigger* state_set_trigger(State *state, StateTrigger *trigger)
{
	if (state) {
		StateTrigger *old = state->trigger;
		state->trigger = trigger;
		return old;
	}

	return 0;
}

StateTrigger* state_get_trigger(State *state)
{
	if (state) {
		return state->trigger;
	}

	return 0;
}

static
int state_trigger_exit(State *state, state_t cur)
{
	if (!state_is_available(state)) {
		return -1;
	}

	int ret = 0;
	StateTrigger *trigger = state_get_trigger(state);

	if (trigger && trigger->exit) {
		state_set_noright(state);
		ret = trigger->exit(state, cur);
		state_set_right(state);
		if (ret) {
			state_set_unavailable(state);
			state_set_error(state, STATE_EXIT_FAILED);
		}
	}

	return ret;
}

static
int state_trigger_enter(State *state, state_t from, state_t to)
{
	if (!state_is_available(state)) {
		return -1;
	}

	int ret = 0;
	StateTrigger *trigger = state_get_trigger(state);

	if (trigger && trigger->enter) {
		state_set_noright(state);
		ret = trigger->enter(state, from, to);
		state_set_right(state);
		if (ret) {
			state_set_unavailable(state);
			state_set_error(state, STATE_ENTER_FAILED);
		}
	}

	return ret;
}

static
int state_trigger_run(State *state, state_t cur, event_t event, void *data)
{
	if (!state_is_available(state)) {
		return -1;
	}

	int ret = 0;
	StateTrigger *trigger = state_get_trigger(state);

	if (trigger && trigger->run) {
		ret = trigger->run(state, cur, event, data);
		if (ret) {
			state_set_unavailable(state);
			state_set_error(state, STATE_RUN_FAILED);
		}
	}

	return ret;
}

void state_set_name(State *state, const char *name)
{
	if (state && name) {
		state->name = name;
	}
}

const char* state_get_name(State *state)
{
	if (state) {
		return state->name;
	}
	return 0;
}

void* state_set_ctx(State *state, void *ctx)
{
	if (state) {
		void *old = state->ctx;
		state->ctx = ctx;
		return old;
	}

	return 0;
}

void* state_get_ctx(State *state)
{
	if (state) {
		return state->ctx;
	}

	return 0;
}

State* state_new(const char *name, StateTrigger *trigger, void *ctx)
{
	State *state = (State*)state_malloc(sizeof(State));
	if (!state) {
		return 0;
	}

	memset(state, 0, sizeof(State));
	state->name = name;
	state->trigger = trigger;
	state->ctx = ctx;
	state_set_available(state);

	(void)state_trigger_enter(state, 0, 0);
	return state;
}

void state_destroy(State *state)
{
	if (state) {
		state_free(state);
	}
}

int state_set_event(State *state, event_t event)
{
	if (state && state_has_right(state)) {
		state->event = event;
		return 1;
	}

	return 0;
}

int state_set_state(State *state, state_t next)
{
	if (state && state_has_right(state)) {
		state->state = next;
		return 1;
	}

	return 0;
}

int state_trigger(State *state, event_t event, void *data)
{
	if (!state || !state_is_available(state)) {
		return -1;
	}

	state_t cur = state->state;
	state->event = event;

retry:
	if (state_trigger_run(state, cur, event, data)) {
		return -1;
	}

	if (cur != state->state) {
		if (state_trigger_exit(state, cur)) {
			return -1;
		}
		if (state_trigger_enter(state, cur, state->state)) {
			return -1;
		}
		cur = state->state;
		event = state->event;
		goto retry;
	} else if (event != state->event) {
		event = state->event;
		goto retry;
	}

	return 0;
}

int state_reset(State *state) 
{
	if (!state) {
		return -1;
	}

	if (state_is_available(state)) {
		state_trigger_exit(state, state->state);
	}

	state_set_state(state, 0);
	state_set_event(state, 0);
	state_set_available(state);

	return 0;
}
