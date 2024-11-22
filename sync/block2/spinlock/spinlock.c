#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "linkedlist/linkedlist.h"

int ascending_counter = 0;
int descending_counter = 0;
int equal_counter = 0;

int swap_counter = 0;

void* ascending_length_count(void* arg) {
    Storage* storage = (Storage *) arg;

    while (1) { 
        int k = 0;
        Node* current = storage->first;
        LOCK_LOCK(&current->sync);
        while (current->next != NULL) {
                LOCK_LOCK(&current->next->sync);
                size_t currentLength = strlen(current->value);
                size_t nextLength = strlen(current->next->value);

                if (currentLength < nextLength) {
                    k++;
                }
                Node *prev = current;
                current = current->next;
                LOCK_UNLOCK(&prev->sync);
        }
        LOCK_UNLOCK(&current->sync);
        ascending_counter++;

        // printf("ASC: %d, k = %d\n", ascending_counter, k);
    }

    return NULL;
}

void* descending_length_count(void* arg) {
    Storage* storage = (Storage *) arg;

    while (1) {
        int k = 0;
        Node* current = storage->first;
        LOCK_LOCK(&current->sync);
        while (current->next != NULL) {
                LOCK_LOCK(&current->next->sync);
                size_t currentLength = strlen(current->value);
                size_t nextLength = strlen(current->next->value);

                if (currentLength > nextLength) {
                    k++;
                }
                Node *prev = current;
                current = current->next;
                LOCK_UNLOCK(&prev->sync);
        }
        LOCK_UNLOCK(&current->sync);
        descending_counter++;

        // printf("DESC: %d, k = %d\n", descending_counter, k);
    }

    return NULL;
}

void* equal_length_count(void* arg) {
    Storage* storage = (Storage *) arg;

    while (1) {
        int k = 0;
        Node* current = storage->first;
        LOCK_LOCK(&current->sync);
        while (current->next != NULL) {
                LOCK_LOCK(&current->next->sync);
                size_t currentLength = strlen(current->value);
                size_t nextLength = strlen(current->next->value);

                if (currentLength == nextLength) {
                    k++;
                }
                Node *prev = current;
                current = current->next;
                LOCK_UNLOCK(&prev->sync);
        }
        LOCK_UNLOCK(&current->sync);
        equal_counter++;

        // printf("EQUALS: %d, k = %d\n", equal_counter, k);
    }

    return NULL;
}

void* random_swap(void* arg) {
    Storage* storage = (Storage *) arg;
    unsigned int seed = (unsigned int)pthread_self();
	
    while (1) {
        int k = 0;
        Node* prev = storage->first;
        LOCK_LOCK(&prev->sync);
        Node* current = prev->next;

        if (current == NULL) {
            LOCK_UNLOCK(&prev->sync);
            continue;
        }

        LOCK_LOCK(&current->sync);

        while (current->next != NULL) {
            Node* next = current->next;
            LOCK_LOCK(&next->sync);
            
            if (rand_r(&seed) % 2 == 0) {
                Node* tmp1 = current;
                Node* tmp2 = next;

                tmp1->next = tmp2->next;
                tmp2->next = tmp1;
                prev->next = tmp2;

                k++;
                LOCK_UNLOCK(&prev->sync);
                prev = next;
            } else {
                LOCK_UNLOCK(&prev->sync);
                prev = current;
                current = next;
            }

        }

        if (current != NULL) {
            LOCK_UNLOCK(&current->sync);
        }

        LOCK_UNLOCK(&prev->sync);
        swap_counter++;

        // printf("SWAP: %d k = %d\n", swap_counter, k);
    }

    return NULL;
}

void *count_monitor(void *arg) {
    while (1) {
        printf("Ascending: %d, Descending: %d, Equal: %d Swap: %d\n", 
                    ascending_counter, descending_counter, equal_counter, swap_counter);
        sleep(1);
    }

    return NULL;
}

void fill_storage(Storage* storage, int num_nodes) {
    for (int i = 0; i < num_nodes; ++i) {
        char buff[99];
        sprintf(buff, "%d", i);
        storage_add(storage, buff);
    }
}

int main() {
    Storage* storage = storage_init();
    int err;

    fill_storage(storage, 10000);

    pthread_t monitor;
    pthread_t equal_thread, ascending_thread, descending_thread;
    pthread_t swap1, swap2, swap3;

    err = pthread_create(&equal_thread, NULL, equal_length_count, storage);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}
	err = pthread_create(&ascending_thread, NULL, ascending_length_count, storage);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}
    err = pthread_create(&descending_thread, NULL, descending_length_count, storage);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

    err = pthread_create(&monitor, NULL, count_monitor, NULL);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

    err = pthread_create(&swap1, NULL, random_swap, storage);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}
    err = pthread_create(&swap2, NULL, random_swap, storage);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}
    err = pthread_create(&swap3, NULL, random_swap, storage);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}
 
	if (pthread_join(equal_thread, NULL)) {
		perror("pthread_join - equal_thread");
	}
	if (pthread_join(ascending_thread, NULL)) {
		perror("pthread_join - ascending_thread");
	}
    if (pthread_join(descending_thread, NULL)) {
		perror("pthread_join - descending_thread");
	}
    if (pthread_join(monitor, NULL)) {
		perror("pthread_join - monitor");
	}

    pthread_join(swap1, NULL);
    pthread_join(swap2, NULL);
    pthread_join(swap3, NULL);

    storage_destroy(storage);

    return 0;
}