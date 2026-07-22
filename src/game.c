#include "game.h"
#include "raylib.h"

Environment Environment_CreateDefault() {
  return (Environment){
      .width = 800,
      .height = 600,
      .title = "Game",
      .target_fps = 60,
      .exit_key = KEY_Q,
      .flags = 0,
  };
}

void Environment_InitWindow(Environment *env) {
  SetConfigFlags(env->flags);
  InitWindow(env->width, env->height, env->title);
  SetTargetFPS(env->target_fps);
  SetExitKey(env->exit_key);
}

/* vim:set ts=3 sw=2 sts=2 et: */
