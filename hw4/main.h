#ifndef BUS_PROBLEM_H_
#define BUS_PROBLEM_H_

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NUM_SEMS 5
#define SEM_MUTEX 0
#define SEM_TLINE 1
#define SEM_AGENTWRK 2
#define SEM_GETTCKT 3
#define SEM_NBUS 4

#define SEMKEY 77
#define SHMKEY 77

void semwait(int semid, int semaphore)
{
  struct sembuf psembuf;
  
  psembuf.sem_op = -1;
  psembuf.sem_flg = 0;
  psembuf.sem_num = semaphore;
  semop(semid, &psembuf, 1);
  return;
}

void semsignal(int semid, int semaphore)
{
  struct sembuf vsembuf;
  
  vsembuf.sem_op = 1;
  vsembuf.sem_flg = 0;
  vsembuf.sem_num = semaphore;
  semop(semid, &vsembuf, 1);
}

struct Ticket {
  char TicketHolder[100];
  // TODO: Time
  int SeatNo;
};

struct Common {
  int NB_WtCnt;
  int CB_Avail_SCnt;
  int NB_Avail_SCnt;
  char CName[100];
  // TODO: Time
  struct Ticket ticket;
};

union semun {
  /* value for SETVAL */
  int val;
  /* buffer for IPC_STAT, IPC_SET */
  struct semid_ds *buf;
  /* array for GETALL, SETALL */
  unsigned short *array;
  /* Linux specific part: */
  /* buffer for IPC_INFO */
  struct seminfo *__buf;
};

#endif
