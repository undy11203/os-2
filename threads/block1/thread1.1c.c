#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

pthread_t subthread;
static int global_value = 20;

void *mythread(void *arg) {
  int local = 15;
  const int const_local = 5;
  static int static_local = 10;
  printf("mythread [%d %d %d %lu]: Hello from mythread!\n", getpid(), getppid(),
         gettid(), pthread_self());
  printf("mythread: address local: %p, static_local: %p, const_local: %p, "
         "global_value: %p\n\n",
         &local, &static_local, &const_local, &global_value);
  subthread = pthread_self();
  return NULL;
}

int main() {
  pthread_t tid;
  int err;

  int local = 15;
  const int const_local = 5;
  static int static_local = 10;

  printf("main [%d %d %d %lu]: Hello from main!\n", getpid(), getppid(),
         gettid(), pthread_self());
  printf("main: address local: %p, static_local: %p, const_local: %p, "
         "global_value: %p\n\n",
         &local, &static_local, &const_local, &global_value);

  err = pthread_create(&tid, NULL, mythread, NULL);
  printf("new thread tid %lu\n\n", tid);
  if (err) {
    printf("main: pthread_create() failed: %s\n", strerror(err));
    return -1;
  }
  pthread_join(tid, NULL);
  if (pthread_equal(tid, subthread)) {
    printf("Equal\n");
  }
  // sleep(1);
  return 0;
}
