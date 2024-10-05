#define _GNU_SOURCE
#include <errno.h>
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
  /* pthread_detach(pthread_self()); */
  printf("mythread print struct_data %d %s\n", ((struct MyStruct *)arg)->a,
         ((struct MyStruct *)arg)->b);
  return NULL;
}

int main() {
  pthread_t tid;
  int err;
  struct MyStruct mystruct = {10, "hello, world"};

  printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

  err = pthread_create(&tid, NULL, mythread, &mystruct);
  if (err) {
    printf("main: pthread_create() failed: %s\n", strerror(err));
    return -1;
  }
  /* sleep(5); */
  pthread_join(tid, NULL);
  // sleep(1);
  return 0;
}
