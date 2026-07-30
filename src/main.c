// #include <stdio.h>

#include "./engine/ui/ui.h"
#include "./game/entity/entity.h" // IWYU pragma: keep
#include "./platform/window.h"
#include "engine/event_system/event_queue.h"
// #include "engine/event_system/events.h"
#include "game/entity/components.h"

#include "./game/event_handlers.h"

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

  #define SLIME_COUNT 32
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

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();
    character_data_ctx.dt = dt;

    /* velocity toward target */
    world_query(
        &world,
        (BIT(Position_id) | BIT(Velocity_id) | BIT(Target_id) | BIT(Speed_id)),
        sys_vel_toward_target_position, &event_queue);

    /* collision check */
    world_query(&world, (BIT(Position_id) | BIT(Collider_id)), sys_collision,
                NULL);

    /* movement */
    world_query(&world, (BIT(Position_id) | BIT(Velocity_id)), sys_movement,
                &dt);

    world_query(&world, (BIT(CharacterData_id)) , sys_character_stats,
                &character_data_ctx);

    /* selection collection */
    world_query(&world, (BIT(Position_id) | BIT(Selectable_id)),
                sys_selectables, &selectable_ctx);

    /* selection priority filter */
    selectables_resolve(&world, &selectable_ctx);

    event_queue_flush(&event_queue);

    BeginDrawing();
    ClearBackground(GRAY);
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

    EndDrawing();
  }

  game_handler_unregister(&event_bus);
}

/* vim:set ts=3 sw=2 sts=2 et: */
