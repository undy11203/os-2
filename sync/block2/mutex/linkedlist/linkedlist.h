#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#define LENGTH 100

typedef struct _Node {
    char value[LENGTH];
    struct _Node* next;
    pthread_mutex_t sync;
} Node;

typedef struct _Storage {
    Node* first;
} Storage;

Storage* storage_init();
void storage_add(Storage* storage, const char* value);
void storage_print(Storage* storage);
void storage_destroy(Storage* storage);

void swap_nodes(Node* prev, Node* current, Node* next);

#define LOCK_INIT(lock) pthread_mutex_init(lock, NULL)
#define LOCK_LOCK(lock) pthread_mutex_lock(lock)
#define LOCK_UNLOCK(lock) pthread_mutex_unlock(lock)
#define LOCK_DESTROY(lock) pthread_mutex_destroy(lock)

#endif