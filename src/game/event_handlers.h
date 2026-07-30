#ifndef EVENT_HANDLERS_H
#define EVENT_HANDLERS_H

#include "../engine/ecs/archetypes.h"
#include "../engine/event_system/event_bus.h"
#include "./entity/prefabs.h" /* StatMenuCtx */

typedef struct {
  World *world;
  StatMenuCtx *stat_menu_ctx;
} GameHandlerCtx;

void game_handler_register(EventBus *event_bus, GameHandlerCtx *ctx);
void game_handler_unregister(EventBus *event_bus);

#endif
/* vim:set ts=3 sw=2 sts=2 et: */
