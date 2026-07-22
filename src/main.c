#include "./game/entity.h"

#include "engine/ecs/archetypes.h"
#include "game/game.h"
#include "raylib.h"

int main(void) {
  Environment env = Environment_CreateDefault();
  Environment_InitWindow(&env);

  register_components();

  World world = {0};

  world_init(&world);
  // Entity player = prefab_player(&world);
  Entity slimes[8] = {0};

  for (int i = 0; i < 8; i++) {
    slimes[i] = prefab_slime(&world);
  }

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    world_query(
        &world,
        (BIT(Position_id) | BIT(Velocity_id) | BIT(Target_id) | BIT(Speed_id)),
        sys_vel_toward_target_position, NULL);

    world_query(&world, (BIT(Position_id) | BIT(Velocity_id)), sys_movement,
                &dt);

    BeginDrawing();
    ClearBackground(GRAY);
    world_query(&world, BIT(Position_id), sys_render, NULL);

    EndDrawing();
  }
}

/* vim:set ts=3 sw=2 sts=2 et: */
