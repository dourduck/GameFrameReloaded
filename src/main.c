#include <stdio.h>

#include "./engine/ui/ui.h"
#include "./platform/window.h"

void on_target_reached(const Event *e, void *ctx);
void on_entity_selected(const Event *e, void *ctx);

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
  // Entity player = prefab_player(&world);
  Entity slimes[32] = {0};

  for (int i = 0; i < 32; i++) {
    slimes[i] = prefab_slime(&world);
  }

  // Entity button = prefab_ui_button(&world);
  StatMenuCtx statMenuCtx = prefab_ui_stat_menu(&world);

  // typedef void (*EventCallback)(const Event *event, void* ctx);
  event_subscribe(&event_bus, EVENT_ENTITY_TARGET_REACHED, on_target_reached,
                  &world);

  event_subscribe(&event_bus, EVENT_ENTITY_SELECTED, on_entity_selected,
                  &statMenuCtx);

  SelectableCtx selectable_ctx = selectable_ctx_init(&event_queue);

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

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

    /* selection collection */
    world_query(&world, (BIT(Position_id) | BIT(Selectable_id)),
                sys_selectables, &selectable_ctx);
    /* selection priority filter */
    selectables_resolve(&selectable_ctx);

    event_queue_flush(&event_queue);
    event_arena_free();

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

  event_unsubscribe(&event_bus, EVENT_ENTITY_TARGET_REACHED, on_target_reached);
  event_unsubscribe(&event_bus, EVENT_ENTITY_SELECTED, on_entity_selected);
}

void on_entity_selected(const Event *e, void *ctx) {
  StatMenuCtx *stat_menu_ctx = ctx;

  switch (e->data.entity_selection_data.selection_type) {
  case SELECTION_BUTTON:
    printf("Selected BUTTON!: %d\n",
           (e->data.entity_selection_data.entity.index));
    break;
  case SELECTION_CHARACTER:
    printf("Selected CHARACTER!: %d\n",
           (e->data.entity_selection_data.entity.index));

    CharacterData *character_data = world_get_component(
        stat_menu_ctx->world, e->data.entity_selection_data.entity,
        CharacterData_id);

    printf("Character Data: \nHunger: %d\nThirst: %d\n", character_data->hunger,
           character_data->thirst);

    int char_written = sprintf(stat_menu_ctx->buffer, "Hunger: %d\nThirst: %d",
                               character_data->hunger, character_data->thirst);

    stat_menu_ctx->txt_hunger->content = stat_menu_ctx->buffer;

    break;
  }
}

void on_target_reached(const Event *e, void *ctx) {
  World *w = ctx;

  Target *target = world_get_component(
      w, e->data.entity_target_reached.current_entity, Target_id);

  Position *target_pos = world_get_component(
      w, e->data.entity_target_reached.target_entity, Position_id);

  float rand_x = target_pos->x + (GetRandomValue(0, 1) ? -25 : 25);
  float rand_y = target_pos->y + (GetRandomValue(0, 1) ? -25 : 25);

  rand_x = rand_x > 800 ? (rand_x - 50) : rand_x < 0 ? (rand_x + 50) : rand_x;
  rand_y = rand_y > 600 ? (rand_y - 50) : rand_y < 0 ? (rand_y + 50) : rand_y;

  target_pos->x = rand_x;
  target_pos->y = rand_y;

  target->reached = false;
}

/* vim:set ts=3 sw=2 sts=2 et: */
