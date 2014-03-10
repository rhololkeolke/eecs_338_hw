#include "main.h"

int main(int argc, char** argv)
{
  printf("Main process started\n");
  fflush(stdout);

  pid_t agent_pid, bus_pid, customer_pid;

  if((int)(agent_pid = fork()) == 0) {
    execl("agent.bin", "agent", NULL);
    exit(0);
  }
  else if((int)(bus_pid = fork()) == 0) {
    execl("bus.bin", "bus", "0", NULL);
    exit(0);
  }
  else if((int)(customer_pid = fork()) == 0) {
    execl("customer.bin", "customer", "0", NULL);
    exit(0);
  }

  wait(NULL);
  wait(NULL);
  wait(NULL);

  return 0;
}
