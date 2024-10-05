#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static int global_value = 20;

void *mythread(void *arg) {
  int local = 15;
  const int const_local = 5;
  static int static_local = 10;
  printf("mythread [%d %d %d %lu]: Hello from mythread!\n", getpid(), getppid(),
         gettid(), pthread_self());
  printf("mythread: address local: %p, static_local: %p, const_local: %p, "
         "global_value: %p value "
         "local: %d, static_local: %d, const_local: %d, "
         "global_value: %d\n\n",
         &local, &static_local, &const_local, &global_value, local,
         static_local, const_local, global_value);
  local++;
  global_value++;
  static_local++;
  while (1) {
  }
  return NULL;
}

int main() {
  pthread_t tid[5];
  int err;

  int local = 15;
  const int const_local = 5;
  static int static_local = 10;

  printf("main [%d %d %d %lu]: Hello from main!\n", getpid(), getppid(),
         gettid(), pthread_self());
  printf("main: address local: %p, static_local: %p, const_local: %p, "
         "global_value: %p\n\n",
         &local, &static_local, &const_local, &global_value);

  for (int i = 0; i < 5; i++) {
    err = pthread_create(&tid[i], NULL, mythread, NULL);
    if (err) {
      printf("main: pthread_create() failed: %s\n", strerror(err));
      return -1;
    }
  }
  for (int i = 0; i < 5; i++) {
    pthread_join(tid[i], NULL);
  }
  // sleep(1);
  return 0;
}
