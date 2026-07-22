#ifndef GAME_H
#define GAME_H

#include "raylib.h"

typedef int ExitKey;

typedef struct {
  int width;
  int height;
  const char *title;
  int target_fps;
  ExitKey exit_key;
  int flags;
} Environment;

static Environment Environment_CreateDefault() {
  return (Environment){
      .width = 800,
      .height = 600,
      .title = "Game",
      .target_fps = 60,
      .exit_key = KEY_Q,
      .flags = 0,
  };
}

static void Environment_InitWindow(Environment *env) {
  SetConfigFlags(env->flags);
  InitWindow(env->width, env->height, env->title);
  SetTargetFPS(env->target_fps);
  SetExitKey(env->exit_key);
}

#endif

/* vim:set ts=3 sw=2 sts=2 et: */
