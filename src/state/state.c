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
	state->flag |= 0x1;
}

static
void state_set_unavailable(State *state)
{
	if (state->flag & 0x1) {
		state->flag &= ~0x1;
	}
}

static
int state_is_available(State *state)
{
	if (state->flag & 0x1) {
		return 1;
	}

	return 0;
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
	int ret = 0;
	StateTrigger *trigger = state_get_trigger(state);

	if (trigger && trigger->exit) {
		ret = trigger->exit(state, cur);
		if (ret) {
			state_set_unavailable(state);
			state_set_error(state, STATE_EXIT_FAILED);
		}
	}

	return ret;
}

static
int state_trigger_enter(State *state, state_t cur)
{
	int ret = 0;
	StateTrigger *trigger = state_get_trigger(state);

	if (trigger && trigger->enter) {
		ret = trigger->enter(state, cur);
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

	if (trigger->enter(state, state->state)) {
		state->error = STATE_ENTER_FAILED;
	}

	return state;
}

void state_destroy(State *state)
{
	if (state) {
		state_free(state);
	}
}

void state_set_event(State *state, event_t event)
{
	if (state) {
		state->event = event;
	}
}

void state_set_state(State *state, state_t next)
{
	if (state) {
		state->state = next;
	}
}

int state_run(State *state, event_t event, void *data)
{
	if (!state || !state_is_available(state)) {
		return -1;
	}

	state_t cur = state->state;
	state->event = event;

	if (state_trigger_run(state, cur, event, data)) {
		return -1;
	}

	if (cur != state->state) {
		if (state_trigger_exit(state, cur)) {
			return -1;
		}
		if (state_trigger_enter(state, state->state)) {
			return -1;
		}
		if (state_trigger_run(state, state->state, state->event, data)) {
			return -1;
		}
	} else if (event != state->event) {
		if (state_trigger_run(state, state->state, state->event, data)) {
			return -1;
		}
	}

	return 0;
}

