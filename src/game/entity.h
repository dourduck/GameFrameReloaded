#ifndef ENTITY_H
#define ENTITY_H

#include <stdint.h>
#include <stdio.h>
#define ARENA_IMPLEMENTATION
#include "./../engine/arena/arena.h"
#include "./../engine/ecs/archetypes.h"
#include "./../engine/event_system/event_queue.h"
#include "./../engine/event_system/events.h"

#include "raylib.h"
#include <assert.h>
#include <float.h>
#include <math.h>
#include <stddef.h>

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

typedef struct {
  float x;
  float y;
} Position;

typedef struct {
  float dx;
  float dy;
} Velocity;

typedef struct {
  int hp;
} Health;

typedef struct {
  int speed;
} Speed;

typedef struct {
  Color color;
  float radius;
} BodyDebug;

typedef struct {
  int state;
  int next_state_table[8][8]; // [transition_event][current_state] -> next_state
} StateMachine;

typedef struct {
  Entity entity;
  bool reached;
  float reached_threshold;
} Target;

typedef struct {
  float radius;
} Collider;

DECLARE_COMPONENT_ID(Position);
DECLARE_COMPONENT_ID(Velocity);
DECLARE_COMPONENT_ID(Health);
DECLARE_COMPONENT_ID(Speed);
DECLARE_COMPONENT_ID(StateMachine);
DECLARE_COMPONENT_ID(Target);
DECLARE_COMPONENT_ID(BodyDebug);
DECLARE_COMPONENT_ID(Collider);

static void register_components() {
  REGISTER(Position);
  REGISTER(Velocity);
  REGISTER(Health);
  REGISTER(Speed);
  REGISTER(StateMachine);
  REGISTER(Target);
  REGISTER(BodyDebug);
  REGISTER(Collider);
}

/* Set the entity velocity based on the direction to given point */
static void sys_vel_toward_target_position(World *w, Archetype *a,
                                           void *userdata) {
  (void)w;
  EventQueue *event_queue = userdata;

  Target *targets = archetype_column(a, Target_id);
  Velocity *velocities = archetype_column(a, Velocity_id);
  Position *positions = archetype_column(a, Position_id);
  Speed *speeds = archetype_column(a, Speed_id);

  for (uint32_t i = 0; i < a->count; i++) {
    if (targets[i].reached) {
      continue;
    }

    /* calc direction to target */
    Position *target_pos =
        world_get_component(w, targets[i].entity, Position_id);
    float dx = (target_pos->x) - positions[i].x;
    float dy = (target_pos->y) - positions[i].y;

    /* calc magnitude of dir */
    float dxx = dx * dx;
    float dyy = dy * dy;
    float mag = sqrt(dxx + dyy);

    if (mag <= targets[i].reached_threshold && !targets[i].reached) {

      Entity current_entity = a->entities[i];
      Entity target_entity = targets[i].entity;

      Event *e = event_arena_alloc(sizeof(Event));
      e->type = EVENT_ENTITY_TARGET_REACHED;
      e->data.entity_target_reached.current_entity = current_entity;
      e->data.entity_target_reached.target_entity = target_entity;
      event_queue_push(event_queue, e);

      targets[i].reached = true;
      velocities[i].dx = 0;
      velocities[i].dy = 0;
      continue;
    }

    /* normalize direction */
    float nx = dx / mag;
    float ny = dy / mag;

    /* Apply speed to normalized direction */
    float spd = speeds[i].speed;

    /* Assign to velocity */
    velocities[i].dx = nx * spd;
    velocities[i].dy = ny * spd;
  }
}

/* [NAIVE IMPLEMENTATION] */
static void sys_collision(World *w, Archetype *a, void *userdata) {
  (void)w;
  (void)userdata;

  Position *positions = archetype_column(a, Position_id);
  Collider *colliders = archetype_column(a, Collider_id);
  Velocity *velocities = archetype_column(a, Velocity_id);

  for (uint32_t i = 0; i < a->count; i++) {
    float ix = positions[i].x;
    float iy = positions[i].y;

    // float min_mag = FLT_MAX;
    // int32_t min_j_idx = -1;
    // float min_dir_x = 0;
    // float min_dir_y = 0;
    // float min_radius = 0;

    for (uint32_t j = 0; j < a->count; j++) {
      if (j == i) {
        continue;
      }

      float jx = positions[j].x;
      float jy = positions[j].y;

      float dir_x = jx - ix;
      float dir_y = jy - iy;

      float mag = sqrt((dir_x * dir_x) + (dir_y * dir_y));

      float collider_radius = (colliders[i].radius * 1.25f);

      if (mag < collider_radius) {
        if (mag > 0.0001f) {
          /* normalize and invert then scale to correct position*/
          positions[i].x += -(dir_x / mag) * collider_radius * 0.1f;
          positions[i].y += -(dir_y / mag) * collider_radius * 0.1f;

          positions[j].x += (dir_x / mag) * collider_radius * 0.1f;
          positions[j].y += (dir_y / mag) * collider_radius * 0.1f;

          // velocities[i].dx += (dir_x / mag) * 10;
          // velocities[i].dy += (dir_x / mag) * 10;
          //
          // velocities[j].dx += -(dir_x / mag) * 10;
          // velocities[j].dy += -(dir_x / mag) * 10;
        } else {
          positions[i].x +=
              (colliders[i].radius) * (GetRandomValue(0, 1) ? 1 : -1);
          positions[i].y +=
              (colliders[i].radius) * (GetRandomValue(0, 1) ? 1 : -1);

          // positions[i].x += (collider_radius_sum*2);
        }

        // if (mag < min_mag) {
        //   min_mag = mag;
        //   min_dir_x = dir_x;
        //   min_dir_y = dir_y;
        //   min_radius = colliders[j].radius;
        //   // min_j_idx = j;
        // }
      }
      // TODO: Figure out why this is failing
      // assert(min_j_idx > -1);
    }
  }
}

static void sys_movement(World *w, Archetype *a, void *userdata) {
  (void)w;
  float dt = *(float *)userdata;
  Position *positions = archetype_column(a, Position_id);
  Velocity *velocities = archetype_column(a, Velocity_id);
  for (uint32_t i = 0; i < a->count; i++) {
    positions[i].x += velocities[i].dx * dt;
    positions[i].y += velocities[i].dy * dt;
  }
}

static void sys_render(World *w, Archetype *a, void *userdata) {
  (void)w;
  (void)userdata;

  Position *positions = archetype_column(a, Position_id);
  BodyDebug *debug_bodies = archetype_column(a, BodyDebug_id);

  for (uint32_t i = 0; i < a->count; i++) {
    DrawCircle(positions[i].x, positions[i].y, debug_bodies[i].radius,
               debug_bodies[i].color);
  }
}

static Entity prefab_player(World *world) {
  Entity player = entity_create(world);

  /* component contents get copied */
  Position position = (Position){.x = 0, .y = 0};
  Velocity velocity = (Velocity){.dx = 100.0f, .dy = 50.0f};
  Health health = (Health){.hp = 100};

  world_add_component(world, player, Position_id, &position);
  world_add_component(world, player, Velocity_id, &velocity);
  world_add_component(world, player, Health_id, &health);

  return player;
}

typedef enum {
  // FSM_S_WANDERING,
  F_S_JOY,
  F_S_SLEEP,
  F_S_HUNT,
  F_S_EAT,

  F_S_PLAY,
  F_S_WALLOW,
  F_S_FIGHTING,
  F_S_FLEE,

} F_State;

typedef enum {
  F_E_FOOD,
  F_E_FUN,
  F_E_PAIN,
  F_E_ANGER,

  F_E_BORED,
  F_E_HUNGRY,
  F_E_TIRED,
  F_E_ATE,

  F_E_DEATH,
} F_Event;

static Entity prefab_target(World *world, float x, float y) {
  Entity target = entity_create(world);

  Position position = (Position){.x = x, .y = y};
  BodyDebug body_debug = (BodyDebug){.color = YELLOW, .radius = 8};

  world_add_component(world, target, Position_id, &position);
  world_add_component(world, target, BodyDebug_id, &body_debug);

  return target;
}

static Entity prefab_slime(World *world) {
  Entity slime = entity_create(world);

  Position position = (Position){.x = 100, .y = 100};
  Velocity velocity = (Velocity){.dx = 0.0f, .dy = 0.0f};
  Speed speed = (Speed){.speed = 100.0f};
  // Health health = (Health){.hp = 20};
  BodyDebug body_debug = (BodyDebug){.color = DARKGREEN, .radius = 16};

  Collider collider = (Collider){.radius = 16};

  float target_x = GetRandomValue(200, 600);
  float target_y = GetRandomValue(150, 450);
  Entity target_entity = prefab_target(world, target_x, target_y);

  Position *target_position =
      world_get_component(world, target_entity, Position_id);

  assert(target_position);

  Target target = (Target){
      .entity = target_entity,
      .reached = false,
      .reached_threshold = 10.0f,
  };

  world_add_component(world, slime, Position_id, &position);
  world_add_component(world, slime, Velocity_id, &velocity);
  world_add_component(world, slime, Speed_id, &speed);
  world_add_component(world, slime, BodyDebug_id, &body_debug);

  world_add_component(world, slime, Target_id, &target);

  world_add_component(world, slime, Collider_id, &collider);

  // world_add_component(world, slime, Health_id, &health);

  // int state_table[8][8] = {
  //     /* happy | sleeping | hunting | eating | playing | wallowing | fighting
  //     |
  //        fleeing */
  //     {F_S_JOY, F_S_SLEEP, F_S_EAT, F_S_EAT, F_S_PLAY, F_S_EAT, F_S_FIGHTING,
  //      F_S_FLEE}, /* found food */
  //     {F_S_JOY, F_S_SLEEP, F_S_HUNT, F_S_EAT, F_S_PLAY, F_S_JOY,
  //     F_S_FIGHTING,
  //      F_S_FLEE}, /* having fun */
  //     {F_S_FLEE, F_S_FLEE, F_S_FIGHTING, F_S_FIGHTING, F_S_FLEE, F_S_FLEE,
  //      F_S_FLEE, F_S_FLEE}, /* injured */
  //     {F_S_JOY, F_S_SLEEP, F_S_HUNT, F_S_EAT, F_S_PLAY, F_S_WALLOW,
  //      F_S_FIGHTING, F_S_FLEE}, /* angered */
  //     {F_S_WALLOW, F_S_SLEEP, F_S_HUNT, F_S_EAT, F_S_PLAY, F_S_WALLOW,
  //      F_S_FIGHTING, F_S_FLEE}, /* bored */
  //     {F_S_HUNT, F_S_SLEEP, F_S_HUNT, F_S_EAT, F_S_HUNT, F_S_HUNT,
  //     F_S_FIGHTING,
  //      F_S_FLEE}, /* hungry */
  //     {F_S_SLEEP, F_S_SLEEP, F_S_HUNT, F_S_EAT, F_S_SLEEP, F_S_SLEEP,
  //      F_S_FIGHTING, F_S_FLEE}, /* tired */
  //     {F_S_JOY, F_S_SLEEP, F_S_JOY, F_S_JOY, F_S_PLAY, F_S_WALLOW,
  //     F_S_FIGHTING,
  //      F_S_FLEE}, /* satiated */
  // };
  //
  // StateMachine state_machine = {0};

  // for (int i = 0; i < 8; i++) {
  //   for (int j = 0; j < 8; j++) {
  //     state_machine.next_state_table[i][j] = state_table[i][j];
  //   }
  // }

  // world_add_component(world, slime, StateMachine_id, &state_machine);

  return slime;
}
#endif
/* vim:set ts=3 sw=2 sts=2 et: */
