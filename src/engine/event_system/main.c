#include "./event_queue.h"
#include "./events.h"
#include "event_bus.h"

#include <stdio.h>

/* vvv [ Listener Callbacks ] vvv */

typedef struct {
  int score;
  int kills;
} PlayerStats;

static void on_entity_died(const Event *e, void *ctx) {
  PlayerStats *stats = (PlayerStats *)ctx;
  stats->score += 100;
  stats->kills++;
  printf("[UI] Entity %u killed by %u. Score %d\n",
         e->data.entity_death.entity_id, e->data.entity_death.killer_id,
         stats->score);
}

static void on_entity_died_audio(const Event *e, void *ctx) {
  (void)ctx;
  printf("[Audio] Playing death sound for entity %u\n",
         e->data.entity_death.entity_id);
}

static void on_level_complete(const Event *e, void *ctx) {
  (void)ctx;
  printf("[Game] Level %d complete in %.2fs!\n", e->data.level.level,
         e->data.level.completion_time);
}

static void on_jump(const Event *e, void *ctx) {
  (void)ctx;
  printf("[Game] Level %d complete in %.2fs!\n", e->data.level.level,
         e->data.level.completion_time);
}

/* ^^^ [ Listener Callbacks ] ^^^ */

/* vvv [ Entry Point ] vvv */

int main(void) {
  EventBus bus;
  EventQueue queue;
  PlayerStats stats = {0};

  event_bus_init(&bus);
  event_queue_init(&queue, &bus);

  /* vvv   [ Subscribe ] vvv */
  event_subscribe(&bus, EVENT_ENTITY_DIED, on_entity_died, &stats);
  event_subscribe(&bus, EVENT_ENTITY_DIED, on_entity_died_audio, NULL);
  event_subscribe(&bus, EVENT_LEVEL_COMPLETE, on_level_complete, NULL);


  /* ^^^   [ Subscribe ] ^^^ */

  /* vvv   [ Simulating Game Frame ] vvv */

  Event death = event_entity_died(42, 1, 0);
  death.timestamp = 3.14f;

  event_queue_push(&queue, &death);

  Event lvl = {0};
  lvl.type = EVENT_LEVEL_COMPLETE;
  lvl.data.level.level = 1;
  lvl.data.level.completion_time = 99.0f;
  lvl.data.level.stars = 69;

  event_queue_push(&queue, &lvl);

  printf("Flushing %d queued events ---\n ", event_queue_count(&queue));
  event_queue_flush(&queue);

  /* ^^^   [ Simulating Game Frame ] ^^^ */

  /* vvv   [ Unsubscibe Listeners ] vvv */

  event_unsubscribe(&bus, EVENT_ENTITY_DIED, on_entity_died);
  event_unsubscribe(&bus, EVENT_ENTITY_DIED, on_entity_died_audio);
  event_unsubscribe(&bus, EVENT_LEVEL_COMPLETE, on_level_complete);

  /* ^^^   [ Unsubscibe Listeners ] ^^^ */

  return 0;
}

/* ^^^ [ Entry Point ] ^^^ */

/* vim:set ts=3 sw=2 sts=2 et: */
