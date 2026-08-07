#include "./engine/ui/ui.h"
#include "./game/entity/component_systems.h"
#include "./game/event_handlers.h"
#include "./platform/window.h"
#include "./resources.h"
#include "game/entity/spatial_hash_system.h"
#include "raylib.h"
#include <stdio.h>

int main(void) {
  Environment env = Environment_CreateDefault();
  Environment_InitWindow(&env);

  ui_init();
  register_components();

  EventQueue event_queue = {0};
  EventBus event_bus = {0};
  event_bus_init(&event_bus);
  event_queue_init(&event_queue, &event_bus);

  World world = {0};
  world_init(&world);

#define SLIME_COUNT 2000
  Entity slimes[SLIME_COUNT] = {0};

  for (int i = 0; i < SLIME_COUNT; i++) {
    slimes[i] = prefab_slime(&world);
  }

  StatMenuCtx stat_menu_ctx = prefab_ui_stat_menu(&world);
  SelectableCtx selectable_ctx = selectable_ctx_init(&event_queue);

  CharacterDataCtx character_data_ctx = {0};
  character_data_ctx.event_queue = &event_queue;

  GameHandlerCtx game_handler_ctx = {
      .world = &world,
      .stat_menu_ctx = &stat_menu_ctx,
  };

  game_handler_register(&event_bus, &game_handler_ctx);

  Vector2 mos_pos = GetMousePosition();
  Entity cursor = prefab_cursor(&world, mos_pos.x, mos_pos.y);

  SpatialGrid spatial_grid;
  SpatialHashCtx spatial_ctx;
  spatial_ctx.grid = &spatial_grid;
  spatial_ctx.out_entities;

  ResourceHashTable *resource_hashtable = resource_hashtable_create();

  // DECLARE_KEY_TEXTURE(chicken);
  // ResourceData *rtexture_chicken = resource_data_create(
  //     resource_hashtable,
  //     (ResourceParams){.rkind = RESOURCE_KIND_TEXTURE,
  //                      .rkey = rkey_texture_chicken,
  //                      .rpath = "./assets/Chicken_placeholder.png"});
  //
  // DECLARE_KEY_TEXTURE(egg);
  // ResourceData *resource_texture_egg = resource_data_create(
  //     resource_hashtable,
  //     (ResourceParams){.rkind = RESOURCE_KIND_TEXTURE,
  //                      .rkey = rkey_texture_egg,
  //                      .rpath = "./assets/Chicken_egg.png"});

  /* vvv [ [DEBUG] ] vvv */

  DECLARE_KEY_FONT(open_dyslexic);
  ResourceData *rfont_open_dyslexic = resource_data_create(
      resource_hashtable, (ResourceParams){
                              .rkind = RESOURCE_KIND_FONT,
                              .rkey = rkey_font_open_dyslexic,
                              .rpath = "./assets/font/OpenDyslexic-Regular.otf",
                          });

  Font font =
      resource_hashtable_get_item(resource_hashtable, rkey_font_open_dyslexic)
          ->data.font;

  Color debug_text_color = (Color){.r = 200, .g = 200, .b = 32, .a = 255};

  /* ^^^ [ [DEBUG] ] ^^^ */

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();
    character_data_ctx.dt = dt;

    /* velocity toward target */
    world_query(
        &world,
        (BIT(Position_id) | BIT(Velocity_id) | BIT(Target_id) | BIT(Speed_id)),
        sys_vel_toward_target_position, &event_queue);

    spatial_hash_clear(&spatial_grid);
    world_query(&world, (BIT(Position_id)), spatial_hash_rebuild,
                &spatial_grid);

    /* collision check */
    world_query(&world, (BIT(Position_id) | BIT(Collider_id)), sys_collision,
                &spatial_ctx);

    /* movement */
    world_query(&world, (BIT(Position_id) | BIT(Velocity_id)), sys_movement,
                &dt);

    world_query(&world, (BIT(CharacterData_id)), sys_character_stats,
                &character_data_ctx);

    /* selection collection */
    world_query(&world, (BIT(Position_id) | BIT(Selectable_id)),
                sys_selectables, &selectable_ctx);

    /* selection priority filter */
    selectables_resolve(&world, &selectable_ctx);

    event_queue_flush(&event_queue);

    BeginDrawing();
    ClearBackground(DARKPURPLE);

    world_query(&world, (BIT(Position_id) | BIT(BodyDebug_id)), sys_render,
                NULL);

    world_query(&world, (BIT(Position_id) | BIT(Selectable_id) | BIT(Panel_id)),
                sys_ui_panels, NULL);

    world_query(&world,
                (BIT(Position_id) | BIT(Selectable_id) | BIT(Button_id)),
                sys_ui_buttons, NULL);

    world_query(&world, (BIT(Position_id) | BIT(TextComponent_id)), sys_ui_text,
                NULL);

    world_query(&world, (BIT(Position_id) | BIT(Selectable_id)),
                sys_render_selections, NULL);

    world_query(&world, (BIT(Position_id) | BIT(Cursor_id)), sys_render_cursor,
                NULL);

    DrawTextEx(font, TextFormat("FPS: %d", GetFPS()), (Vector2){10, 10}, 36, 0,
               debug_text_color);

    EndDrawing();
  }

  game_handler_unregister(&event_bus);

  resource_hashtable_free(resource_hashtable);
}

/* vim:set ts=3 sw=2 sts=2 et: */
