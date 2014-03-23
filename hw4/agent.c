#include "main.h"

int main(int argc, char** argv)
{
  printf("Ticket agent started\n");
  fflush(stdout);

  int semid, shmid;
  struct Common *shared;

  semid = semget(SEMKEY, NUM_SEMS, 0777);
  shmid = shmget(SHMKEY, 0, 0);
  shared = (struct Common *)shmat(shmid, 0, 0);

  // loop until killed
  while(1)
  {
    printf("[agent] waiting for more work\n");
	fflush(stdout);

	semwait(semid, SEM_TICKET_QUEUE);
	semwait(semid, SEM_MUTEX);
	if(shared->tickets_sold >= BUS_CAPACITY)
	{
		printf("[agent] Assigning a ticket to the next bus\n");
		fflush(stdout);
		shared->ticket.dept_time = shared->departure_time + BUS_PERIOD;
		shared->ticket.seat_no = ++shared->next_bus_tickets;
	}
	else
	{
		printf("[agent] Assigning a ticket to the current bus\n");
		fflush(stdout);
		shared->ticket.dept_time = shared->departure_time;
		shared->ticket.seat_no = ++shared->tickets_sold;
	}

	semsignal(semid, SEM_TICKET_READY);
	semwait(semid, SEM_TICKET_RECEIVED);
	semsignal(semid, SEM_MUTEX);
  }

  return 0;
}
