#pragma once
#include <stdatomic.h>

typedef atomic_int mutex_t;

int init_mutex(mutex_t* mutex);

void destroy_mutex(mutex_t* mutex);

void lock_mutex(mutex_t* mutex);

void unlock_mutex(mutex_t* mutex);