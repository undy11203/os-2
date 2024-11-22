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

#include "mythread.h"

/* mythread_t gtid; */

int mythread_startup(void *arg) {
  mythread_struct_t *mythread = (mythread_t)arg;

  getcontext(&(mythread->before_start_routine));

  if (!mythread->canceled)
    mythread->retval = mythread->start_routine(mythread->arg);

  mythread->finished = 1;

  while (!mythread->joined) {
    usleep(1);
  }

  if (munmap(mythread->stack, STACK_SIZE - PAGE) == -1) {
    perror("munmap");
  }
  free(mythread);
  size--;

  return 0;
}

void *create_stack(off_t size) {
  int fd = open("my_file.bin", O_RDWR | O_CREAT | O_TRUNC, 0666);
  if (fd == -1) {
    perror("open");
    return NULL;
  }
  ftruncate(fd, 0);
  if (ftruncate(fd, size) == -1) {
    perror("ftruncate");
    close(fd);
    return NULL;
  }

  void *stack;

  stack = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (stack == MAP_FAILED) {
    perror("mmap");
    return NULL;
  }
  close(fd);

  return stack;
}

int mythread_create(mythread_t *mytid, void *(start_routine)(void *),
                    void *arg) {
  if (size >= MAX_THREAD) {
    return -1;
  }
  static int thread_num = 0;

  mythread_struct_t *mythread;
  void *child_stack;
  int flags;
  child_stack = create_stack(STACK_SIZE);
  /* printf("%p\n", child_stack); */
  if (child_stack == NULL) {
    fprintf(stderr, "create_stack() failed\n");
    return -1;
  }
  mprotect(child_stack + PAGE, STACK_SIZE - PAGE, PROT_READ | PROT_WRITE);
  memset(child_stack + PAGE, 'a', STACK_SIZE - PAGE);

  mythread = (mythread_struct_t *)(child_stack + STACK_SIZE -
                                   sizeof(mythread_struct_t));

  mythread->start_routine = start_routine;
  mythread->arg = arg;
  mythread->retval = NULL;
  mythread->joined = 0;
  mythread->finished = 0;
  mythread->stack = child_stack;
  mythread->id = thread_num;
  mythread->canceled = 0;
  thread_num++;

  child_stack = (void *)mythread;
  flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
  int child_pid = clone(mythread_startup, child_stack, flags, (void *)mythread);
  printf("%d\n", child_pid);
  for (int i = 0; i < MAX_THREAD; i++) {
    if (table[i].thread == NULL) {
      table[i].thread_id = child_pid;
      table[i].thread = mythread;
      size++;
      printf("create %d\n", size);
      break;
    }
  }
  /* gtid = mythread; */
  if (child_pid == -1) {
    perror("clone");
    return -1;
  }
  *mytid = mythread;

  return 0;
}

void mythread_join(mythread_t mytid, void **retval) {
  /* printf("%d\n", mythread->finished); */
  while (!mytid->finished) {
    usleep(1);
  }

  *retval = mytid->retval;
  mytid->joined = 1;
}

void mythread_cancel(mythread_t mytid) {
  mytid->retval = "canceled";
  mytid->canceled = 1;
}

void mythread_testcancel(void) {
  mythread_t thread = NULL;
  int tid = gettid();
  for (int i = 0; i < size; i++) {
    if (table[i].thread_id == tid) {
      thread = table[i].thread;
      break;
    }
  }

  if (thread != NULL && thread->canceled) {
    setcontext(&(thread->before_start_routine));
  }
}

int mythread_equal(mythread_t m1, mythread_t m2) {
  return m1->id == m2->id ? 1 : 0;
}

int mythread_self() {
  int tid = gettid();
  return tid;
}
