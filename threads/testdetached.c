#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread(void *arg) {
  printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(),
         gettid());
  sleep(5);
  printf("yeee %d\n", *(int *)arg);
  return NULL;
}

int main() {
  pthread_t tid;
  int err;

  printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  int a = 5;
  err = pthread_create(&tid, NULL, mythread, &a);
  if (err) {
    printf("main: pthread_create() failed: %s\n", strerror(err));
    return -1;
  }
  a = 7;
  pthread_join(tid, NULL);
  sleep(1);
  return 0;
}
