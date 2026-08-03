#ifndef SPATIAL_HASH_SYSTEM_H
#define SPATIAL_HASH_SYSTEM_H

#include "../../core/types.h"
#include "../../engine/ecs/archetypes.h"
#include "./components.h"

#include <math.h>
#include <stddef.h>
#include <string.h>

#define CELL_SIZE 128.0 /* What is the average size of an entity? */
#define TABLE_SIZE 4096

typedef struct HashNode {
  Entity entity;
  struct HashNode *next;
} HashNode;

typedef struct {
  HashNode *buckets[TABLE_SIZE];
  HashNode pool[MAX_ENTITIES];
  int pool_used;
} SpatialGrid;

typedef struct {
  SpatialGrid *grid;
  World *world;
} SpatialHashCtx;

static void world_to_cell(float pos_x, float pos_y, i32 *cx, i32 *cy) {
  *cx = (i32)floorf(pos_x / CELL_SIZE);
  *cy = (i32)floorf(pos_y / CELL_SIZE);
}

static u32 hash_cell(i32 cx, i32 cy) {
  u32 prime_a = 73856093u;
  u32 prime_b = 19349663u;
  u32 h = ((u32)cx * prime_a) ^ ((u32)cy * prime_b);

  return (h % TABLE_SIZE);
}

static void spatial_hash_clear(SpatialGrid *grid) {
  memset(grid->buckets, 0, sizeof(grid->buckets));
  grid->pool_used = 0;
}

static void spatial_hash_insert(SpatialGrid *grid, Position *entity_position,
                                Entity entity) {
  i32 cx;
  i32 cy;
  world_to_cell(entity_position->x, entity_position->y, &cx, &cy);
  u32 bucket = hash_cell(cx, cy);

  HashNode *node = &grid->pool[grid->pool_used++];
  node->entity = entity;
  node->next = grid->buckets[bucket];
  grid->buckets[bucket] = node;
}

static void spatial_hash_rebuild(World *w, Archetype *a, void *userdata) {
  SpatialGrid *grid = (SpatialGrid *)userdata;
  Position *positions = archetype_column(a, Position_id);
  spatial_hash_clear(grid);
  for (u32 i = 0; i < a->count; i++) {
    spatial_hash_insert(grid, &positions[i], a->entities[i]);
  }
}

/* Gets entities within radius 
 * returns the amount of entities found. */
static i32 spatial_hash_query(World *world, SpatialGrid *grid,
                              Position current_pos, float radius,
                              Entity *out_entities, i32 max_out) {
  i32 found = 0;
  float r2 = radius * radius;

  i32 min_cx;
  i32 min_cy;
  i32 max_cx;
  i32 max_cy;

  world_to_cell(current_pos.x - radius, current_pos.y - radius, &min_cx,
                &min_cy);
  world_to_cell(current_pos.x + radius, current_pos.y + radius, &max_cx,
                &max_cy);

  for (i32 cy = min_cy; cy <= max_cy; cy++) {
    for (i32 cx = min_cx; cx <= max_cx; cx++) {

      u32 bucket = hash_cell(cx, cy);

      /* Narrow phase */
      /* Pretty sure this doesn't filter out the current entity */
      for (HashNode *node = grid->buckets[bucket]; node; node = node->next) {
        Position *pos = world_get_component(world, node->entity, Position_id);
        float dx = pos->x - current_pos.x;
        float dy = pos->y - current_pos.y;

        if (dx * dx + dy * dy <= r2) {
          if (found < max_out) {
            out_entities[found] = node->entity;
            // Selectable *sel = world_get_component(world, node->entity, Selectable_id);
            // if(sel) {
            //   sel->selected = true;
            // }
          }

          found++;
        }
      }
    }
  }

  return found;
}

// static void sys_movement(World *w, Archetype *a, void *userdata) {
//   (void)w;
//   float dt = *(float *)userdata;
//   Position *positions = archetype_column(a, Position_id);
//   Velocity *velocities = archetype_column(a, Velocity_id);
//   for (uint32_t i = 0; i < a->count; i++) {
//     positions[i].x += velocities[i].dx * dt;
//     positions[i].y += velocities[i].dy * dt;
//   }
// }


/*
 * TODO:
 * - Debug Grid
 * - Debug Rays
 * - Highlight Entities that are within a given radius from the cursor
 *
 * */


#endif
/* vim:set ts=3 sw=2 sts=2 et: */
