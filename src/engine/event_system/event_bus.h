#ifndef EVENT_BUS_H
#define EVENT_BUS_H


/* Holds a fixed 2D table of listeners, one row per event type and up to
 * MAX_LISTENERS per row. */

#include "./events.h"

/* Should be a power of 2 */
#define MAX_LISTENERS 16

/*
 * Callback signature
 *
 * const Event *e - fired event (readonly)
 * void *ctx - caller supplied context
 *
 * contex pointer eliminates need for global state in listeners
 *
 * */

typedef void (*EventCallback)(const Event *event, void* ctx);

typedef struct {
  EventCallback callback;
  void *ctx;
} Listener;

typedef struct{
  Listener listeners[EVENT_COUNT][MAX_LISTENERS];
  int count[EVENT_COUNT];
} EventBus;

/* vvv [ API ] vvv */

void event_bus_init(EventBus *bus);
int event_subscribe(EventBus *bus, EventType type, EventCallback cb, void *ctx);
void event_unsubscribe(EventBus *bus, EventType type, EventCallback cb);
void event_publish(EventBus *bus, const Event *event);

/* ^^^ [ API ] ^^^ */


#endif
/* vim:set ts=3 sw=2 sts=2 et: */
