#ifndef GAME_H
#define GAME_H

typedef int ExitKey;

typedef struct {
  int width;
  int height;
  const char *title;
  int target_fps;
  ExitKey exit_key;
  int flags;
} Environment;

Environment Environment_CreateDefault();
void Environment_InitWindow(Environment *env);

#endif

/* vim:set ts=3 sw=2 sts=2 et: */
