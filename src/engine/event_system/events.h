#ifndef EVENTS_H
#define EVENTS_H

#include "../ecs/archetypes.h"
#include <stdint.h>

typedef uint32_t u32;

typedef enum {
  EVENT_NONE = 0,

  /* (-Input-) */
  // EVENT_KEY_PRESSED,
  // EVENT_KEY_RELEASED,
  // EVENT_MOUSE_MOVED,
  // EVENT_MOUSE_CLICKED,

  /* (-Gameplay-) */
  EVENT_ENTITY_DIED,
  EVENT_ENTITY_SPAWNED,
  EVENT_ENTITY_TARGET_REACHED,

  EVENT_ENTITY_SELECTED,
  // EVENT_CHARACTER_SELECTED,
  // EVENT_BUTTON_PRESSED,
  // EVENT_DAMAGE_DEALT,
  // EVENT_ITEM_PICKED_UP,

  /* (-Gameflow-) */
  // EVENT_LEVEL_STARTED,
  // EVENT_LEVEL_COMPLETE,
  // EVENT_GAME_OVER,
  // EVENT_PAUSE_TOGGLED,

  /* (-*-) */
  EVENT_COUNT
} EventType;

typedef enum {
  SELECTION_CHARACTER,
  SELECTION_BUTTON,
} SelectionType;

typedef struct {
  SelectionType type;
  Entity entity;

  union {
    struct {
      Entity entity;
      float pos_x;
      float pos_y;
    } character_data;

    struct {
      Entity entity;
      void (*callback)(void *ctx);
    } button_data;
  } data;

} SelectionData;

/* [[ Event Payload ]] */
typedef struct {
  EventType type;
  float timestamp; // gametime when fired
  union {
    /* (-Gameplay-) */
    struct {
      u32 entity_id;
      u32 killer_id;
      int cause;
    } entity_death;
    struct {
      u32 entity_id;
      float x, y;
      int entity_type;
    } entity_spawn;
    struct {
      Entity current_entity;
      Entity target_entity;
    } entity_target_reached;

    SelectionData entity_selection_data;

    // struct {
    //   Entity entity;
    // } button_pressed;
    // EVENT_ENTITY_DIED,
    // EVENT_ENTITY_SPAWNED,
    // EVENT_ENTITY_TARGET_REACHED,
    // // EVENT_ENTITY_SELECTED,
    // EVENT_CHARACTER_SELECTED,
    // EVENT_BUTTON_PRESSED,

    /* (-Input-) */
    // struct {
    //   int keycode;
    //   int scancode;
    //   int mods; // shift/ctrl/alt
    // } key;
    // struct {
    //   float x, y;   // cursor position
    //   float dx, dy; // last frame delta
    //   int button;
    // } mouse;

    // struct {
    //   u32 target_id;
    //   u32 source_id;
    //   float amount;
    //   int damage_type;
    // } damage;
    // struct {
    //   u32 entity_id;
    //   int item_id;
    // } item;

    /* (-Game flow-) */
    // struct {
    //   int level;
    //   float completion_time;
    //   int stars;
    // } level;

    /* (-Fallback-) */
    // struct {
    //   int i[4];
    //   int f[4];
    // } generic;
  } data;
} Event;

// static inline Event event_key(EventType type, int keycode, int mods) {
//   Event e = {0};
//   e.type = type;
//   e.data.key.keycode = keycode;
//   e.data.key.mods = mods;
//   return e;
// }
static inline Event event_entity_died(u32 id, u32 killer, int cause) {
  Event e = {0};
  e.type = EVENT_ENTITY_DIED;
  e.data.entity_death.entity_id = id;
  e.data.entity_death.killer_id = killer;
  e.data.entity_death.cause = cause;
  return e;
}

#endif

/* vim:set ts=3 sw=2 sts=2 et: */
