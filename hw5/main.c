#include "main.h"

sem_t o_sem, h_sem;
pthread_mutex_t mutex, b_mutex;

int h_count = 0, o_count = 0, b_count = 0;

void* oxygenThread(void* input)
{
	pthread_mutex_lock(&mutex);
	o_count++;
	if(h_count >= 2)
	{
		sem_post(&h_sem);
		sem_post(&h_sem);
		h_count -= 2;
		sem_post(&o_sem);
		o_count--;
	}
	else
	{
		pthread_mutex_unlock(&mutex);
	}

	sem_wait(&o_sem);

	pthread_mutex_lock(&b_mutex);
	printf("oxygen is bonding\n");
	b_count++;
	if(b_count == 3)
	{
		b_count = 0;
		pthread_mutex_unlock(&mutex);
	}
	pthread_mutex_unlock(&b_mutex);
	
	return (void*)NULL;
}

void* hydrogenThread(void* input)
{
	pthread_mutex_lock(&mutex);
	h_count++;
	if(h_count >=2 && o_count >= 1)
	{
		sem_post(&h_sem);
		sem_post(&h_sem);
		h_count -= 2;
		sem_post(&o_sem);
		o_count--;
	}
	else
	{
		pthread_mutex_unlock(&mutex);
	}

	sem_wait(&h_sem);

	pthread_mutex_lock(&b_mutex);
	printf("hydrogen is bonding\n");
	b_count++;
	if(b_count == 3)
	{
		b_count = 0;
		pthread_mutex_unlock(&mutex);
	}
	pthread_mutex_unlock(&b_mutex);
	
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
	if(pthread_mutex_init(&b_mutex, NULL) != 0)
	{
		perror("pthread_mutex_init b_mutex failed: ");
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
	
	pthread_t thread1, thread2, thread3;
	int status, *pstatus = &status;

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

	sleep(5);

	if(pthread_create(&thread3, NULL, hydrogenThread, (void*)NULL) != 0)
	{
		perror("pthread_create thread2: ");
		exit(1);
	}

	pthread_join(thread2, (void**)pstatus);
	pthread_join(thread1, (void**)pstatus);
	pthread_join(thread3, (void**)pstatus);

	printf("Both threads have finished\n");
	
	return 0;
}
