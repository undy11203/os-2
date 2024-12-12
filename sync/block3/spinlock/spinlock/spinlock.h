#pragma once
#include <stdatomic.h>


typedef atomic_int spinlock_t;

int init_spinlock(spinlock_t* spinlock);

void destroy_spinlock(spinlock_t* spinlock);

void lock_spinlock(spinlock_t* spinlock);

void unlock_spinlock(spinlock_t* spinlock);