#define _GNU_SOURCE
#include <errno.h>
#include <malloc.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread(void *arg) {
  int *value = (int *)malloc(sizeof(int));
  printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(),
         gettid());
  *value = 42;
  return value;
}

void *mythread1(void *arg) {
  char *value = "hello, world";
  printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(),
         gettid());
  return value;
}

int main() {
  void *ref_val;
  pthread_t tid;
  int err;

  printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

  err = pthread_create(&tid, NULL, mythread, NULL);
  if (err) {
    printf("main: pthread_create() failed: %s\n", strerror(err));
    return -1;
  }
  pthread_join(tid, &ref_val);

  printf("%d\n", *(int *)ref_val);

  err = pthread_create(&tid, NULL, mythread1, NULL);
  if (err) {
    printf("main: pthread_create() failed: %s\n", strerror(err));
    return -1;
  }
  pthread_join(tid, &ref_val);

  printf("%s\n", (char *)ref_val);

  return 0;
}
