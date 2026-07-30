#ifndef ARENA_H
#define ARENA_H

#define ARENA_IMPLEMENTATION
#include "./../../external/tsoding/arena.h"
static Arena event_arena = {0};
static Arena *event_arena_ptr = &event_arena;

void *event_arena_alloc(size_t size) {
  assert(event_arena_ptr);
  return arena_alloc(event_arena_ptr, size);
}

void event_arena_free() {
  assert(event_arena_ptr);
  arena_free(event_arena_ptr);
}

#endif
/* vim:set ts=3 sw=2 sts=2 et: */
