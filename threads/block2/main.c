
#include "mythread.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *mythread1(void *arg) {
  char *str = (char *)arg;

  printf("args = %s\n", str);

  char *res = "lol kek";

  return res;
}

int main() {
  mythread_t tid;
  printf("It is main\n\n");
  int err = mythread_create(&tid, mythread1, "hello world");
  if (err == -1) {
    printf("mypthread_create() failed\n");
    return -1;
  }

  sleep(2);
  mythread_cancel(tid);
  void *res;
  mythread_join(tid, &res);
  printf("res = %s\n", (char *)res);
}
