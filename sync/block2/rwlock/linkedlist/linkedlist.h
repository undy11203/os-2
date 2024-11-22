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
    pthread_rwlock_t sync;
} Node;

typedef struct _Storage {
    Node* first;
} Storage;

Storage* storage_init();
void storage_add(Storage* storage, const char* value);
void storage_print(Storage* storage);
void storage_destroy(Storage* storage);

void swap_nodes(Node* prev, Node* current, Node* next);

#define LOCK_INIT(lock) pthread_rwlock_init(lock, NULL)
#define LOCK_RLOCK(lock) pthread_rwlock_rdlock(lock)
#define LOCK_WLOCK(lock) pthread_rwlock_wrlock(lock)
#define LOCK_UNLOCK(lock) pthread_rwlock_unlock(lock)
#define LOCK_DESTROY(lock) pthread_rwlock_destroy(lock)

#endif