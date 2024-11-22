#define _GNU_SOURCE
#include <fcntl.h>
#include <linux/sched.h>
#include <malloc.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <ucontext.h>
#include <unistd.h>

#include "uthread.h"

uthread_list_t list_thread = {0};

void *create_stack(off_t size) {
  void *stack;

  stack = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
               -1, 0);
  if (stack == MAP_FAILED) {
    perror("mmap");
    return NULL;
  }

  return stack;
}

int uthread_startup(void *arg) {
  uthread_t uthread = (uthread_t)arg;
  void *retval = NULL;

  printf("uthread_startup: starting a thread func %d\n", uthread->uthread_id);

  uthread->retval = uthread->start_routine(uthread->arg);
  printf("res %s\n", (char *)uthread->retval);

  uthread->finished = 1;
  list_thread.uthread_count--;

  return 0;
}

int uthread_scheduler() {
  int err;
  ucontext_t *cur_ctx, *next_ctx;
  /* printf("sheduler cur %d\n", list_thread.uthread_cur); */

  cur_ctx = &(list_thread.uthreads[list_thread.uthread_cur]->ucontext);

  if (list_thread.uthread_count <= 1) {
    printf("es\n");
    return 1;
  }

  do {
    list_thread.uthread_cur = (list_thread.uthread_cur + 1) % MAX_THREADS;

  } while (list_thread.uthreads[list_thread.uthread_cur] == NULL ||
           list_thread.uthreads[list_thread.uthread_cur]->finished);

  next_ctx = &(list_thread.uthreads[list_thread.uthread_cur]->ucontext);
  err = swapcontext(cur_ctx, next_ctx);
  if (err == -1) {
    perror("sheduler: swapcontext() failed:");
    exit(1);
  }
  return 0;
}

int uthread_create(uthread_t *uthread, void(*start_routine), void *arg) {
  if (list_thread.uthread_count >= MAX_THREADS)
    return -1;
  static int uthread_id = 0;
  uthread_t new_uthread;
  void *stack;
  int err;

  printf("uthread_create: creating thread %d\n", uthread_id);

  stack = create_stack(STACK_SIZE);
  if (stack == NULL) {
    fprintf(stderr, "create_stack() failed\n");
    return -1;
  }
  new_uthread =
      (uthread_struct_t *)(stack + STACK_SIZE - sizeof(uthread_struct_t));
  err = getcontext(&new_uthread->ucontext);
  if (err == -1) {
    perror("uthread_create: getcontext() failed:");
    return -1;
  }

  new_uthread->ucontext.uc_stack.ss_sp = stack;
  new_uthread->ucontext.uc_stack.ss_size = STACK_SIZE - sizeof(uthread_t);
  new_uthread->ucontext.uc_link = &(list_thread.uthreads[0]->ucontext);

  makecontext(&new_uthread->ucontext, (void (*)(void))uthread_startup, 1,
              new_uthread);

  new_uthread->uthread_id = uthread_id;
  new_uthread->start_routine = start_routine;
  new_uthread->arg = arg;
  new_uthread->finished = 0;

  for (int i = 1; i < MAX_THREADS; i++) {
    if (list_thread.uthreads[i] == NULL) {
      list_thread.uthreads[i] = new_uthread;
      list_thread.uthread_count++;
      break;
    }
  }
  printf("thread create: thread count %d\n", list_thread.uthread_count);

  *uthread = new_uthread;

  uthread_id++;

  return 0;
}
