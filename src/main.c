#include "./engine/ui/ui.h"
#include "./game/entity/component_systems.h"
#include "./game/event_handlers.h"
#include "./platform/window.h"
#include "./resources.h"
#include "./rkeys.h"
#include "engine/ecs/archetypes.h"
#include "game/entity/components.h"
#include "game/entity/prefabs.h"
#include "game/entity/spatial_hash_system.h"
#include "raylib.h"
#include <stdio.h>

#define CHICKEN_COUNT (1024 * 8)

typedef struct {
  float physics_tick_rate;
  float physics_timer;
} PhysicsUpdateCtx;

typedef struct {
  Font debug_font;
  Color debug_text_color;
} DebugCtx;

typedef struct {
  World world;
  EventBus event_bus;
  EventQueue event_queue;
  StatMenuCtx stat_menu_ctx;
  SelectableCtx selectable_ctx;
  CharacterDataCtx character_data_ctx;
  GameHandlerCtx game_handler_ctx;
  SpatialGrid spatial_grid;
  SpatialHashCtx spatial_ctx;
  ResourceHashTable *resource_hashtable;
  DebugCtx debug_ctx;
  PhysicsUpdateCtx physics_update_ctx;
  Entity chickens[CHICKEN_COUNT];
} GameCtx;

void spawn_chickens(GameCtx *game_ctx) {
  Texture chicken_texture =
      resource_hashtable_get_item(game_ctx->resource_hashtable,
                                  rkey_texture_chicken)
          ->data.texture;
  Rectangle chicken_tex_src = {.x = 0,
                               .y = 0,
                               .width = chicken_texture.width,
                               .height = chicken_texture.height};
  Vector2 chicken_tex_scale = {.x = 2, .y = 2};

  int margin_x = 100 - (chicken_texture.width * chicken_tex_scale.x * 0.5);
  int margin_y = 100 - (chicken_texture.height * chicken_tex_scale.y * 0.5);
  int screen_w = GetScreenWidth();
  int screen_h = GetScreenHeight();

  for (int i = 0; i < CHICKEN_COUNT; i++) {
    int rand_x = GetRandomValue(margin_x, (screen_w - margin_x));
    int rand_y = GetRandomValue(0, (screen_h - margin_y));
    game_ctx->chickens[i] =
        prefab_chicken(&game_ctx->world, rand_x, rand_y, chicken_texture,
                       chicken_tex_src, chicken_tex_scale);
  }
}

void start(GameCtx *game_ctx) {
  Environment env = Environment_CreateDefault();
  Environment_InitWindow(&env);

  game_ctx->resource_hashtable = resources_init();

  ui_init();

  register_components();

  event_bus_init(&game_ctx->event_bus);
  event_queue_init(&game_ctx->event_queue, &game_ctx->event_bus);

  world_init(&game_ctx->world);

  game_ctx->debug_ctx = (DebugCtx){
      .debug_font = (resource_hashtable_get_item((game_ctx->resource_hashtable),
                                                 rkey_font_open_dyslexic)
                         ->data.font),
      .debug_text_color = (Color){.r = 200, .g = 200, .b = 32, .a = 255},
  };

  prefab_ui_stat_menu(&game_ctx->world, &game_ctx->stat_menu_ctx);

  selectable_ctx_init(&game_ctx->event_queue, &game_ctx->selectable_ctx);

  game_ctx->character_data_ctx.event_queue = &game_ctx->event_queue;

  game_ctx->game_handler_ctx.world = &game_ctx->world;
  game_ctx->game_handler_ctx.stat_menu_ctx = &game_ctx->stat_menu_ctx;

  game_handler_register(&game_ctx->event_bus, &game_ctx->game_handler_ctx);

  game_ctx->spatial_ctx.grid = &game_ctx->spatial_grid;

  game_ctx->physics_update_ctx.physics_tick_rate = 1.0f / 60.0f;
  game_ctx->physics_update_ctx.physics_timer = 0.0f;

  memset(game_ctx->chickens, 0, sizeof(game_ctx->chickens));
  spawn_chickens(game_ctx);
}

void update(GameCtx *game_ctx, float dt) {
  World *world = &game_ctx->world;
  EventQueue *event_queue = &game_ctx->event_queue;
  CharacterDataCtx *character_data_ctx = &game_ctx->character_data_ctx;
  SelectableCtx *selectable_ctx = &game_ctx->selectable_ctx;

  character_data_ctx->dt = dt;

  world_query(world, (BIT(CharacterData_id)), sys_character_stats,
              character_data_ctx);

  /* selection collection */
  world_query(world, (BIT(Position_id) | BIT(Selectable_id)), sys_selectables,
              selectable_ctx);

  /* selection priority filter */
  selectables_resolve(world, selectable_ctx);

  event_queue_flush(event_queue);
}

void update_physics(GameCtx *game_ctx, float dt) {
  PhysicsUpdateCtx *physics_update_ctx = &game_ctx->physics_update_ctx;
  physics_update_ctx->physics_timer += dt;

  if (physics_update_ctx->physics_timer >=
      physics_update_ctx->physics_tick_rate) {
    World *world = &game_ctx->world;
    EventQueue *event_queue = &game_ctx->event_queue;
    SpatialGrid *spatial_grid = &game_ctx->spatial_grid;
    SpatialHashCtx *spatial_ctx = &game_ctx->spatial_ctx;

    world_query(world, (BIT(Position_id) | BIT(Velocity_id)), sys_movement,
                &dt);

    /* velocity toward target */
    world_query(
        world,
        (BIT(Position_id) | BIT(Velocity_id) | BIT(Target_id) | BIT(Speed_id)),
        sys_vel_toward_target_position, event_queue);

    spatial_hash_clear(spatial_grid);
    world_query(world, (BIT(Position_id)), spatial_hash_rebuild, spatial_grid);

    /* collision check */
    world_query(world, (BIT(Position_id) | BIT(Collider_id)), sys_collision,
                spatial_ctx);

    game_ctx->physics_update_ctx.physics_timer = 0;
  }
}

void update_render(GameCtx *game_ctx) {
  world_query(&game_ctx->world, (BIT(Position_id) | BIT(BodyDebug_id)),
              sys_render, NULL);

  world_query(&game_ctx->world, (BIT(Position_id) | BIT(Sprite_id)),
              sys_render_sprites, NULL);
}

void update_render_ui(GameCtx *game_ctx) {
  world_query(&game_ctx->world,
              (BIT(Position_id) | BIT(Selectable_id) | BIT(Panel_id)),
              sys_ui_panels, NULL);

  world_query(&game_ctx->world,
              (BIT(Position_id) | BIT(Selectable_id) | BIT(Button_id)),
              sys_ui_buttons, NULL);

  world_query(&game_ctx->world, (BIT(Position_id) | BIT(TextComponent_id)),
              sys_ui_text, NULL);

  world_query(&game_ctx->world, (BIT(Position_id) | BIT(Selectable_id)),
              sys_render_selections, NULL);

  world_query(&game_ctx->world, (BIT(Position_id) | BIT(Cursor_id)),
              sys_render_cursor, NULL);

  DrawTextEx(game_ctx->debug_ctx.debug_font, TextFormat("FPS: %d", GetFPS()),
             (Vector2){10, 10}, 36, 0, game_ctx->debug_ctx.debug_text_color);
}

void end(GameCtx *game_ctx) {
  game_handler_unregister(&game_ctx->event_bus);

  resource_data_unload_all(game_ctx->resource_hashtable);
  resource_hashtable_free(game_ctx->resource_hashtable);
}

int main(void) {
  GameCtx game_ctx = {0};

  start(&game_ctx);

  // Entity chickens[CHICKEN_COUNT] = {0};
  // spawn_chickens(&game_ctx, chickens);

  Vector2 mos_pos = GetMousePosition();
  Entity cursor = prefab_cursor(&game_ctx.world, mos_pos.x, mos_pos.y);

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    update_physics(&game_ctx, dt);
    update(&game_ctx, dt);

    BeginDrawing();
    ClearBackground(DARKPURPLE);

    update_render(&game_ctx);
    update_render_ui(&game_ctx);

    EndDrawing();
  }

  end(&game_ctx);
}

/* vim:set ts=3 sw=2 sts=2 et: */
