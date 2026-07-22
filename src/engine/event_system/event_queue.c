#include "event_queue.h"
#include "event_bus.h"
#include <stdio.h>

void event_queue_init(EventQueue *q, EventBus *bus) {
  q->head = 0;
  q->tail = 0;
  q->count = 0;
  q->bus = bus;
}
/* Event queue push
 * Adds an event to the ring buffer
 * returns 0 on success and -1 if the buffer is full
 * On overflow, the event is dropped
 * consider increasing EVENT_QUEUE_CAPACITY or flushing
 * more frequently
 * */
int event_queue_push(EventQueue *q, const Event *event) {
  if (q->count >= EVENT_QUEUE_CAPACITY) {
    fprintf(stderr, "[EventQueue] Overflow! Dropping event type %d\n",
            event->type);
    return -1;
  }
  q->buffer[q->tail] = *event;
  q->tail = (q->tail + 1) % EVENT_QUEUE_CAPACITY;
  q->count++;
  return 0;
}

/*
 * Event queue flush
 * Dispatches all   queued events through the bus, then clears the queue.
 * New events pushed from within callbacks are queued for the next flush
 * */
void event_queue_flush(EventQueue *q) {
  /*Snapshot count so events fired during callbacks wait until next flush */
  int to_process = q->count;
  while (to_process-- > 0) {
    Event e = q->buffer[q->head];
    q->head = (q->head + 1) % EVENT_QUEUE_CAPACITY;
    q->count--;
    event_publish(q->bus, &e);
  }
}

int event_queue_count(const EventQueue *q) { return q->count; }

/* [ Usage example ]
while(game_running){
  poll_input(&input, &queue); // pushes EVENT_KEY_*
  game_update(&state, &queue); // pushes gameplay events
  event_queue_flush(&queue); // all listeners fire here
  render(&state);
}
*/


/* vim:set ts=3 sw=2 sts=2 et: */
