#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H
#include "events.h"
#include "event_bus.h"

#define EVENT_QUEUE_CAPACITY 256

typedef struct {
  Event buffer[EVENT_QUEUE_CAPACITY];
  int head;
  int tail;
  int count;
  EventBus *bus; /* Dispatches to bus on flush */
} EventQueue;

void event_queue_init(EventQueue *q, EventBus *bus);
int event_queue_push(EventQueue *q, const Event *event);
void event_queue_flush(EventQueue *q);
int event_queue_count(const EventQueue *q);

#endif
/* vim:set ts=3 sw=2 sts=2 et: */
