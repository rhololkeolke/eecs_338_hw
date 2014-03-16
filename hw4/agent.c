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
    printf("waiting for more work\n");
    semwait(semid, SEM_AGENTWRK);
    if(shared->CB_Avail_SCnt != 0) {
      printf("Assigning a ticket to the current bus\n");
      strcpy(shared->ticket.TicketHolder, shared->CName);
      shared->ticket.Dept_Time = shared->CB_DeptTime;
      shared->ticket.SeatNo = BUS_CAPACITY - shared->CB_Avail_SCnt;
      shared->CB_Avail_SCnt--;
    } else {
      printf("Assigning a ticket to the next bus\n");
      strcpy(shared->ticket.TicketHolder, shared->CName);
      shared->ticket.Dept_Time = shared->NB_DeptTime;
      shared->ticket.SeatNo = BUS_CAPACITY - shared->NB_Avail_SCnt;
      shared->NB_Avail_SCnt--;
    }
    semsignal(semid, SEM_GETTCKT);
    semsignal(semid, SEM_TLINE);
  }

  return 0;
}
