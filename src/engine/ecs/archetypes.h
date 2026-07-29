#ifndef ARCHETYPES_H
#define ARCHETYPES_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "../../core/types.h"

#define BIT(id) (1u << (id))

#define MAX_COMPONENTS 32
#define MAX_ARCHETYPES 256
#define MAX_ENTITIES 4096
#define ARCHETYPE_HASH_BUCKETS                                                 \
  512 /* power of two, comfortably > MAX_ARCHETYPES */

#define DECLARE_COMPONENT_ID(Type) static ComponentId Type##_id
#define REGISTER(Type) (Type##_id = component_register(#Type, sizeof(Type)))

typedef u32 ComponentId;
typedef u32 Signature; /* bit i set -> archetype has component id i */

typedef struct {
  const char *name;
  size_t size;
} ComponentInfo;

static ComponentInfo g_registry[MAX_COMPONENTS];
static u32 g_component_count = 0;

typedef struct {
  u32 index;
  u32 generation;
} Entity;

typedef struct Archetype Archetype;

typedef struct {
  Archetype *archetype; /* which archetype currently holds this entity */
  u32 row;         /* row within that archetype's columns */
} EntityRecord;

static ComponentId component_register(const char *name, size_t size) {
  assert(g_component_count < MAX_COMPONENTS);
  ComponentId id = g_component_count++;
  g_registry[id] = (ComponentInfo){.name = name, .size = size};
  return id;
}

struct Archetype {
  Signature signature;
  void *columns[MAX_COMPONENTS]; /* columns[id] == NULL if id not present */
  Entity *entities;              /* dense entity list (len == count) */
  u32 count;
  u32 capacity;
  Archetype *next; /* intrusive chain link for the signature hash table */
};

static inline void *archetype_column(Archetype *a, ComponentId id) {
  return a->columns[id];
}

typedef struct {
  /* entity bookkeeping */
  u32 generation[MAX_ENTITIES];
  u32 free_list[MAX_ENTITIES];
  u32 free_count;
  EntityRecord records[MAX_ENTITIES];

  /* archetype storage */
  Archetype *archetypes[MAX_ARCHETYPES];
  u32 archetype_count;

  /* SIMPLIFICATION 3: signature -> archetype lookup is now a hash table
   * (intrusive chaining) instead of a linear scan over every archetype.
   * archetype_find_or_create() is called on every add/remove_component,
   * so this turns that from O(archetype_count) into O(1) amortized. */
  Archetype *hash_buckets[ARCHETYPE_HASH_BUCKETS];
} World;

static bool world_alive(World *w, Entity e) {
  return w->generation[e.index] == e.generation;
}

/* ^^^ [ World ] ^^^ */

/* vvv [ Archetype storage + lookup ] vvv */

static u32 hash_signature(Signature sig) {
  /* Knuth-style multiplicative mix, then mask down to table size. */
  sig ^= sig >> 16;
  sig *= 0x7feb352dU;
  sig ^= sig >> 15;
  return sig & (ARCHETYPE_HASH_BUCKETS - 1);
}

static Archetype *archetype_create(Signature sig) {
  Archetype *a = calloc(1, sizeof(Archetype));
  a->signature = sig;
  return a;
}

static Archetype *archetype_find_or_create(World *w, Signature sig) {
  u32 bucket = hash_signature(sig);
  for (Archetype *a = w->hash_buckets[bucket]; a; a = a->next) {
    if (a->signature == sig)
      return a;
  }

  assert(w->archetype_count < MAX_ARCHETYPES);
  Archetype *a = archetype_create(sig);
  a->next = w->hash_buckets[bucket];
  w->hash_buckets[bucket] = a;
  w->archetypes[w->archetype_count++] = a;
  return a;
}

static void archetype_ensure_capacity(Archetype *a, u32 needed) {
  if (needed <= a->capacity)
    return;
  u32 new_cap = a->capacity ? a->capacity * 2 : 4;
  while (new_cap < needed) {
    new_cap *= 2;
  }

  a->entities = realloc(a->entities, new_cap * sizeof(Entity));
  for (ComponentId id = 0; id < g_component_count; id++) {
    if (!(a->signature & BIT(id)))
      continue;
    size_t elem_size = g_registry[id].size;
    a->columns[id] = realloc(a->columns[id], new_cap * elem_size);
  }
  a->capacity = new_cap;
}

static u32 archetype_add_entity(Archetype *a, Entity e) {
  archetype_ensure_capacity(a, a->count + 1);
  u32 row = a->count++;
  a->entities[row] = e;
  return row;
}

/* Swap remove row "row", then fix up the EntityRecord of whichever entity
 * got swapped into that slot (the previously last row) */
static void archetype_remove_entity(World *w, Archetype *a, u32 row) {
  u32 last = a->count - 1;
  if (row != last) {
    for (ComponentId id = 0; id < g_component_count; id++) {
      if (!(a->signature & BIT(id)))
        continue;
      size_t sz = g_registry[id].size;
      char *col = a->columns[id];
      /* memcpy: (dest, src, amount) */
      memcpy(col + row * sz, col + last * sz, sz);
    }
    a->entities[row] = a->entities[last];
    Entity moved = a->entities[row];
    w->records[moved.index].row = row;
  }
  a->count--;
}

/* ^^^ [ Archetype storage + lookup ] ^^^ */

/* vvv [ World, Entity and Component API ] vvv */

static void world_init(World *w) {
  memset(w, 0, sizeof(*w));
  for (u32 i = 0; i < MAX_ENTITIES; i++) {
    w->free_list[i] = MAX_ENTITIES - i - 1;
  }
  w->free_count = MAX_ENTITIES;

  /* every entity starts in the empty entity archetype (signature: 0) */
  Archetype *empty = archetype_find_or_create(w, 0);
  (void)empty; /* discard value */
}

static Entity entity_create(World *w) {
  assert(w->free_count > 0 && "Entity Pool Exhausted");
  u32 idx = w->free_list[--w->free_count];
  Entity e = {.index = idx, .generation = w->generation[idx]};

  Archetype *empty = archetype_find_or_create(w, 0);
  u32 row = archetype_add_entity(empty, e);
  w->records[idx] = (EntityRecord){.archetype = empty, .row = row};
  return e;
}

static void entity_destroy(World *w, Entity e) {
  assert(world_alive(w, e));
  EntityRecord *rec = &w->records[e.index];
  archetype_remove_entity(w, rec->archetype, rec->row);
  w->generation[e.index]++;
  w->free_list[w->free_count++] = e.index;
}

/* ^^^ [ World, Entity and Component API ] ^^^ */

/* Move 'e' from its current archetype to one with 'new_sig', copying over
 * any components it already had that still exist in the new signature.
 * Returns the new row. */
static u32 move_entity(World *w, Entity e, Signature new_sig) {
  EntityRecord *rec = &w->records[e.index];
  Archetype *old = rec->archetype;
  Archetype *dst = archetype_find_or_create(w, new_sig);

  u32 new_row = archetype_add_entity(dst, e);
  for (ComponentId id = 0; id < g_component_count; id++) {
    if (!(old->signature & BIT(id)) || !(dst->signature & BIT(id)))
      continue; /* not present in old, or dropped in new signature */

    size_t sz = g_registry[id].size;
    memcpy((char *)dst->columns[id] + new_row * sz,
           (char *)old->columns[id] + rec->row * sz, sz);
  }

  archetype_remove_entity(w, old, rec->row);
  rec->archetype = dst;
  rec->row = new_row;
  return new_row;
}

static void world_add_component(World *w, Entity e, ComponentId id,
                                const void *data) {
  EntityRecord *rec = &w->records[e.index];
  Archetype *cur = rec->archetype;
  size_t sz = g_registry[id].size;

  if (cur->signature & BIT(id)) { /* already has it, overwrite in place */
    memcpy((char *)cur->columns[id] + rec->row * sz, data, sz);
    return;
  }

  Signature new_sig = cur->signature | BIT(id);
  u32 new_row = move_entity(w, e, new_sig);
  Archetype *dst = w->records[e.index].archetype;
  memcpy((char *)dst->columns[id] + new_row * sz, data, sz);
}

static void world_remove_component(World *w, Entity e, ComponentId id) {
  EntityRecord *rec = &w->records[e.index];
  Archetype *cur = rec->archetype;
  if (!(cur->signature & BIT(id))) {
    return;
  }
  move_entity(w, e, cur->signature & ~BIT(id));
}

static void *world_get_component(World *w, Entity e, ComponentId id) {
  EntityRecord *rec = &w->records[e.index];
  if (!(rec->archetype->signature & BIT(id))) {
    return NULL;
  }
  return (char *)rec->archetype->columns[id] + rec->row * g_registry[id].size;
}

/* vvv [ Query: run a callback over every archetype matching a mask ] vvv */

typedef void (*ArchetypeFn)(World *w, Archetype *a, void *userdata);

static void world_query(World *w, Signature mask, ArchetypeFn fn,
                        void *userdata) {
  for (u32 i = 0; i < w->archetype_count; i++) {
    Archetype *a = w->archetypes[i];
    if (a->count > 0 && (a->signature & mask) == mask) {
      fn(w, a, userdata);
    }
  }
}

#endif
/* vim:set ts=3 sw=2 sts=2 et: */
