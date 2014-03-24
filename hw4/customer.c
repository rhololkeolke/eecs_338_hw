#include "main.h"

int main(int argc, char** argv)
{
  // should probably add in a safety check
  // on argv size here
  printf("Customer %s started\n", argv[1]);
  fflush(stdout);

  int semid, shmid;
  struct Common* shared;

  semid = semget(SEMKEY, NUM_SEMS, 0777);
  shmid = shmget(SHMKEY, 0, 0);
  shared = (struct Common *)shmat(shmid, 0, 0);

  // enforce the assumption that more customers than can fit on the current
  // plus the next bus is never exceeded
  semwait(semid, SEM_MAX_CUSTOMERS);
  printf("Customer %s has arrived\n", argv[1]);
  fflush(stdout);

  semsignal(semid, SEM_TICKET_QUEUE);
  semwait(semid, SEM_TICKET_READY);

  printf("Cusomter %s getting ticket\n", argv[1]);
  struct Ticket ticket = shared->ticket;
  strcpy(ticket.ticket_holder, argv[1]);
  printf("Customer %s's ticket info:\n", argv[1]);
  printf("\tName: %s\n", ticket.ticket_holder);
  printf("\tTime: %s", ctime(&ticket.dept_time));
  printf("\tSeat: %d\n", ticket.seat_no);
  fflush(stdout);

  // is this ticket for the current or the next bus?
  char curr_bus_ticket = ticket.dept_time == shared->departure_time ? 1 : 0;

  semsignal(semid, SEM_TICKET_RECEIVED);
  if(curr_bus_ticket == 0)
  {
	  printf("Customer %s waiting for next bus\n", argv[1]);
	  semwait(semid, SEM_NEXT_BUS_QUEUE);
  }

  // passengers may block here if they are released
  // from the next bus queue but the next bus
  // hasn't finished pulling up to the gate to allow boarders
  semwait(semid, SEM_BUS_BOARDABLE);
  semsignal(semid, SEM_BUS_BOARDABLE);

  // serializes the boarding of passengers, but doesn't enforce FCFS boarding
  semwait(semid, SEM_CAN_BOARD);
  printf("Customer %s is boarding bus\n", argv[1]);
  fflush(stdout);
  shared->boarded++;
  semsignal(semid, SEM_CAN_BOARD);

  return 0;
}
