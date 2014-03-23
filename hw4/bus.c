#include "main.h"

int main(int argc, char** argv)
{
  printf("bus %s started\n", argv[1]);
  fflush(stdout);

  int semid, shmid;
  struct Common* shared;

  semid = semget(SEMKEY, NUM_SEMS, 0777);
  shmid = shmget(SHMKEY, 0, 0);
  shared = (struct Common *)shmat(shmid, 0, 0);

  semwait(semid, SEM_GATE_EMPTY);
  printf("Bus %s has arrived\n", argv[1]);
  fflush(stdout);

  semsignal(semid, SEM_BUS_BOARDABLE);

  semwait(semid, SEM_MUTEX);
  time_t sleep_time = shared->departure_time - time(NULL);
  semsignal(semid, SEM_MUTEX);

  printf("Bus %s Sleeping until departure\n", argv[1]);
  fflush(stdout);
  sleep(sleep_time);

  semwait(semid, SEM_MUTEX);
  printf("Bus %s: last call for boarding\n", argv[1]);
  fflush(stdout);
  while(1)
  {
	  semwait(semid, SEM_CAN_BOARD);
	  if(shared->tickets_sold == shared->boarded)
	  {
		  semsignal(semid, SEM_CAN_BOARD);
		  break;
	  }
	  semsignal(semid, SEM_CAN_BOARD);
	  // wait another second to give customer
	  // threads a chance to finish boarding
	  sleep(1);
  }

  semwait(semid, SEM_BUS_BOARDABLE);

  // release passengers to the gate that were waiting for the next bus
  printf("Bus %s releasing next bus queue\n", argv[1]);
  fflush(stdout);
  int i;
  for(i=0; i<shared->next_bus_tickets; i++)
  {
	  semsignal(semid, SEM_NEXT_BUS_QUEUE);
  }

  // reset the ticket variables for the next bus
  printf("Bus %s preparing shared variables for next bus\n", argv[1]);
  fflush(stdout);
  shared->tickets_sold = shared->next_bus_tickets;
  shared->next_bus_tickets = 0;
  shared->departure_time += BUS_PERIOD;
  shared->boarded = 0;
  semsignal(semid, SEM_MUTEX);

  semsignal(semid, SEM_GATE_EMPTY);

  printf("Bus %s is departing\n", argv[1]);
  fflush(stdout);
  return 0;
}
