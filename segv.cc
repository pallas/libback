#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

extern
int this_is_a_function(int n) {
  if (n)
    this_is_a_function(n-1);
  else
    *((int*)0x1234) = 0x1234;
  return 0;
}

int
main(int argc, char *argv[]) {
  return this_is_a_function(1);
}
