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

#define NUM_SEMS 9
#define SEM_TICKET_QUEUE 0
#define SEM_TICKET_READY 1
#define SEM_TICKET_RECEIVED 2
#define SEM_MUTEX 3
#define SEM_NEXT_BUS_QUEUE 4
#define SEM_BUS_BOARDABLE 5
#define SEM_GATE_EMPTY 6
#define SEM_CAN_BOARD 7
#define SEM_MAX_CUSTOMERS 8

#define SEMKEY 77
#define SHMKEY 77

#define BUS_CAPACITY 7
#define BUS_PERIOD 30 // seconds

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
  char ticket_holder[100];
  time_t dept_time;
  int seat_no;
};

struct Common {
	time_t departure_time;
	int tickets_sold;
	int next_bus_tickets;
	int boarded;
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
