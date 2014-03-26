#include "main.h"

sem_t o_sem, h_sem, b_sem;
pthread_mutex_t mutex;

int h_count = 0, o_count = 0, b_count = 0;

void* oxygenThread(void* input)
{
	
	return (void*)NULL;
}

void* hydrogenThread(void* input)
{
	
	return (void*)NULL;
}

int main(int argc, char** argv)
{
	printf("Devin Schwab EECS 338 HW 5\n");

	// initialize semaphores and mutexes
	if(pthread_mutex_init(&mutex, NULL) != 0)
	{
		perror("pthread_mutex_init mutex failed: ");
		exit(1);
	}
	if(sem_init(&o_sem, 0, 0) != 0)
	{
		perror("sem_init o_sem failed: ");
		exit(1);
	}
	if(sem_init(&h_sem, 0, 0) != 0)
	{
		perror("sem_init h_sem failed: ");
		exit(1);
	}
	if(sem_init(&b_sem, 0, 1) != 0)
	{
		perror("sem_init b_sem failed: ");
		exit(1);
	}
	
	pthread_t thread1, thread2;
	int status1, status2, *pstatus1, *pstatus2;
	pstatus1 = &status1;
	pstatus2 = &status2;

	printf("Creating 2 threads\n");
	fflush(stdout);
	
	if(pthread_create(&thread1, NULL, oxygenThread, (void*)NULL) != 0)
	{
		perror("pthread_create thread1: ");
		exit(1);
	}
	if(pthread_create(&thread2, NULL, hydrogenThread, (void*)NULL) != 0)
	{
		perror("pthread_create thread2: ");
		exit(1);
	}
	

	pthread_join(thread2, (void**)pstatus1);
	pthread_join(thread1, (void**)pstatus2);

	printf("Both threads have finished\n");
	
	return 0;
}
