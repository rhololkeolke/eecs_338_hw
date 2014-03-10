#ifndef BUS_PROBLEM_H_
#define BUS_PROBLEM_H_

#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

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

#endif
