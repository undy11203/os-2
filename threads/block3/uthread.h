#ifndef UTHREAD_H
#define UTHREAD_H
#include <ucontext.h>

#define PAGE 4096
#define STACK_SIZE (PAGE * 8)
#define MAX_THREADS 10

typedef struct {
  int uthread_id;

  void *(*start_routine)(void *);

  void *arg;
  void *retval;

  volatile int finished;
  ucontext_t ucontext;
} uthread_struct_t;

typedef uthread_struct_t *uthread_t;

typedef struct _uthread_list_t {
  uthread_t uthreads[MAX_THREADS];
  int uthread_count;
  int uthread_cur;
} uthread_list_t;

extern uthread_list_t list_thread;

int uthread_create(uthread_t *uthread, void(*start_routine), void *arg);
int thread_is_finished(uthread_t utid);
int uthread_scheduler();

#endif
