#include "./game/entity.h"

#include "engine/ecs/archetypes.h"
#include "engine/event_system/event_bus.h"
#include "engine/event_system/event_queue.h"
#include "game/game.h"
#include "raylib.h"

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
    world_query(&world, BIT(Position_id), sys_render, NULL);

    EndDrawing();
  }
}

/* vim:set ts=3 sw=2 sts=2 et: */
