#include "./game/entity.h"

#include "game.h"
#include "raylib.h"

int main(void) {
  Environment env = Environment_CreateDefault();
  Environment_InitWindow(&env);

  register_components();

  World world = {0};

  world_init(&world);
  Entity player = prefab_player(&world);

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    world_query(&world, (BIT(Position_id) | BIT(Velocity_id)), sys_movement,
                &dt);

    BeginDrawing();
    ClearBackground(GRAY);
    world_query(&world, BIT(Position_id), sys_render, NULL);

    EndDrawing();
  }

  world_compoent_data_free(&world);
}

/* vim:set ts=3 sw=2 sts=2 et: */
