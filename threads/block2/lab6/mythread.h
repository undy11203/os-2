#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <sys/types.h>
#include <sys/ucontext.h>

#include <ucontext.h>

#define PAGE 4096
#define STACK_SIZE (PAGE * 8)
#define MAX_THREAD 10

typedef void *(*start_routine_t)(void *);

typedef struct _mythread_t {
  int id;
  start_routine_t start_routine;
  void *arg;
  void *retval;
  void *stack;

  volatile int joined;
  volatile int finished;
  ucontext_t before_start_routine;

  volatile int canceled;
} mythread_struct_t;

typedef mythread_struct_t *mythread_t;

typedef struct {
  int thread_id;
  mythread_t thread;
} threads;

threads table[MAX_THREAD];
volatile int size = 0;

int mythread_startup(void *arg);
void *create_stack(off_t size);
int mythread_create(mythread_t *mytid, void *(start_routine)(void *),
                    void *arg);
void mythread_join(mythread_t mytid, void **retval);
void mythread_cancel(mythread_t mytid);
void mythread_testcancel(void);
int mythread_equal(mythread_t m1, mythread_t m2);
int mythread_self();

#endif
