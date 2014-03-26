#include "main.h"

void* threadFunc1(void* input)
{
	printf("threadFunc1 starting\n");
	fflush(stdout);

	sleep(10);

	printf("threadFunc1 ending\n");
	fflush(stdout);

	return (void*)NULL;
}

void* threadFunc2(void* input)
{
	sleep(2);
	printf("threadFunc2 starting\n");
	fflush(stdout);

	sleep(3);

	printf("threadFunc2 ending\n");
	fflush(stdout);

	return (void*)NULL;
}

int main(int argc, char** argv)
{
	printf("Devin Schwab EECS 338 HW 5\n");

	pthread_t thread1, thread2;
	int status1, status2, *pstatus1, *pstatus2;
	pstatus1 = &status1;
	pstatus2 = &status2;

	printf("Creating 2 threads\n");
	fflush(stdout);
	
	if(pthread_create(&thread1, NULL, threadFunc1, (void*)NULL) != 0)
	{
		perror("pthread_create thread1: ");
		exit(1);
	}
	if(pthread_create(&thread2, NULL, threadFunc2, (void*)NULL) != 0)
	{
		perror("pthread_create thread2: ");
		exit(1);
	}
	

	pthread_join(thread2, (void**)pstatus1);
	pthread_join(thread1, (void**)pstatus2);

	printf("Both threads have finished\n");
	
	return 0;
}
