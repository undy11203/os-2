#define _GNU_SOURCE
#include <errno.h>
#include <malloc.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void mythread_handler(void *arg) {
  free(arg);
  printf("my thread handler\n");
}

void *mythread(void *arg) {
  char *text = (char *)malloc(sizeof(char) * 100);
  strcpy(text, "hello, world");
  pthread_cleanup_push(mythread_handler, text);
  while (1) {
    printf("my thread: print text %s\n", text);
  }
  pthread_cleanup_pop(1);
  return NULL;
}

int main() {
  pthread_t tid;
  int err;

  printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

  err = pthread_create(&tid, NULL, mythread, NULL);
  if (err) {
    printf("main: pthread_create() failed: %s\n", strerror(err));
    return -1;
  }
  sleep(1);
  pthread_cancel(tid);
  pthread_join(tid, NULL);
  return 0;
}
