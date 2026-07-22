#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./archetypes.h"

// static Arena *arena_component_data_ptr = &arena_component_data;


/* vvv [ Component Registry ] vvv */


/* ^^^ [ Component Registry ] ^^^ */

/* vvv [ Entity ] vvv */


/* ^^^ [ Entity ] ^^^ */

/* vvv [ Archetype: one bucket per unique signature ] vvv
 *
 * SIMPLIFICATION 1: columns[] is now indexed directly by ComponentId
 * (fixed MAX_COMPONENTS slots, NULL where the archetype lacks that
 * component) instead of a compacted array paired with a linear-scan
 * lookup. This removes archetype_column_index() entirely and turns every
 * component access into an O(1) array index rather than an O(component
 * count) scan - simpler *and* faster, since MAX_COMPONENTS is small (32)
 * the wasted pointer slots are negligible.
 */


/* ^^^ [ Archetype: one bucket per unique signature ] ^^^ */

/* vvv [ World ] vvv
 *
 * SIMPLIFICATION 2: EntityManager is folded directly into World. It was
 * never used independently of World, so the extra struct and the
 * w->em.<field> indirection were pure overhead.
 */



/* ^^^ [ Query: run a callback over every archetype matching a mask ] ^^^ */

/* vvv [ Example Components ] vvv */
//
// typedef struct {
//   float x;
//   float y;
// } Position;
//
// typedef struct {
//   float dx;
//   float dy;
// } Velocity;
//
// typedef struct {
//   int hp;
// } Health;
//
// DECLARE_COMPONENT_ID(Position);
// DECLARE_COMPONENT_ID(Velocity);
// DECLARE_COMPONENT_ID(Health);

/* ^^^ [ Example Components ] ^^^ */

/* vvv [ Example System ] vvv */
//
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

/* ^^^ [ Example System ] ^^^ */

/* vvv [ DEMO ] vvv */

// int main(void) {
//   REGISTER(Position);
//   REGISTER(Velocity);
//   REGISTER(Health);
//
//   World world;
//   world_init(&world);
//
// Entity player = entity_create(&world);
// world_add_component(&world, player, Position_id, &(Position){0, 0});
// world_add_component(&world, player, Velocity_id, &(Velocity){1.0f, 0.5f});
// world_add_component(&world, player, Health_id, &(Health){100});
//
//   Entity static_prop = entity_create(&world);
//   world_add_component(&world, static_prop, Position_id, &(Position){10, 10});
//   /* ^^^ no velocity so sys_move skips this ^^^ */
//
//   Entity projectile = entity_create(&world);
//   world_add_component(&world, projectile, Position_id, &(Position){5, 5});
//   world_add_component(&world, projectile, Velocity_id,
//                       &(Velocity){-2.0f, 0.0f});
//
//   printf("archetypes after setup: %u\n", world.archetype_count);
//
//   float dt = 1.0 / 60.0f;
//   for (int frame = 0; frame < 3; frame++) {
//     world_query(&world, BIT(Position_id) | BIT(Velocity_id), sys_movement,
//     &dt);
//   }
//
//   Position *pp = world_get_component(&world, player, Position_id);
//   printf("player position after 3 frames: (%.4f, %.4f)\n", pp->x, pp->y);
//
//   Position *sp = world_get_component(&world, static_prop, Position_id);
//   printf("Static prop position (Unchanged): (%.4f, %.4f)\n", sp->x, sp->y);
//
//   world_remove_component(&world, player, Velocity_id);
//   printf("player has velocity? %s\n",
//          world_get_component(&world, player, Velocity_id) ? "yes" : "no");
//
//   printf("archetypes after remove: %u\n", world.archetype_count);
//
//   entity_destroy(&world, static_prop);
//   printf("static prop alive? %s\n",
//          world_alive(&world, static_prop) ? "yes" : "no");
//
//   return 0;
// }

/* ^^^ [ DEMO ] ^^^ */

/* vim:set ts=3 sw=2 sts=2 et: */
