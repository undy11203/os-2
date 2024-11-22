#define _GNU_SOURCE

#include "mythread.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread1(void *arg) {
  char *str = (char *)arg;

  /* printf("args = %s\n", str); */

  /* char *res = malloc(10); // Allocate memory dynamically */
  /* if (res != NULL) { */
  /*   snprintf(res, 10, "lol kek"); */
  /* } */
  /**/
  /* return res; */
  printf("mythread1 %d\n", mythread_self());
  while (1) {
    printf("%s\n", str);
    mythread_testcancel();
  }
}

void *mythread2(void *arg) {
  char *str = (char *)arg;

  /* printf("args = %s\n", str); */

  /* char *res = malloc(10); // Allocate memory dynamically */
  /* if (res != NULL) { */
  /*   snprintf(res, 10, "lol kek"); */
  /* } */
  /**/
  /* return res; */
  /* sleep(2); */
  printf("mythread2 %d\n", mythread_self());
  char *var = "hello";
  return var;
}

int main() {
  mythread_t tid;
  printf("It is main\n\n");
  int err = mythread_create(&tid, mythread1, "hello world");
  if (err == -1) {
    printf("mypthread_create() failed\n");
    return -1;
  }
  mythread_t tid1;
  err = mythread_create(&tid1, mythread2, "hello world1");
  if (err == -1) {
    printf("mypthread_create() failed\n");
    return -1;
  }

  sleep(1);
  mythread_cancel(tid);
  void *res;
  mythread_join(tid, &res);
  printf("res = %s\n", (char *)res);
  /* mythread_cancel(tid1); */
  mythread_join(tid1, &res);
  printf("res1 = %s\n", (char *)res);
}
