#ifndef EVENTS_H
#define EVENTS_H

#include "../ecs/archetypes.h"

typedef enum {
  EVENT_NONE = 0,

  /* (-Gameplay-) */
  EVENT_ENTITY_SPAWNED,
  EVENT_ENTITY_TARGET_REACHED,
  EVENT_ENTITY_SELECTED,
  EVENT_CHARACTER_SELECTED,
  EVENT_NO_SELECTION,

  /* (-*-) */
  EVENT_COUNT
} EventType;

/* [[ Event Payload ]] */
typedef struct {
  EventType type;

  union {
    /* (-Gameplay-) */
    struct {
      Entity entity;
      float x, y;
      int entity_type;
    } entity_spawn;

    struct {
      Entity current_entity;
      Entity target_entity;
    } entity_target_reached;

    /* (-Selectable Data-) */
    struct{
      int selection_type;
      Entity entity;

      union {
        struct {
          void *character_component;
        } character_data;

        struct {
          void (*callback)(void *ctx);
        } button_data;

        struct {
        } panel_data;
      };
    } entity_selection_data;

  } data;
} Event;

#endif
/* vim:set ts=3 sw=2 sts=2 et: */
