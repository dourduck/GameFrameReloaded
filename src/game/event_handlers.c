#include "event_handlers.h"

#include <stdio.h>

#include "./../../external/raysan5/include/raylib.h"
#include "./entity/components.h"

static void on_target_reached(const Event *e, void *ctx) {
  // GameHandlerCtx *game_ctx = ctx;
  // World *w = game_ctx->world;
  // printf("target reached event\n");



  // Velocity *velocity = world_get_component(
  //     w, e->data.entity_target_reached.current_entity, Velocity_id);
  //
  // velocity->dx = 0;
  // velocity->dy = 0;

  // Target *target = world_get_component(
  //     w, e->data.entity_target_reached.current_entity, Target_id);

  // Position *target_pos = world_get_component(
  //     w, e->data.entity_target_reached.target_entity, Position_id);
  //
  // float rand_x = target_pos->x + (GetRandomValue(0, 1) ? -25 : 25);
  // float rand_y = target_pos->y + (GetRandomValue(0, 1) ? -25 : 25);
  //
  // rand_x = rand_x > 800 ? (rand_x - 50) : rand_x < 0 ? (rand_x + 50) : rand_x;
  // rand_y = rand_y > 600 ? (rand_y - 50) : rand_y < 0 ? (rand_y + 50) : rand_y;
  //
  // target_pos->x = rand_x;
  // target_pos->y = rand_y;
}

static void on_entity_selected(const Event *e, void *ctx) {
  (void)ctx;

  switch (e->data.entity_selection.selection_type) {
  case SELECTION_BUTTON:
    printf("Selected BUTTON!: %d\n", (e->data.entity_selection.entity.index));
    break;
  case SELECTION_CHARACTER:
    printf("Selected CHARACTER!: %d\n",
           (e->data.entity_selection.entity.index));
    break;
  case SELECTION_PANEL:
    printf("Selected PANEL!: %d\n", (e->data.entity_selection.entity.index));
    break;
  }
}

static void on_no_selection(const Event *e, void *ctx) {
  GameHandlerCtx *game_ctx = ctx;
  StatMenuCtx *stat_menu_ctx = game_ctx->stat_menu_ctx;

  stat_menu_ctx->buffer[0] = '\0';
  stat_menu_ctx->txt_hunger->content = stat_menu_ctx->buffer;
}

static void on_character_selected(const Event *e, void *ctx) {
  GameHandlerCtx *game_ctx = ctx;
  StatMenuCtx *stat_menu_ctx = game_ctx->stat_menu_ctx;
  World *world = game_ctx->world;

  CharacterData *character_data = world_get_component(
      world, e->data.entity_selection.entity, CharacterData_id);

  if (!character_data) {
    return;
  }

  printf("Event Hunger: %.0f\n", character_data->hunger);

  int char_written =
      snprintf(stat_menu_ctx->buffer, sizeof(stat_menu_ctx->buffer),
               "Hunger: %.0f\nThirst: %.0f", character_data->hunger,
               character_data->thirst);

  stat_menu_ctx->txt_hunger->content = stat_menu_ctx->buffer;
}

/* Handler Binding */

typedef struct {
  EventType type;
  EventCallback callback;
} HandlerBinding;

static const HandlerBinding kBindings[] = {
    {EVENT_ENTITY_TARGET_REACHED, on_target_reached},
    {EVENT_ENTITY_SELECTED, on_entity_selected},
    {EVENT_CHARACTER_SELECTED, on_character_selected},
    {EVENT_NO_SELECTION, on_no_selection},
};

#define HANDLER_COUNT (sizeof(kBindings) / sizeof(kBindings[0]))
void game_handler_register(EventBus *event_bus, GameHandlerCtx *ctx) {
  for (size_t i = 0; i < HANDLER_COUNT; i++) {
    event_subscribe(event_bus, kBindings[i].type, kBindings[i].callback, ctx);
  }
}

void game_handler_unregister(EventBus *event_bus) {
  for (size_t i = 0; i < HANDLER_COUNT; i++) {
    event_unsubscribe(event_bus, kBindings[i].type, kBindings[i].callback);
  }
}

/* vim:set ts=3 sw=2 sts=2 et: */
