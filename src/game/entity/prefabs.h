#ifndef PREFABS_H
#define PREFABS_H

#include "./../../engine/ecs/archetypes.h"
#include "./components.h"
#include "raylib.h"
#include <math.h>

#define DEBUG 1

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

static StatMenuCtx prefab_ui_stat_menu(World *world) {
  StatMenuCtx menu = {0};
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

  menu.root = root;
  menu.world = world;
  menu.txt_hunger = world_get_component(world, txt_hunger, TextComponent_id);

  return menu;
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
