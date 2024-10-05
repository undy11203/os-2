
#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void signal_handler(int sig_num) {
  printf("Caught signal %d, tid %d\n", sig_num, gettid());
}

/* void *blockall(void *arg) { */
/*   printf("I'm block all %d\n", gettid()); */
/*   sigset_t set_all_signal; */
/*   sigfillset(&set_all_signal); */
/*   pthread_sigmask(SIG_BLOCK, &set_all_signal, NULL); */
/*   sleep(30); */
/*   return NULL; */
/* } */

void *recivesigint(void *arg) {
  printf("I'm recive SIGINT %d\n", gettid());
  sigset_t set_all_sig;

  sigfillset(&set_all_sig);
  pthread_sigmask(SIG_SETMASK, &set_all_sig, NULL);
  sigdelset(&set_all_sig, SIGINT);
  /* sigdelset(&set_all_sig, SIGQUIT); */

  pthread_sigmask(SIG_SETMASK, &set_all_sig, NULL);
  signal(SIGINT, signal_handler);

  sleep(30);
  return NULL;
}

void *recievesigout(void *arg) {
  printf("I'm recive SIGQUIT %d\n", gettid());
  sigset_t set_all_sig;
  int sig;

  sigfillset(&set_all_sig);
  sigdelset(&set_all_sig, SIGQUIT);
  pthread_sigmask(SIG_SETMASK, &set_all_sig, NULL);

  sigemptyset(&set_all_sig);
  sigaddset(&set_all_sig, SIGQUIT);

  /* signal(SIGQUIT, signal_handler); */

  sigwait(&set_all_sig, &sig);

  printf("Signal recive, %d\n", getpid());
  /* while (1) { */
  /* } */
  /* sleep(3); */

  return NULL;
}

int main() {
  pthread_t tid;
  int err;

  /* printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(),
   * gettid()); */

  /* err = pthread_create(&tid, NULL, blockall, NULL); */
  /* if (err) { */
  /*   printf("main: pthread_create() failed: %s\n", strerror(err)); */
  /*   return -1; */
  /* } */

  sigset_t set_all_signal;
  sigfillset(&set_all_signal);
  pthread_sigmask(SIG_BLOCK, &set_all_signal, NULL);

  err = pthread_create(&tid, NULL, recivesigint, NULL);
  if (err) {
    printf("main: pthread_create() failed: %s\n", strerror(err));
    return -1;
  }

  err = pthread_create(&tid, NULL, recievesigout, NULL);
  if (err) {
    printf("main: pthread_create() failed: %s\n", strerror(err));
    return -1;
  }

  // pthread_join(tid, NULL);
  sleep(30);
  return 0;
}
