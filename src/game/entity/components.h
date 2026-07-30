#ifndef COMPONENTS_H
#define COMPONENTS_H
#include "./../../../external/raysan5/include/raylib.h"
#include "./../../engine/ecs/archetypes.h"
#include "./../../engine/event_system/event_queue.h"

typedef struct {
  float x;
  float y;
} Position;

typedef struct {
  float dx;
  float dy;
} Velocity;

typedef struct {
  int speed;
} Speed;

typedef struct {
  Color color;
  float radius;
} BodyDebug;

typedef struct {
  Entity entity;
  bool reached;
  float reached_threshold;
} Target;

typedef struct {
  float radius;
} Collider;

typedef enum {
  SELECTION_CHARACTER,
  SELECTION_BUTTON,
  SELECTION_PANEL,
} SelectionType;

typedef struct {
  bool selected;
  float width;
  float height;
  float offset_x;
  float offset_y;
  int priority; /* Lower is higher priority (0-31)*/
  Entity entity;
  SelectionType type;
} Selectable;

typedef struct {
  float w;
  float h;

  const char *text;
} Button;

typedef struct {
  float hunger;
  float thirst;

  float hunger_timer;
  float thirst_timer;

  float hunger_rate;
  float thirst_rate;
} CharacterData;

typedef struct {
  EventQueue *event_queue;
  float dt;
} CharacterDataCtx;

typedef struct {
  Color color;
  Color border_color;
  int border_width;
  Rectangle rect;
  Vector2 origin;
  float rotation;
} Panel;

typedef struct {
  char *content;
  Rectangle bounds;
  int alignment;
  Color color;
} TextComponent;

DECLARE_COMPONENT_ID(Position);
DECLARE_COMPONENT_ID(Velocity);
DECLARE_COMPONENT_ID(Health);
DECLARE_COMPONENT_ID(Speed);
DECLARE_COMPONENT_ID(StateMachine);
DECLARE_COMPONENT_ID(Target);
DECLARE_COMPONENT_ID(BodyDebug);
DECLARE_COMPONENT_ID(Collider);
DECLARE_COMPONENT_ID(Selectable);
DECLARE_COMPONENT_ID(Button);
DECLARE_COMPONENT_ID(Panel);
DECLARE_COMPONENT_ID(CharacterData);
DECLARE_COMPONENT_ID(TextComponent);

static void register_components() {
  REGISTER(Position);
  REGISTER(Velocity);
  REGISTER(Speed);
  REGISTER(Target);
  REGISTER(BodyDebug);
  REGISTER(Collider);
  REGISTER(Selectable);
  REGISTER(Button);
  REGISTER(Panel);
  REGISTER(CharacterData);
  REGISTER(TextComponent);
}

#define SELECTABLE_MAX 32

typedef struct {
  EventQueue *event_queue;
  Selectable *selections[SELECTABLE_MAX];
  int count;
} SelectableCtx;

SelectableCtx selectable_ctx_init(EventQueue *q) {
  SelectableCtx ctx = (SelectableCtx){
      .event_queue = q,
      .count = 0,
      .selections = {0},
  };

  return ctx;
}
#endif
/* vim:set ts=3 sw=2 sts=2 et: */
