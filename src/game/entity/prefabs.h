#ifndef PREFABS_H
#define PREFABS_H

#include "./../../engine/ecs/archetypes.h"
#include "./components.h"
#include "raylib.h"
#include <math.h>

#define DEBUG 0

static Entity prefab_target(World *world, float x, float y) {
  Entity target = entity_create(world);

  Position position = (Position){.x = x, .y = y};

  world_add_component(world, target, Position_id, &position);

  if (DEBUG) {
    BodyDebug body_debug = (BodyDebug){.color = LIGHTGRAY, .radius = 2};
    world_add_component(world, target, BodyDebug_id, &body_debug);
  }

  return target;
}

static Entity prefab_chicken(World *world, float x, float y, Texture texture,
                             Rectangle src, Vector2 scale) {

  float w = texture.width;
  float h = texture.height;

  float w_scaled = w * scale.x;
  float h_scaled = h * scale.y;

  float padding = 4;
  float x_offset = -(16 * scale.x * 0.25f) - (padding * scale.x * 0.5f);
  float y_offset = -(17.5 * scale.y * 0.25f) - (padding * scale.y * 0.5f);

  /*
   * res: 32x32
   * pos: 10, 11
   * size: 12x13
   * center: 16, 17.5
   * */

  Entity chicken_entity = entity_create(world);

  Position position = (Position){.x = x, .y = y};
  world_add_component(world, chicken_entity, Position_id, &position);

  Vector2 sprite_origin = {.x = (16 * scale.x),
                           .y = (17.5 * scale.y) + (padding * scale.y * 0.25f)};
  Sprite sprite = (Sprite){
      .texture = texture, .src = src, .scale = scale, .origin = sprite_origin};
  world_add_component(world, chicken_entity, Sprite_id, &sprite);

  Selectable selectable = (Selectable){
      .entity = chicken_entity,
      .width = (16 * scale.x * 0.5f) + (padding * scale.x),
      .height = (17.5 * scale.y * 0.5f) + (padding * scale.y),
      .offset_x = x_offset,
      .offset_y = y_offset,
      .priority = 1,
      .selected = false,
      .type = SELECTION_CHARACTER,
  };

  world_add_component(world, chicken_entity, Selectable_id, &selectable);

  Collider collider = (Collider){.radius = (16 * scale.x * 0.6f)};
  world_add_component(world, chicken_entity, Collider_id, &collider);

  Velocity velocity = (Velocity){.dx = 0.0f, .dy = 0.0f};
  world_add_component(world, chicken_entity, Velocity_id, &velocity);

  float target_x = GetRandomValue(2, GetScreenWidth() - 2);
  float target_y = GetRandomValue(2, GetScreenHeight() - 2);
  Entity target_entity = prefab_target(world, target_x, target_y);

  Position *target_position =
      world_get_component(world, target_entity, Position_id);

  assert(target_position);

  Target target = (Target){
      .entity = target_entity,
      .reached_threshold = 10.0f,
  };

  world_add_component(world, chicken_entity, Target_id, &target);

  Speed speed = (Speed){.speed = 60.0f};
  world_add_component(world, chicken_entity, Speed_id, &speed);

  if (DEBUG) {
    BodyDebug body_debug =
        (BodyDebug){.color = ORANGE, .radius = collider.radius};

    world_add_component(world, chicken_entity, BodyDebug_id, &body_debug);
  }

  return chicken_entity;
}

static Entity prefab_ui_button(World *world, Vector2 origin, int width,
                               int height) {
  Entity btn_entity = entity_create(world);

  int pos_x = 8;
  int pos_y = 8;
  Position position = (Position){.x = origin.x + pos_x, .y = origin.y + pos_y};

  Button btn_component =
      (Button){.w = width, .h = height, .text = "Use Potion or Something :P"};

  Selectable selectable = (Selectable){
      .width = width,
      .height = height,
      .offset_x = 0,
      .offset_y = 0,
      .selected = false,
      .entity = btn_entity,
      .priority = 0,
      .type = SELECTION_BUTTON,
  };

  world_add_component(world, btn_entity, Position_id, &position);
  world_add_component(world, btn_entity, Selectable_id, &selectable);
  world_add_component(world, btn_entity, Button_id, &btn_component);

  return btn_entity;
}

static Entity prefab_ui_text(World *world, Vector2 origin, int width,
                             int height, char *content) {
  Entity txt_entity = entity_create(world);

  int pos_x = 8;
  int pos_y = 8;
  Position position = (Position){.x = origin.x + pos_x, .y = origin.y + pos_y};
  Rectangle bounds = (Rectangle){
      .x = position.x, .y = position.y, .width = width, .height = height};
  int alignment = 0;
  Color color = RAYWHITE;

  TextComponent txt_component = (TextComponent){
      .content = content, .bounds = bounds, .alignment = 0, .color = color};

  world_add_component(world, txt_entity, Position_id, &position);
  world_add_component(world, txt_entity, TextComponent_id, &txt_component);

  return txt_entity;
}

static Entity prefab_slime(World *world) {
  Entity slime = entity_create(world);

  Position position = (Position){.x = 100, .y = 100};
  Velocity velocity = (Velocity){.dx = 0.0f, .dy = 0.0f};
  Speed speed = (Speed){.speed = 100.0f};
  Collider collider = (Collider){.radius = 8};
  BodyDebug body_debug =
      (BodyDebug){.color = (Color){.r = 32, .g = 200, .b = 32, .a = 255},
                  .radius = collider.radius};

  int rand_hunger = GetRandomValue(1, 100);
  int rand_thirst = GetRandomValue(1, 100);
  CharacterData character_data = {
      .hunger = 100.0,
      .thirst = 100.0,
      .hunger_rate = 0.5f,
      .thirst_rate = 0.5f,
      .hunger_timer = 0.0f,
      .thirst_timer = 0.0f,
  };

  float target_x = GetRandomValue(200, 600);
  float target_y = GetRandomValue(150, 450);
  Entity target_entity = prefab_target(world, target_x, target_y);

  Position *target_position =
      world_get_component(world, target_entity, Position_id);

  assert(target_position);

  Target target = (Target){
      .entity = target_entity,
      .reached_threshold = 10.0f,
  };

  Selectable selectable = (Selectable){
      .width = collider.radius * 2,
      .height = collider.radius * 2,
      .offset_x = -(collider.radius),
      .offset_y = -(collider.radius),
      .selected = false,
      .entity = slime,
      .priority = 10,
      .type = SELECTION_CHARACTER,
  };

  world_add_component(world, slime, Position_id, &position);
  world_add_component(world, slime, Velocity_id, &velocity);
  world_add_component(world, slime, Speed_id, &speed);
  world_add_component(world, slime, BodyDebug_id, &body_debug);

  world_add_component(world, slime, Target_id, &target);

  world_add_component(world, slime, Collider_id, &collider);
  world_add_component(world, slime, Selectable_id, &selectable);
  world_add_component(world, slime, CharacterData_id, &character_data);

  return slime;
}

typedef struct {
  Entity root;
  TextComponent *txt_hunger;
  World *world;
  char buffer[64];
} StatMenuCtx;

static void prefab_ui_stat_menu(World *world, StatMenuCtx *stat_menu_out) {
  // StatMenuCtx menu = {0};
  Entity root = entity_create(world);

  float width = 300;
  float height = GetScreenHeight();
  float pos_x = GetScreenWidth() - width;
  float pos_y = 0;

  Position position = (Position){.x = pos_x, .y = pos_y};

  Selectable selectable = (Selectable){
      .selected = false,
      .width = width,
      .height = height,
      .offset_x = 0,
      .offset_y = 0,
      .priority = 1, /* Lower is higher priority (0-31)*/
      .entity = root,
      .type = SELECTION_PANEL,
  };

  Panel panel = (Panel){
      .color =
          (Color){
              .r = 60,
              .g = 159,
              .b = 156,
              .a = 100,
          },
      .border_color =
          (Color){
              .r = 146,
              .g = 126,
              .b = 106,
              .a = 100,
          },
      .border_width = 4,
      .rect =
          (Rectangle){
              .x = pos_x,
              .y = pos_y,
              .width = width,
              .height = height,
          },
      .origin = (Vector2){.x = 0, .y = 0},
      .rotation = 0,
  };

  world_add_component(world, root, Position_id, &position);
  world_add_component(world, root, Selectable_id, &selectable);
  world_add_component(world, root, Panel_id, &panel);

  int elem_h = floor(width * 0.25f);
  /*TODO: Figure out better way to approach padding/margins */
  Entity btn = prefab_ui_button(
      world, (Vector2){.x = pos_x, .y = GetScreenHeight() - elem_h - 16},
      width - 16, elem_h);

  // static Entity prefab_ui_text(World *world, Vector2 origin, int width, int
  // height, const char* content) {
  Entity txt_hunger = prefab_ui_text(
      world,
      (Vector2){.x = pos_x, .y = GetScreenHeight() - ((elem_h - 16) * 3)},
      width - 16, elem_h, "");

  stat_menu_out->root = root;
  stat_menu_out->world = world;
  stat_menu_out->txt_hunger = world_get_component(world, txt_hunger, TextComponent_id);
}

static Entity prefab_cursor(World *world, float x, float y) {
  Entity target = entity_create(world);

  Position position = (Position){.x = x, .y = y};
  Cursor cursor = (Cursor){.radius = 8, .color = WHITE};

  world_add_component(world, target, Position_id, &position);
  world_add_component(world, target, Cursor_id, &cursor);

  return target;
}

#undef DEBUG

#endif
/* vim:set ts=3 sw=2 sts=2 et: */
