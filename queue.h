#ifndef QUEUE_H__
#define QUEUE_H__

#include <stddef.h>

struct queue_head {
    struct queue_head *next, *prev;
};

#define QUEUE_HEAD_INIT(name) { &(name), &(name) }
#define queue_head_init(head) (head)->next = (head)->prev = head;
#define queue_entry(ptr, type, member) \
    (type *)((char *)(ptr) - offsetof(type, member))

void
queue_add(struct queue_head *new,
          struct queue_head *prev,
          struct queue_head *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

void
enqueue(struct queue_head *new, struct queue_head *head)
{
    queue_add(new, head->prev, head);
}

void
queue_del(struct queue_head *prev, struct queue_head *next)
{
    next->prev = prev;
    prev->next = next;
}

struct queue_head *
dequeue(struct queue_head *head)
{
    struct queue_head *first = head->next;
    queue_del(head, head->next->next);
    first->prev = first->next = NULL;
    return first;
}

#define dequeue_entry(head, type, member) \
    queue_entry(dequeue(head), type, member)

int
queue_empty(const struct queue_head *head)
{
    return head->next == head;
}

#endif
