#include "event_bus.h"
#include "events.h"
#include <stdio.h>
#include <string.h>

void event_bus_init(EventBus *bus) { memset(bus, 0, sizeof(*bus)); }

/* event_subscribe */
/* returns 0 on succeed, -1 if listener table for this type is full */
int event_subscribe(EventBus *bus, EventType type, EventCallback cb,
                    void *ctx) {
  if (type <= EVENT_NONE || type >= EVENT_COUNT) {
    return -1;
  }

  int n = bus->count[type];
  if (n >= MAX_LISTENERS) {
    fprintf(stderr, "[EventBus] listener table full for type %d\n", type);
    return -1;
  }

  bus->listeners[type][n].callback = cb;
  bus->listeners[type][n].ctx = ctx;
  bus->count[type]++;

  return 0;
}

/*
 * Event unsubscribe
 * removes the first listener matching cb.
 * Uses swap with last for O(1) removal.
 * */

void event_unsubscribe(EventBus *bus, EventType type, EventCallback cb) {
  if (type <= EVENT_NONE || type >= EVENT_COUNT) {
    return;
  }

  int n = bus->count[type];

  for (int i = 0; i < n; i++) {
    if (bus->listeners[type][i].callback == cb) {
      bus->listeners[type][i] = bus->listeners[type][n - 1];
      bus->count[type]--;
      return;
    }
  }
}

/*
 * Event Publish
 * Immediatly invokes all listeners registered for event type.
 * snapshot the count before iterating so that listeners added during dispatch
 * are not called in the same frame.
 * */
void event_publish(EventBus *bus, const Event *event) {
  if (event->type <= EVENT_NONE || event->type >= EVENT_COUNT) {
    return;
  }

  int n = bus->count[event->type];
  for (int i = 0; i < n; i++) {
    Listener *l = &bus->listeners[event->type][i];
    l->callback(event, l->ctx);
  }
}

/* vim:set ts=3 sw=2 sts=2 et: */
