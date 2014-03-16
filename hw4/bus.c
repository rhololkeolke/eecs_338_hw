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

  semwait(semid, SEM_GATEEMPTY);
  printf("Bus %s has arrived\n", argv[1]);
  fflush(stdout);

  semwait(semid, SEM_MUTEX);
  printf("Bus %s is setting departure times\n", argv[1]);
  fflush(stdout);
  shared->CB_DeptTime = shared->NB_DeptTime;
  shared->NB_DeptTime += BUS_PERIOD;
  shared->CB_Avail_SCnt = BUS_CAPACITY - shared->NB_WtCnt;
  while(shared->NB_WtCnt != 0) {
    semsignal(semid, SEM_NBUS);
    shared->NB_WtCnt--;
  }

  // get the sleep time
  time_t sleep_time = shared->CB_DeptTime - time(NULL);

  semsignal(semid, SEM_MUTEX);

  printf("Bus %s Sleeping until departure\n", argv[1]);
  fflush(stdout);
  sleep(sleep_time);

  printf("Bus %s is departing\n", argv[1]);
  fflush(stdout);
  semsignal(semid, SEM_GATEEMPTY);
  return 0;
}
