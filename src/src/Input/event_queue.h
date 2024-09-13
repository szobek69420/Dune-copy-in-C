#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "event.h"

#define EVENT_QUEUE_MAX_SIZE 50

typedef struct {
    int front, back, size;
    Event data[EVENT_QUEUE_MAX_SIZE];
} EventQueue;

void eventQueue_init();

void eventQueue_push(Event e);
Event eventQueue_poll();

int eventQueue_isEmpty();
int eventQueue_isFull();
int eventQueue_getSize();

Event eventQueue_front();
Event eventQueue_back();

//a kepernyo meretenek megvaltoztatasanal kb pixelenkent lefut a callback, ami nagyon gyorsan feltolti a queue-t (es akkor elvesznek a legfrissebb meretek)
//szoval a window resize callback-ben amennyiben az utolso event is egy window resize, csak kicserelem az uj eventre
void eventQueue_swapBack(Event e);

#endif