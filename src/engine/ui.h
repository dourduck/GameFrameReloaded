#ifndef UI_H
#define UI_H

#include "ecs/archetypes.h"
#include "./../../external/raysan5/include/raylib.h"
#include <stdio.h>
#define RAYGUI_IMPLEMENTATION
#include "./../../external/raysan5/include/raygui.h"

#include "../game/entity.h"

void ui_init() {
  GuiLoadStyle("./external/raysan5/styles/wisteria/style_wisteria.rgs");

  int font_size = 36;
  Font font =
      LoadFontEx("./assets/font/OpenDyslexic-Regular.otf", font_size, NULL, 0);

  GuiSetFont(font);

  GuiSetStyle(DEFAULT, TEXT_SIZE, font_size);
}

void ui_text_impl(TextComponent text_component) {
  GuiDrawText(text_component.content, text_component.bounds,
              text_component.alignment, text_component.color);
}

void ui_button_impl(Selectable selectable, Button btn_component, Position pos) {
  Rectangle r = (Rectangle){.x = pos.x,
                            .y = pos.y,
                            .width = btn_component.w,
                            .height = btn_component.h};
  const char *txt = btn_component.text;

  if (GuiButton(r, txt)) {
    /* gameplay code can be done in the selection system pass to queue button
     * press event*/
    /* this could be a good spot for general effects when a button is pressed
     * such as sound */
    /* WARNING: GAMEPLAY CODE SHOULD NOT GO HERE!*/
  }
}

static void sys_ui_panels(World *w, Archetype *a, void *userdata) {
  (void)w;
  (void)userdata;

  Position *positions = archetype_column(a, Position_id);
  Panel *panels = archetype_column(a, Panel_id);

  for (uint32_t i = 0; i < a->count; i++) {
    Color color = panels[i].color;
    Color border_color = panels[i].border_color;
    int border_width = panels[i].border_width;
    Rectangle rect = panels[i].rect;
    Vector2 origin = panels[i].origin;
    float rot = panels[i].rotation;

    GuiDrawRectangle(rect, border_width, border_color, color);
  }
}

static void sys_ui_buttons(World *w, Archetype *a, void *userdata) {
  (void)w;
  (void)userdata;

  Position *positions = archetype_column(a, Position_id);
  Selectable *selectables = archetype_column(a, Selectable_id);
  Button *buttons = archetype_column(a, Button_id);

  for (uint32_t i = 0; i < a->count; i++) {
    Button btn = buttons[i];
    Position pos = positions[i];
    Selectable selectable = selectables[i];

    ui_button_impl(selectable, btn, pos);
  }
}

static void sys_ui_text(World *w, Archetype *a, void *userdata) {
  (void)w;
  (void)userdata;

  Position *positions = archetype_column(a, Position_id);
  // Selectable *selectables = archetype_column(a, Selectable_id);
  TextComponent *text_components = archetype_column(a, TextComponent_id);

  for (uint32_t i = 0; i < a->count; i++) {
    Position pos = positions[i];
    // Selectable selectable = selectables[i];
    TextComponent text_component = text_components[i];

    ui_text_impl(text_component);
  }
}

#endif
/* vim:set ts=3 sw=2 sts=2 et: */
