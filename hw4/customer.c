#include "main.h"

int main(int argc, char** argv)
{
  // should probably add in a safety check
  // on argv size here
  printf("Customer %s started\n", argv[1]);
  fflush(stdout);

  int semid, shmid;
  struct Common* shared;

  struct Ticket ticket;

  semid = semget(SEMKEY, NUM_SEMS, 0777);
  shmid = shmget(SHMKEY, 0, 0);
  shared = (struct Common *)shmat(shmid, 0, 0);

  printf("Customer %s has arrived\n", argv[1]);

  semwait(semid, SEM_TLINE);
  semwait(semid, SEM_MUTEX);
  strcpy(shared->CName, argv[1]);
  semsignal(semid, SEM_AGENTWRK);
  semwait(semid, SEM_GETTCKT);
  
  // get ticket
  printf("Customer %s getting ticket\n", argv[1]);
  ticket = shared->ticket;
  printf("Customer %s's ticket info:\n", argv[1]);
  printf("\tName: %s\n", ticket.TicketHolder);
  printf("\tTime: TODO\n");
  printf("\tSeat: %d\n", ticket.SeatNo);

  // TODO: add time check
  if(0) {
    shared->NB_WtCnt++;
    semsignal(semid, SEM_MUTEX);
    semwait(semid, SEM_NBUS);
  } else {
    semsignal(semid, SEM_MUTEX);
  }

  printf("Customer %s boarding bus\n", argv[1]);
  return 0;
}
