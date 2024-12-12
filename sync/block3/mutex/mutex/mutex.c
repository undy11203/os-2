#include "mutex.h"
#include <linux/futex.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <errno.h>
#include <error.h>

int futex(int* uaddr, int futex_op, int val,
	const struct timespec* timeout, int* uaddr2, int val3) {

	return syscall(SYS_futex, uaddr, futex_op, val,
		timeout, uaddr2, val3);
}

int init_mutex(mutex_t* mutex) {
    atomic_store(mutex, 0);
    return 0;
}

void destroy_mutex(mutex_t* mutex) {
    int expected = 0;

    if (atomic_compare_exchange_strong(mutex, &expected, 0)) {}
    else {printf("Error");}
}

void lock_mutex(mutex_t* mutex) {
    int compare = 0;
    atomic_compare_exchange_strong(mutex, &compare, 1);
    if (compare != 0) {
        if(compare != 2) {
            compare = atomic_exchange(mutex, 2);
        }
        while(compare != 0) {
            // printf("mutex1 = %d\n", *mutex);
            int res = futex((int*)&mutex, FUTEX_WAIT, 2, NULL, NULL, 0);
            // printf("mutex2 = %d\n", *mutex);
            compare = atomic_exchange(mutex, 2);
        }
    }
}

void unlock_mutex(mutex_t* mutex) {
    // if (atomic_fetch_sub(mutex, 1) != 1) {
        atomic_store(mutex, 0);
        futex((int*)&mutex, FUTEX_WAKE, 1, NULL, NULL, 0);
    // }
}

// #include "mutex.h"
// #include <linux/futex.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/syscall.h>
// #include <stdio.h>
// #include <errno.h>
// #include <error.h>

// int futex(int* uaddr, int futex_op, int val,
// 	const struct timespec* timeout, int* uaddr2, int val3) {

// 	return syscall(SYS_futex, uaddr, futex_op, val,
// 		timeout, uaddr2, val3);
// }

// int init_mutex(mutex_t* mutex) {
//     atomic_store(mutex, 0);
//     return 0;
// }

// void destroy_mutex(mutex_t* mutex) {
//     int expected = 0;

//     if (atomic_compare_exchange_strong(mutex, &expected, 0)) {}
//     else {printf("Error");}
// }

// void lock_mutex(mutex_t* mutex) {
//     int compare = 0;
//     if (!atomic_compare_exchange_strong(mutex, &compare, 1)) {
//         do {
//             int tmp = 1;
//             if (compare == 2 || !atomic_compare_exchange_strong(mutex, &tmp, 2)) {
//                 if(futex((int*)&mutex, FUTEX_WAIT, 2, 0, 0, 0) == -1){
//                     //printf("Error %s\n", strerror(errno));
//                 }
//             }
//             compare = 0;
//         } while (!atomic_compare_exchange_strong(mutex, &compare, 2));
//     }

// }

// void unlock_mutex(mutex_t* mutex) {
//     if (atomic_fetch_sub(mutex, 1) != 1) {
//         atomic_store(mutex, 0);
//         futex((int*)&mutex, FUTEX_WAKE, 1, 0, 0, 0);
//     }
// }


// #include "mutex.h"
// #include <linux/futex.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/syscall.h>
// #include <stdio.h>
// #include <errno.h>
// #include <error.h>

// int futex(int* uaddr, int futex_op, int val,
// 	const struct timespec* timeout, int* uaddr2, int val3) {

// 	return syscall(SYS_futex, uaddr, futex_op, val,
// 		timeout, uaddr2, val3);
// }

// int init_mutex(mutex_t* mutex) {
//     atomic_store(mutex, 0);
//     return 0;
// }

// void destroy_mutex(mutex_t* mutex) {
//     int expected = 0;

//     if (atomic_compare_exchange_strong(mutex, &expected, 0)) {}
//     else {printf("Error");}
// }

// void lock_mutex(mutex_t* mutex) {
//     while (1) {
//         int compare = 0; 
//         if (atomic_compare_exchange_strong(mutex, &compare, 1)) {
//             return;
//         }
//         if(futex((int*)&mutex, FUTEX_WAIT, 1, 0, 0, 0) == -1){
//             //printf("Failed to wait on futex: %s\n", strerror(errno));
//         }
//     }
// }

// void unlock_mutex(mutex_t* mutex) {
//     if (atomic_fetch_sub(mutex, 1) == 0) {
//         if(futex((int*)&mutex, FUTEX_WAKE, 1, 0, 0, 0) == -1){
//             //printf("Failed to wake on futex: %s\n", strerror(errno));
//         }
//     }
// }