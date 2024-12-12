#include "spinlock.h"
#include <stdatomic.h>
#include <stdio.h>

int init_spinlock(spinlock_t* spinlock) {
    atomic_store(spinlock, 0);
    return 0;
}

void destroy_spinlock(spinlock_t* spinlock) {
    int expected = 0;

    if (atomic_compare_exchange_strong(spinlock, &expected, 0)) {}
    else {printf("Error");}
}

void lock_spinlock(spinlock_t* spinlock) {
    while (1) {
        int expected = 0;

        if (atomic_compare_exchange_strong(spinlock, &expected, 1)) {
            return;
        }
    }
}

void unlock_spinlock(spinlock_t* spinlock) {
    int expected = 1;
    atomic_compare_exchange_strong(spinlock, &expected, 0);
}