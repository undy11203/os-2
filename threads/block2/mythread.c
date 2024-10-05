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
#include <unistd.h>

#include "mythread.h"

int mythread_startup(void *arg) {
  mythread_struct_t *mythread = (mythread_t)arg;

  getcontext(&(mythread->before_start_routine));

  if (!mythread->canceled)
    mythread->retval = mythread->start_routine(mythread->arg);
  /* printf("%s\n", mythread->retval); */

  mythread->finished = 1;

  while (!mythread->joined) {
    usleep(1);
  }
  /* printf("Next\n"); */

  if (munmap(mythread->stack, STACK_SIZE - PAGE) == -1) {
    perror("munmap");
  }

  return 0;
}

void *create_stack(off_t size) {
  int fd = open("my_file.bin", O_RDWR | O_CREAT | O_TRUNC, 0666);
  if (fd == -1) {
    perror("open");
    return NULL;
  }
  if (ftruncate(fd, size) == -1) {
    perror("ftruncate");
    close(fd);
    return NULL;
  }

  void *stack;

  stack = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (stack == MAP_FAILED) {
    perror("mmap");
    return NULL;
  }

  memset(stack, 0xFF, size);

  return stack;
}

int mythread_create(mythread_t *mytid, void *(start_routine)(void *),
                    void *arg) {
  static int thread_num = 0;

  mythread_struct_t *mythread;
  void *child_stack;
  int flags;
  child_stack = create_stack(STACK_SIZE);
  if (child_stack == NULL) {
    fprintf(stderr, "create_stack() failed\n");
    return -1;
  }

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
  if (child_pid == -1) {
    perror("clone");
    return -1;
  }
  *mytid = mythread;
  printf("tid = %d\n", (*mytid)->id);

  return 0;
}

void mythread_join(mythread_t mytid, void **retval) {
  mythread_t mythread = mytid;
  /* printf("%d\n", mythread->finished); */
  while (!mythread->finished) {
    usleep(1);
  }

  *retval = mythread->retval;

  mythread->joined = 1;
  mythread_testcancel(mytid);
}

void mythread_cancel(mythread_t mytid) {
  mytid->retval = (void *)"canceled";
  mytid->canceled = 1;
}

void mythread_testcancel(mythread_t mytid) {
  if (mytid->canceled) {
    /* printf("Yep\n"); */
    setcontext(&(mytid->before_start_routine));
  }
}

int mythread_equal(mythread_t m1, mythread_t m2) {
  return m1->id == m2->id ? 1 : 0;
}
