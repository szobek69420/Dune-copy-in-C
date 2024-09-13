#include "event_queue.h"

#include <string.h>

static EventQueue queue;

void eventQueue_init()
{
    queue.front = queue.size = 0;
    queue.back = EVENT_QUEUE_MAX_SIZE - 1;
    memset(queue.data, 0, sizeof(Event) * EVENT_QUEUE_MAX_SIZE);
}

void eventQueue_push(Event e)
{
    if (eventQueue_isFull())
        return;
    queue.back = (queue.back + 1) % EVENT_QUEUE_MAX_SIZE;
    queue.data[queue.back] = e;
    queue.size++;
}
Event eventQueue_poll()
{
    if (eventQueue_isEmpty())
        return (Event){ 0 };
    Event e = queue.data[queue.front];
    queue.front = (queue.front + 1) % EVENT_QUEUE_MAX_SIZE;
    queue.size--;
    return e;
}

int eventQueue_isEmpty()
{
    return queue.size == 0;
}
int eventQueue_isFull()
{
    return queue.size == EVENT_QUEUE_MAX_SIZE;
}
int eventQueue_getSize()
{
    return queue.size;
}

Event eventQueue_front()
{
    if (eventQueue_isEmpty())
        return (Event){ 0 };
    return queue.data[queue.front];
}
Event eventQueue_back()
{
    if (eventQueue_isEmpty())
        return (Event){ 0 };
    return queue.data[queue.back];
}

void eventQueue_swapBack(Event e)
{
    if (eventQueue_isEmpty())
        return;
    queue.data[queue.back] = e;
}