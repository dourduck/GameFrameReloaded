#ifndef COMPONENT_SYSTEMS_H
#define COMPONENT_SYSTEMS_H

#include <math.h>

#include "./../../../external/raysan5/include/raylib.h"
#include "./../../engine/ecs/archetypes.h"

#include "./../arena.h"
#include "./components.h"

static void sys_character_stats(World *w, Archetype *a, void *userdata) {
  CharacterDataCtx *character_data_ctx = userdata;

  CharacterData *character_data = archetype_column(a, CharacterData_id);
  Selectable *selectables = archetype_column(a, Selectable_id);

  for (uint32_t i = 0; i < a->count; i++) {
    float dt = character_data_ctx->dt;

    float hunger_rate = character_data[i].hunger_rate;
    float hunger_timer = character_data[i].hunger_timer;

    bool timer_reset = false;

    if (hunger_timer < hunger_rate) {
      character_data[i].hunger_timer += dt;
    } else {
      timer_reset = true;
      character_data[i].hunger_timer = 0;
      character_data[i].hunger -= 1;
    }

    float thirst_rate = character_data[i].thirst_rate;
    float thirst_timer = character_data[i].thirst_timer;

    if (thirst_timer < thirst_rate) {
      character_data[i].thirst_timer += dt;
    } else {
      timer_reset = true;
      character_data[i].thirst_timer = 0;
      character_data[i].thirst -= 1;
    }

    if (selectables[i].selected && timer_reset) {
      Event e = {0};
      e.type = EVENT_CHARACTER_SELECTED;
      e.data.entity_selection_data.selection_type = SELECTION_CHARACTER;
      e.data.entity_selection_data.entity = a->entities[i];
      e.data.entity_selection_data.character_data.character_component =
          &character_data[i];
      event_queue_push(character_data_ctx->event_queue, &e);
    }
  }
}

static void sys_selectables(World *w, Archetype *a, void *userdata) {
  bool mouse_pressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

  if (!mouse_pressed) {
    return;
  }

  (void)w;
  SelectableCtx *ctx = userdata;

  Event e = {0};
  e.type = EVENT_NO_SELECTION;
  event_queue_push(ctx->event_queue, &e);

  Position *positions = archetype_column(a, Position_id);
  Selectable *selectables = archetype_column(a, Selectable_id);

  for (uint32_t i = 0; i < a->count; i++) {
    selectables[i].selected = false;

    Vector2 mouse_pos = GetMousePosition();

    Rectangle r = (Rectangle){
        .x = positions[i].x + selectables[i].offset_x,
        .y = positions[i].y + selectables[i].offset_y,
        .width = selectables[i].width,
        .height = selectables[i].height,
    };

    if (CheckCollisionPointRec(mouse_pos, r)) {
      if (ctx->count < SELECTABLE_MAX) {
        ctx->selections[ctx->count++] = &selectables[i];
      }
    }
  }
}

void selectables_resolve(World *world, SelectableCtx *ctx) {
  if (!ctx->count) {
    return;
  }

  /* bubble sort */
  for (int i = 0; i < (ctx->count - 1); i++) {
    int swapped = 0;

    for (int j = 0; j < (ctx->count - i - 1); j++) {
      if (ctx->selections[j]->priority > ctx->selections[j + 1]->priority) {
        /* swap selections[j] with selections[j + 1] */

        Selectable *temp = ctx->selections[j];
        ctx->selections[j] = ctx->selections[j + 1];
        ctx->selections[j + 1] = temp;
        swapped = 1;
      }
    }

    if (!swapped) {
      break;
    }
  }

  // printf("Selection priorities: ");
  // for (int i = 0; i < ctx->count; i++) {
  //   printf("%d, ", ctx->selections[i]->priority);
  // }
  // printf("\n");

  for (int i = 0; i < ctx->count; i++) {
    ctx->selections[i]->selected = true;

    Event *e = event_arena_alloc(sizeof(Event));
    e->type = EVENT_ENTITY_SELECTED;
    e->data.entity_selection_data.entity = ctx->selections[i]->entity;
    e->data.entity_selection_data.selection_type = ctx->selections[i]->type;

    switch (e->data.entity_selection_data.selection_type) {
    case SELECTION_BUTTON:
      break;
    case SELECTION_CHARACTER:
      e->type = EVENT_CHARACTER_SELECTED;
      e->data.entity_selection_data.character_data.character_component =
          world_get_component(world, e->data.entity_selection_data.entity,
                              CharacterData_id);
      break;
    case SELECTION_PANEL:
      break;
    }

    event_queue_push(ctx->event_queue, e);
    if (i + 1 < ctx->count &&
        ctx->selections[i]->priority < ctx->selections[i + 1]->priority) {
      break;
    }
  }
  ctx->count = 0;
}

static void sys_render_selections(World *w, Archetype *a, void *userdata) {
  (void)w;
  (void)userdata;

  Position *positions = archetype_column(a, Position_id);
  Selectable *selectables = archetype_column(a, Selectable_id);

  for (uint32_t i = 0; i < a->count; i++) {
    if (selectables[i].selected) {
      Rectangle r = (Rectangle){
          .x = positions[i].x + selectables[i].offset_x,
          .y = positions[i].y + selectables[i].offset_y,
          .width = selectables[i].width,
          .height = selectables[i].height,
      };
      DrawRectangleLinesEx(r, 4, GREEN);
    }
  }
}

/* Set the entity velocity based on the direction to given point */
static void sys_vel_toward_target_position(World *w, Archetype *a,
                                           void *userdata) {
  (void)w;
  EventQueue *event_queue = userdata;

  Target *targets = archetype_column(a, Target_id);
  Velocity *velocities = archetype_column(a, Velocity_id);
  Position *positions = archetype_column(a, Position_id);
  Speed *speeds = archetype_column(a, Speed_id);

  for (uint32_t i = 0; i < a->count; i++) {
    if (targets[i].reached) {
      continue;
    }

    /* calc direction to target */
    Position *target_pos =
        world_get_component(w, targets[i].entity, Position_id);
    float dx = (target_pos->x) - positions[i].x;
    float dy = (target_pos->y) - positions[i].y;

    /* calc magnitude of dir */
    float dxx = dx * dx;
    float dyy = dy * dy;
    float mag = sqrt(dxx + dyy);

    if (mag <= targets[i].reached_threshold && !targets[i].reached) {

      Entity current_entity = a->entities[i];
      Entity target_entity = targets[i].entity;

      Event *e = event_arena_alloc(sizeof(Event));
      e->type = EVENT_ENTITY_TARGET_REACHED;
      e->data.entity_target_reached.current_entity = current_entity;
      e->data.entity_target_reached.target_entity = target_entity;
      event_queue_push(event_queue, e);

      targets[i].reached = true;
      velocities[i].dx = 0;
      velocities[i].dy = 0;
      continue;
    }

    /* normalize direction */
    float nx = dx / mag;
    float ny = dy / mag;

    /* Apply speed to normalized direction */
    float spd = speeds[i].speed;

    /* Assign to velocity */
    velocities[i].dx = nx * spd;
    velocities[i].dy = ny * spd;
  }
}

/* [NAIVE IMPLEMENTATION] */
static void sys_collision(World *w, Archetype *a, void *userdata) {
  (void)w;
  (void)userdata;

  Position *positions = archetype_column(a, Position_id);
  Collider *colliders = archetype_column(a, Collider_id);
  Velocity *velocities = archetype_column(a, Velocity_id);

  for (uint32_t i = 0; i < a->count; i++) {
    float ix = positions[i].x;
    float iy = positions[i].y;

    for (uint32_t j = 0; j < a->count; j++) {
      if (j == i) {
        continue;
      }

      float jx = positions[j].x;
      float jy = positions[j].y;

      float dir_x = jx - ix;
      float dir_y = jy - iy;

      float mag = sqrt((dir_x * dir_x) + (dir_y * dir_y));

      float collider_radius = (colliders[i].radius * 1.25f);

      if (mag < collider_radius) {
        if (mag > 0.0001f) {
          /* normalize and invert then scale to correct position*/
          positions[i].x += -(dir_x / mag) * collider_radius * 0.1f;
          positions[i].y += -(dir_y / mag) * collider_radius * 0.1f;

          positions[j].x += (dir_x / mag) * collider_radius * 0.1f;
          positions[j].y += (dir_y / mag) * collider_radius * 0.1f;

          velocities[i].dx += -(dir_x / mag) * 10;
          velocities[i].dy += -(dir_y / mag) * 10;

          velocities[j].dx += (dir_x / mag) * 10;
          velocities[j].dy += (dir_y / mag) * 10;

        } else {
          positions[i].x +=
              (colliders[i].radius) * (GetRandomValue(0, 1) ? 1 : -1);
          positions[i].y +=
              (colliders[i].radius) * (GetRandomValue(0, 1) ? 1 : -1);
        }
      }
    }
  }
}

static void sys_movement(World *w, Archetype *a, void *userdata) {
  (void)w;
  float dt = *(float *)userdata;
  Position *positions = archetype_column(a, Position_id);
  Velocity *velocities = archetype_column(a, Velocity_id);
  for (uint32_t i = 0; i < a->count; i++) {
    positions[i].x += velocities[i].dx * dt;
    positions[i].y += velocities[i].dy * dt;
  }
}

static void sys_render(World *w, Archetype *a, void *userdata) {
  (void)w;
  (void)userdata;

  Position *positions = archetype_column(a, Position_id);
  BodyDebug *debug_bodies = archetype_column(a, BodyDebug_id);

  for (uint32_t i = 0; i < a->count; i++) {
    DrawCircle(positions[i].x, positions[i].y, debug_bodies[i].radius,
               debug_bodies[i].color);
  }
}

#endif
/* vim:set ts=3 sw=2 sts=2 et: */
