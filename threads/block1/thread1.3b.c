#define _GNU_SOURCE
#include <errno.h>
#include <malloc.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

struct MyStruct {
  int a;
  char *b;
};

void *mythread(void *arg) {
  printf("mythread print struct_data %d %s\n", ((struct MyStruct *)arg)->a,
         ((struct MyStruct *)arg)->b);
  return NULL;
}

int main() {
  pthread_t tid;
  int err;

  struct MyStruct *mystruct =
      (struct MyStruct *)malloc(sizeof(struct MyStruct));
  mystruct->a = 10;
  mystruct->b = "hello, world";

  printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  err = pthread_create(&tid, &attr, mythread, mystruct);
  if (err) {
    printf("main: pthread_create() failed: %s\n", strerror(err));
    return -1;
  }

  sleep(1);
  return 0;
}
