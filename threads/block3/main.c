#define _GNU_SOURCE

#include "uthread.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread_func_1(void *arg) {
  char *str = (char *)arg;
  // printf("mythread1 started -- ");
  for (int i = 0; i < 5; i++) {
    printf("hello in mythread1: %s %d\n", str, i);
    sleep(1);
    uthread_scheduler();
  }
  return "hello1";
}

void *mythread_func_2(void *arg) {
  char *str = (char *)arg;
  // printf("mythread2 started -- ");
  for (int i = 0; i < 5; i++) {
    printf("hello in mythread2: %s %d\n", str, i);
    sleep(1);
    uthread_scheduler();
  }
  return "hello2";
}

void *mythread_func_3(void *arg) {
  char *str = (char *)arg;
  // printf("mythread3 started -- ");
  for (int i = 0; i < 5; i++) {
    printf("hello in mythread3: %s %d\n", str, i);
    sleep(1);
    uthread_scheduler();
  }
  return "hello3";
}

int main() {
  char *arg[3] = {"arg 1", "arg 2", "arg 3"};
  printf("main: %s %s %s\n", arg[0], arg[1], arg[2]);
  uthread_t ut[3];
  uthread_t main_thread = malloc(sizeof(uthread_t));

  getcontext(&main_thread->ucontext);
  list_thread.uthreads[0] = main_thread;
  list_thread.uthread_count = 1;
  list_thread.uthread_cur = 0;

  uthread_create(&ut[0], mythread_func_1, arg[0]);
  uthread_create(&ut[1], mythread_func_2, arg[1]);
  uthread_create(&ut[2], mythread_func_3, arg[2]);

  printf("create finish\n");

  int isFinish = 1;

  while (1) {
    printf("main sheduler\n");
    if (uthread_scheduler() == 1)
      break;
  }
  printf("finish work programm\n");
}
