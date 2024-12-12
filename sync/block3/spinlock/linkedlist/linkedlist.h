#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include "../spinlock/spinlock.h"

#define LENGTH 100

typedef struct _Node {
    char value[LENGTH];
    struct _Node* next;
    spinlock_t sync;
} Node;

typedef struct _Storage {
    Node* first;
} Storage;

Storage* storage_init();
void storage_add(Storage* storage, const char* value);
void storage_print(Storage* storage);
void storage_destroy(Storage* storage);


#define LOCK_INIT(lock) init_spinlock(lock)
#define LOCK_LOCK(lock) lock_spinlock(lock)
#define LOCK_UNLOCK(lock) unlock_spinlock(lock)
#define LOCK_DESTROY(lock) destroy_spinlock(lock)

#endif