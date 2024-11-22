#define _GNU_SOURCE
#include <assert.h>
#include <pthread.h>

#include "queue.h"

pthread_spinlock_t spinlock;

void *qmonitor(void *arg) {
  queue_t *q = (queue_t *)arg;

  printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

  while (1) {
    queue_print_stats(q);
    sleep(1);
  }

  return NULL;
}

queue_t *queue_init(int max_count) {

  int spinlock_initialization_status =
      pthread_spin_init(&spinlock, PTHREAD_PROCESS_SHARED);
  if (spinlock_initialization_status != 0) {
    fprintf(stderr, "Error during initializing of spinlock\n");
    abort();
  }

  queue_t *q = malloc(sizeof(queue_t));
  if (!q) {
    printf("Cannot allocate memory for a queue\n");
    abort();
  }

  q->first = NULL;
  q->last = NULL;
  q->max_count = max_count;
  q->count = 0;

  q->add_attempts = q->get_attempts = 0;
  q->add_count = q->get_count = 0;

  /* err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q); */
  /* if (err) { */
  /*   printf("queue_init: pthread_create() failed: %s\n", strerror(err)); */
  /*   abort(); */
  /* } */

  return q;
}

void queue_destroy(queue_t *q) {
  qnode_t *tmp;
  while (q->count) {
    tmp = q->first;
    q->first = q->first->next;
    free(tmp);
    q->count--;
  }

  /* int err = pthread_cancel(q->qmonitor_tid); */
  /* if (err) { */
  /*   printf("queue_destroy(): pthread_cancel() failed: %s\n", strerror(err));
   */
  /* } */

  free(q);

  int destroy_status = pthread_spin_destroy(&spinlock);
  if (destroy_status != 0) {
    fprintf(stderr, "Error during pthread_spin_destroy(); error code: %d\n",
            destroy_status);
  }
}

int queue_add(queue_t *q, int val) {
  qnode_t *new = malloc(sizeof(qnode_t));
  if (!new) {
    printf("Cannot allocate memory for new node\n");
    abort();
  }
  q->add_attempts++;
  int lock_status = pthread_spin_lock(&spinlock);
  if (lock_status != 0) {
    fprintf(stderr, "Error during pthread_spin_lock(); error code: %d\n",
            lock_status);
    return 1;
  }

  assert(q->count <= q->max_count);

  if (q->count == q->max_count) {
    int unlock_status = pthread_spin_unlock(&spinlock);
    if (unlock_status != 0) {
      fprintf(stderr, "Error during pthread_spin_unlock(); error code: %d\n",
              unlock_status);
    }
    return 0;
  }

  new->val = val;
  new->next = NULL;

  if (!q->first)
    q->first = q->last = new;
  else {
    q->last->next = new;
    q->last = q->last->next;
  }

  q->count++;
  q->add_count++;

  int unlock_status = pthread_spin_unlock(&spinlock);
  if (unlock_status != 0) {
    fprintf(stderr, "Error during pthread_spin_unlock(); error code: %d\n",
            unlock_status);
  }

  return 1;
}

int queue_get(queue_t *q, int *val) {
  int lock_status = pthread_spin_lock(&spinlock);
  if (lock_status != 0) {
    fprintf(stderr, "Error during pthread_spin_lock(); error code: %d\n",
            lock_status);
    return 0;
  }

  q->get_attempts++;

  assert(q->count >= 0);

  if (q->count == 0) {
    int unlock_status = pthread_spin_unlock(&spinlock);
    if (unlock_status != 0) {
      fprintf(stderr, "Error during pthread_spin_unlock(); error code: %d\n",
              unlock_status);
    }
    return 0;
  }

  qnode_t *tmp = q->first;

  *val = tmp->val;
  q->first = q->first->next;

  free(tmp);
  q->count--;
  q->get_count++;

  int unlock_status = pthread_spin_unlock(&spinlock);
  if (unlock_status != 0) {
    fprintf(stderr, "Error during pthread_spin_unlock(); error code: %d\n",
            unlock_status);
  }

  return 1;
}

void queue_print_stats(queue_t *q) {
  /* int lock_status = pthread_spin_lock(&spinlock); */
  /* if (lock_status != 0) { */
  /*   fprintf(stderr, "Error during pthread_spin_lock(); error code: %d\n", */
  /*           lock_status); */
  /*   return; */
  /* } */

  printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld "
         "%ld %ld)\n",
         q->count, q->add_attempts, q->get_attempts,
         q->add_attempts - q->get_attempts, q->add_count, q->get_count,
         q->add_count - q->get_count);

  /* int unlock_status = pthread_spin_unlock(&spinlock); */
  /* if (unlock_status != 0) { */
  /*   fprintf(stderr, "Error during pthread_spin_unlock(); error code: %d\n",
   */
  /*           unlock_status); */
  /*   abort(); */
  /* } */
}
