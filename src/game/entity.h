#ifndef ENTITY_H
#define ENTITY_H

#include "./../engine/ecs/archetypes.h"
#include "raylib.h"
#include <math.h>
// #define STB_DS_IMPLEMENTATION
// #include "./../stb_ds.h"

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
  int state;
  int next_state_table[8][8]; // [transition_event][current_state] -> next_state
} StateMachine;

typedef struct {
  Position target_pos;
} Target;

DECLARE_COMPONENT_ID(Position);
DECLARE_COMPONENT_ID(Velocity);
DECLARE_COMPONENT_ID(Health);
DECLARE_COMPONENT_ID(Speed);
DECLARE_COMPONENT_ID(StateMachine);
DECLARE_COMPONENT_ID(Target);

static void register_components() {
  REGISTER(Position);
  REGISTER(Velocity);
  REGISTER(Health);
  REGISTER(Speed);
  REGISTER(StateMachine);
  REGISTER(Target);
}

typedef struct {
  float vx;
  float vy;
} Vec2;

static Vec2 direction_from_to(Position from, Position to) {
  float dx = to.x - from.x;
  float dy = to.y - from.y;
  return (Vec2){.vx = dx, .vy = dy};
}

static Vec2 direction_normalized(Vec2 dir) {
  float dx = dir.vx;
  float dy = dir.vy;

  float dxx = dx * dx;
  float dyy = dy * dy;

  float mag = sqrt(dxx + dyy);

  float nx = dir.vx / mag;
  float ny = dir.vy / mag;

  return (Vec2){.vx = nx, .vy = ny};
}

/* Mutate in place */
static void direction_normalize(Vec2 *out) {
  float dx = out->vx;
  float dy = out->vy;

  float dxx = dx * dx;
  float dyy = dy * dy;

  float mag = sqrt(dxx + dyy);

  float nx = dx / mag;
  float ny = dy / mag;

  out->vx = nx;
  out->vy = ny;
}

/* Set the entity velocity based on the direction to given point */
static void sys_vel_to_target_position(World *w, Archetype *a, void *userdata) {
  (void)w;
  (void)userdata;
  Target *targets = archetype_column(a, Target_id);
  Velocity *velocities = archetype_column(a, Velocity_id);
  Position *positions = archetype_column(a, Velocity_id);
  for (uint32_t i = 0; i < a->count; i++) {
    targets[i].target_pos;
    velocities[i].dx;
    velocities[i].dy;
    positions[i].x;
    positions[i].y;
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
  for (uint32_t i = 0; i < a->count; i++) {
    DrawCircle(positions[i].x, positions[i].y, 16, GREEN);
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

static Entity prefab_slime(World *world) {
  Entity slime = entity_create(world);

  Position position = (Position){.x = 0, .y = 0};
  Velocity velocity = (Velocity){.dx = 0.0f, .dy = 0.0f};
  Health health = (Health){.hp = 20};

  int state_table[8][8] = {
      /* happy | sleeping | hunting | eating | playing | wallowing | fighting |
         fleeing */
      {F_S_JOY, F_S_SLEEP, F_S_EAT, F_S_EAT, F_S_PLAY, F_S_EAT, F_S_FIGHTING,
       F_S_FLEE}, /* found food */
      {F_S_JOY, F_S_SLEEP, F_S_HUNT, F_S_EAT, F_S_PLAY, F_S_JOY, F_S_FIGHTING,
       F_S_FLEE}, /* having fun */
      {F_S_FLEE, F_S_FLEE, F_S_FIGHTING, F_S_FIGHTING, F_S_FLEE, F_S_FLEE,
       F_S_FLEE, F_S_FLEE}, /* injured */
      {F_S_JOY, F_S_SLEEP, F_S_HUNT, F_S_EAT, F_S_PLAY, F_S_WALLOW,
       F_S_FIGHTING, F_S_FLEE}, /* angered */
      {F_S_WALLOW, F_S_SLEEP, F_S_HUNT, F_S_EAT, F_S_PLAY, F_S_WALLOW,
       F_S_FIGHTING, F_S_FLEE}, /* bored */
      {F_S_HUNT, F_S_SLEEP, F_S_HUNT, F_S_EAT, F_S_HUNT, F_S_HUNT, F_S_FIGHTING,
       F_S_FLEE}, /* hungry */
      {F_S_SLEEP, F_S_SLEEP, F_S_HUNT, F_S_EAT, F_S_SLEEP, F_S_SLEEP,
       F_S_FIGHTING, F_S_FLEE}, /* tired */
      {F_S_JOY, F_S_SLEEP, F_S_JOY, F_S_JOY, F_S_PLAY, F_S_WALLOW, F_S_FIGHTING,
       F_S_FLEE}, /* satiated */
  };

  StateMachine state_machine = {0};

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      state_machine.next_state_table[i][j] = state_table[i][j];
    }
  }

  world_add_component(world, slime, Position_id, &position);
  world_add_component(world, slime, Velocity_id, &velocity);
  world_add_component(world, slime, Health_id, &health);
  world_add_component(world, slime, StateMachine_id, &state_machine);

  return slime;
}
#endif
/* vim:set ts=3 sw=2 sts=2 et: */
