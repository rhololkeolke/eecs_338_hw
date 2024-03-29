#include "main.h"
#include <getopt.h>

void print_usage()
{
  printf("\n");
  printf("--totc (-C): Total number of C processes for the run (defaults to 18)\n");
  printf("--totb (-B): Total number of B processes for the run (defaults to 3)\n");
  printf("--numc (-c): Number of C processes to spawn each spawn time (defaults to 6)\n");
  printf("--numb (-b): Number of B processes to spawn each spawn time (defaults to 1)\n");
  printf("--seed (-s): Seed for random number generator (defaults to current time)\n");
  printf("\n");
}

int main(int argc, char** argv)
{
  static struct option long_options[] = {
    {"totc", required_argument, 0, 'C'}, // total number of C processes for the run
    {"totb", required_argument, 0, 'B'}, // total number of B processes for the run
    {"numc", required_argument, 0, 'c'}, // number of C processes to spawn each time
    {"numb", required_argument, 0, 'b'}, // number of B processes to spawn each time
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
  while((opt = getopt_long(argc, argv, "C:B:c:b:s:h", long_options, &long_index)) != -1) {
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
	case 'h':
    default:
      print_usage();
      exit(1);
    }
  }

  if(totc > totb*BUS_CAPACITY)
  {
	  fprintf(stderr, "Error: Number of customers specified is greater than can fit on buses (%d > %d)\n",
			  totc, totb*BUS_CAPACITY);
	  exit(1);
  }

  int semid, shmid;
  unsigned short seminit[NUM_SEMS];
  struct Common* shared;
  union semun semctlarg;

  // initialize the semaphores
  semid = semget(getSemKey(), NUM_SEMS, 0777 | IPC_CREAT);
  seminit[SEM_TICKET_QUEUE] = 0;
  seminit[SEM_TICKET_READY] = 0;
  seminit[SEM_TICKET_RECEIVED] = 0;
  seminit[SEM_MUTEX] = 1;
  seminit[SEM_NEXT_BUS_QUEUE] = 0;
  seminit[SEM_BUS_BOARDABLE] = 0;
  seminit[SEM_GATE_EMPTY] = 1;
  seminit[SEM_CAN_BOARD] = 1;
  seminit[SEM_MAX_CUSTOMERS] = 14;
  semctlarg.array = seminit;
  semctl(semid, NUM_SEMS, SETALL, semctlarg);

  // initiailize the shared memory
  shmid = shmget(getSemKey(), sizeof(struct Common), 0777 | IPC_CREAT);
  shared=(struct Common *)shmat(shmid, 0, 0);
  shared->departure_time = time(NULL) + BUS_PERIOD;
  shared->tickets_sold = 0;
  shared->next_bus_tickets = 0;
  shared->ticket.dept_time = shared->departure_time;
  shared->ticket.seat_no = 0;
  shared->boarded = 0;

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

  // loop until all of the customer and bus processes have spawned
  while(customerCount < totc || busCount < totb) {
    int sleep_time = rand() % 6; // sleep at most 60 seconds before spawning more processes
    printf("sleeping for %d\n", sleep_time);
    fflush(stdout);
    sleep(sleep_time);

    if(busCount < totb) { // more to spawn
		int i;
		for(i=0; i<numb && busCount < totb; i++) {
			busCount++;
			pid_t b_pid;
			if((int)(b_pid = fork()) == 0) {
				sprintf(countStr, "%d", busCount);
				execl("bus.bin", "bus", countStr, NULL);
				exit(0);
			} else if(b_pid < 0) {
				perror("bus fork()");
				exit(1); // should probably cleanup before exiting
			}
		}
    }

    if(customerCount < totc) { // more to spawn
      int i;
      for(i=0; i<numc && customerCount < totc; i++) {
		  customerCount++;
		  pid_t c_pid;
		  if((int)(c_pid = fork()) == 0) {
			  sprintf(countStr, "%d", customerCount);
			  execl("customer.bin", "customer", countStr, NULL);
			  exit(0);
		  } else if(c_pid < 0) {
			  perror("customer fork()");
			  exit(1); // should probably cleanup before exiting
		  }
      }
    }
  }

  int i;
  for(i=0; i < totc + totb; i++) {
    printf("Calling wait %d\n", i);
    wait(NULL);
  }

  printf("Killing ticket agent\n");
  kill(agent_pid, SIGKILL);
  wait(NULL);

  printf("Cleaning up semaphores and shared memory\n");
  semctl(semid, NUM_SEMS, IPC_RMID, 0);
  shmctl(shmid, IPC_RMID, 0);

  return 0;
}
