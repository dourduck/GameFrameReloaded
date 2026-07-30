#ifndef EVENT_FACTORIES_H
#define EVENT_FACTORIES_H

#include "./events.h"

/* One constructor per event type
 * Factory functions for events are to prevent events from being hand assembled incorrectly.
 * */

static inline Event event_create_target_reached(Entity current, Entity target){
  Event e = {0};
  e.type = EVENT_ENTITY_TARGET_REACHED;
  e.data.entity_target_reached.current_entity = current;
  e.data.entity_target_reached.target_entity = target;
  return e;
}



static inline Event event_create_no_selection(void){
  Event e = {0};
  e.type = EVENT_NO_SELECTION;
  return e;
}

/* Selection_type is an enum (SelectionType) from game/entity/components.h
 * Represented as a plain int here.
 * */

static inline Event event_create_entity_selected(Entity entity, int selection_type) {
  Event e = {0};
  e.type = EVENT_ENTITY_SELECTED;
  e.data.entity_selection.entity = entity;
  e.data.entity_selection.selection_type = selection_type;
  return e;
}

/* Fired when a character is first selected AND when a selected characters stats change (sys_character_stats)
 * Use case(s):
 * - Repainting the character stat menu
 * */

static inline Event event_create_character_selected(Entity entity) {
  Event e = {0};
  e.type = EVENT_CHARACTER_SELECTED;
  e.data.entity_selection.entity = entity;
  return e;
}
#endif
/* vim:set ts=3 sw=2 sts=2 et: */
