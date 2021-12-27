#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <unistd.h>

#include "queue.h"

#define TASK_LENGHT pow(10, 5) /* 0.1s */
#define TASK_COUNT 100
#define THREADS_CAP 4

struct task_head {
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    struct queue_head head;
};

struct task_entry {
    int data;
    struct queue_head head;
};

/* time: TASK_LENGHT * TASKS_COUNT / THREADS_CAP */
void *process_task(void *data) {
    struct task_head *tasks = data;
    struct task_entry *entry;

    while (1) {
        pthread_mutex_lock(&tasks->mtx);

        while (queue_empty(&tasks->head))
            pthread_cond_wait(&tasks->cond, &tasks->mtx);

        entry = dequeue_entry(&tasks->head, struct task_entry, head);
        pthread_mutex_unlock(&tasks->mtx);

        if (entry->data == -1) {
            free(entry);
            return NULL;
        }

        printf("%lu => %d\n", pthread_self(), entry->data);
        free(entry);
        usleep(TASK_LENGHT);
    }   
}

int main() {
    pthread_t threads[THREADS_CAP];
    struct task_entry *entry;
    struct task_head tasks;
    size_t i;

    pthread_mutex_init(&tasks.mtx, NULL);
    pthread_cond_init(&tasks.cond, NULL);
    queue_head_init(&tasks.head);

    for (i = 0; i < THREADS_CAP; i++)
        pthread_create(threads + i, NULL, process_task, &tasks);

    for (i = 0; i < TASK_COUNT; i++) {
        entry = malloc(sizeof(struct task_entry));
        entry->data = i;
        pthread_mutex_lock(&tasks.mtx);
        enqueue(&entry->head, &tasks.head);
        pthread_mutex_unlock(&tasks.mtx);
        pthread_cond_signal(&tasks.cond);
    }

    for (i = 0; i < THREADS_CAP; i++) {
        entry = malloc(sizeof(struct task_entry));
        entry->data = -1;
        pthread_mutex_lock(&tasks.mtx);
        enqueue(&entry->head, &tasks.head);
        pthread_mutex_unlock(&tasks.mtx);
        pthread_cond_signal(&tasks.cond);
    }

    for (i = 0; i < THREADS_CAP; i++)
        pthread_join(threads[i], NULL);

    pthread_cond_destroy(&tasks.cond);
    pthread_mutex_destroy(&tasks.mtx);
    return 0;
}
