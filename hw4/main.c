#include "main.h"
#include <getopt.h>

void print_usage()
{
  printf("\n");
  printf("--totc (-C): Total number of C threads for the run (defaults to 18)\n");
  printf("--totb (-B): Total number of B threads for the run (defaults to 3)\n");
  printf("--numc (-c): Number of C threads to spawn each spawn time (defaults to 5)\n");
  printf("--numb (-b): Number of B theads to spawn each spawn time (defaults to 1)\n");
  printf("\n");
}

int main(int argc, char** argv)
{
  static struct option long_options[] = {
    {"totc", required_argument, 0, 'C'}, // total number of C threads for the run
    {"totb", required_argument, 0, 'B'}, // total number of B threads for the run
    {"numc", required_argument, 0, 'c'}, // number of C threads to spawn each time
    {"numb", required_argument, 0, 'b'}  // number of B threads to spawn each time
  };

  printf("\n");
  printf("EECS 338 HW4\n");
  printf("Devin Schwab (dts34)\n");
  printf("------------------------\n");
  fflush(stdout);

  int long_index = 0;
  char opt;
  while((opt = getopt_long(argc, argv, "C:B:c:b:", long_options, &long_index)) != -1) {
    switch(opt) {
    case 'c':
      printf("c: %d\n", atoi(optarg));
      break;
    case 'b':
      printf("b: %d\n", atoi(optarg));
      break;
    case 'C':
      printf("C: %d\n", atoi(optarg));
      break;
    case 'B':
      printf("B: %d\n", atoi(optarg));
      break;
    default:
      print_usage();
      exit(1);
    }
  }

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
