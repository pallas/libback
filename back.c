/*
 * libback - simple backtrace library
 * Author: Derrick Pallas
 *
 * gcc -std=c99 -fPIC -shared back.c -o libback.so -lsupc++
 * env LD_PRELOAD=./libback.so my_program
 */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ucontext.h>
#include <unistd.h>

/* #include <cxxabi.h> */
extern char*
__cxa_demangle(const char* __mangled_name,
               char* __output_buffer,
               size_t* __length,
               int* __status);

char * buf;
size_t len;

static void fn_info(FILE *f, void *fn) {
  fprintf(f, "[%16p]", fn);

  Dl_info dli;
  if (dladdr(fn, &dli)) {
    fprintf(f, "\t+0x%zx", fn-dli.dli_saddr);
    if (buf = __cxa_demangle(dli.dli_sname, buf, &len, NULL))
      fprintf(f, "\t%s", buf);
    else
      fprintf(f, "\t%s", dli.dli_sname);
  }

  fprintf(f, "\n");
}

struct sigaction old_actions[_NSIG];
static void do_backtrace(int signum, siginfo_t *info, void *ptr) {
  sigaction(signum, old_actions+signum, NULL);
  fprintf(stderr, "=== SIGNAL %2d ===\n", signum);

  void *array[256];
  size_t size = backtrace(array, sizeof(array)/sizeof(*array));

  struct ucontext *uc = (struct ucontext *)ptr;

  switch (signum) {
  case SIGSEGV:
    fprintf(stderr, "FAULT: %p\n", info->si_addr);
    break;
  default:
    break;
  };

  for (int i = 1 ; i < size ; ++i)
    fn_info(stderr, array[i]);

  fflush(stderr);
  kill(getpid(), signum);
}

void __attribute__ ((constructor)) bt_init(void) {
  buf = NULL;
  len = 0;

  memset(old_actions, 0, sizeof(old_actions));

  struct sigaction new_action;
  memset(&new_action, 0, sizeof(new_action));
  new_action.sa_sigaction = do_backtrace;
  sigemptyset (&new_action.sa_mask);
  new_action.sa_flags = SA_RESTART | SA_SIGINFO;

  int sigs[] = {
    SIGABRT,
    SIGBUS,
    SIGFPE,
    SIGSEGV,
  };

  for (int i = 0 ; i < sizeof(sigs)/sizeof(*sigs) ; ++i)
    sigaction(sigs[i], &new_action, old_actions+sigs[i]);
}

void __attribute__ ((destructor)) bt_fini(void) {
  free(buf);
}

//
