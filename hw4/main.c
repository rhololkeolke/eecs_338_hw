#include "main.h"
#include <getopt.h>

void print_usage()
{
  printf("\n");
  printf("--totc (-C): Total number of C threads for the run (defaults to 18)\n");
  printf("--totb (-B): Total number of B threads for the run (defaults to 3)\n");
  printf("--numc (-c): Number of C threads to spawn each spawn time (defaults to 6)\n");
  printf("--numb (-b): Number of B theads to spawn each spawn time (defaults to 1)\n");
  printf("--seed (-s): Seed for random number generator (defaults to current time)\n");
  printf("\n");
}

int main(int argc, char** argv)
{
  static struct option long_options[] = {
    {"totc", required_argument, 0, 'C'}, // total number of C threads for the run
    {"totb", required_argument, 0, 'B'}, // total number of B threads for the run
    {"numc", required_argument, 0, 'c'}, // number of C threads to spawn each time
    {"numb", required_argument, 0, 'b'}, // number of B threads to spawn each time
    {"seed", required_argument, 0, 's'} // seed of rand (defaults to current time value) 
  };

  int totc = 18;
  int totb = 3;
  int numc = 6;
  int numb = 1;

  srand(time(NULL));

  printf("\n");
  printf("EECS 338 HW4\n");
  printf("Devin Schwab (dts34)\n");
  printf("------------------------\n");
  fflush(stdout);

  int long_index = 0;
  char opt;
  while((opt = getopt_long(argc, argv, "C:B:c:b:s:", long_options, &long_index)) != -1) {
    switch(opt) {
    case 'c':
      numc = atoi(optarg);
      printf("c: %d\n", numc);
      break;
    case 'b':
      numb = atoi(optarg);
      printf("b: %d\n", numb);
      break;
    case 'C':
      totc = atoi(optarg);
      printf("C: %d\n", totc);
      break;
    case 'B':
      totb = atoi(optarg);
      printf("B: %d\n", totb);
      break;
    case 's':
      srand(atoi(optarg));
      printf("s: %d\n", atoi(optarg));
      break;
    default:
      print_usage();
      exit(1);
    }
  }

  pid_t agent_pid;
  int customerCount = 0;
  int busCount = 0;

  char countStr[100];

  // spawn the ticket agent
  agent_pid = fork();
  if(agent_pid == 0) {
    // child
    execl("agent.bin", "agent", NULL);
    exit(0);
  } else if(agent_pid < 0) {
    perror("agent fork()");
    exit(1);
  }

  if((int)fork() == 0) {
    sprintf(countStr, "%d", busCount);
    execl("bus.bin", "bus", countStr, NULL);
    exit(0);
  }
  else if((int)fork() == 0) {
    sprintf(countStr, "%d", customerCount);
    execl("customer.bin", "customer", countStr, NULL);
    exit(0);
  }

  wait(NULL);
  wait(NULL);

  sleep(5);

  kill(agent_pid, SIGKILL);
  wait(NULL);

  return 0;
}
