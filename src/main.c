#include "./game/entity.h"

#include "engine/ecs/archetypes.h"
#include "engine/event_system/event_bus.h"
#include "engine/event_system/event_queue.h"
#include "game/game.h"
#include "raylib.h"
#include <string.h>

void on_target_reached(const Event *e, void *ctx);

int main(void) {
  Environment env = Environment_CreateDefault();
  Environment_InitWindow(&env);

  register_components();

  EventQueue event_queue = {0};
  EventBus event_bus = {0};
  event_bus_init(&event_bus);
  event_queue_init(&event_queue, &event_bus);

  World world = {0};

  world_init(&world);
  // Entity player = prefab_player(&world);
  Entity slimes[16] = {0};

  for (int i = 0; i < 16; i++) {
    slimes[i] = prefab_slime(&world);
  }

  // typedef void (*EventCallback)(const Event *event, void* ctx);
  event_subscribe(&event_bus, EVENT_ENTITY_TARGET_REACHED, on_target_reached,
                  &world);

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    world_query(
        &world,
        (BIT(Position_id) | BIT(Velocity_id) | BIT(Target_id) | BIT(Speed_id)),
        sys_vel_toward_target_position, &event_queue);

    world_query(&world, (BIT(Position_id) | BIT(Velocity_id)), sys_movement,
                &dt);

    event_queue_flush(&event_queue);
    event_arena_free();

    BeginDrawing();
    ClearBackground(GRAY);
    world_query(&world, (BIT(Position_id) | BIT(BodyDebug_id)), sys_render,
                NULL);

    EndDrawing();
  }

  event_unsubscribe(&event_bus, EVENT_ENTITY_TARGET_REACHED, on_target_reached);
}

void on_target_reached(const Event *e, void *ctx) {
  World *w = ctx;

  Target *target = world_get_component(
      w, e->data.entity_target_reached.current_entity, Target_id);

  Position *target_pos = world_get_component(
      w, e->data.entity_target_reached.target_entity, Position_id);

  float rand_x = (target_pos->x) + (GetRandomValue(0, 1) ? -25 : 25);
  float rand_y = (target_pos->y) + (GetRandomValue(0, 1) ? -25 : 25);

  rand_x = rand_x > 800 ? (rand_x - 50) : rand_x;
  rand_x = rand_x < 0 ? (rand_x + 50) : rand_x;

  rand_y = rand_y > 600 ? (rand_y - 50) : rand_y;
  rand_y = rand_y < 0 ? (rand_y + 50) : rand_y;

  Position new_pos = {.x = rand_x, .y = rand_y};
  Target new_target = {.entity = target->entity,
                       .reached = false,
                       .reached_threshold = target->reached_threshold};

  memcpy(target_pos, &new_pos, sizeof(Position));
  memcpy(target, &new_target, sizeof(Target));
}

/* vim:set ts=3 sw=2 sts=2 et: */
