#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <getopt.h>

sem_t o_sem, h_sem;
pthread_mutex_t mutex, b_mutex;

int h_count = 0, o_count = 0, b_count = 0;

void print_usage();

void* oxygenThread(void* input)
{
	pthread_mutex_lock(&mutex);
	o_count++;
	if(h_count >= 2)
	{
		printf("starting a molecule\n");
		fflush(stdout);
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
	printf("\toxygen[%d] is bonding\n", *((int*)input));
	fflush(stdout);
	b_count++;
	if(b_count == 3)
	{
		printf("finished a molecule\n");
		fflush(stdout);
		b_count = 0;
		pthread_mutex_unlock(&mutex);
	}
	pthread_mutex_unlock(&b_mutex);

	free((int*)input);
	return (void*)NULL;
}

void* hydrogenThread(void* input)
{
	pthread_mutex_lock(&mutex);
	h_count++;
	if(h_count >=2 && o_count >= 1)
	{
		printf("starting a molecule\n");
		fflush(stdout);
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
	printf("\thydrogen[%d] is bonding\n", *((int*)input));
	fflush(stdout);
	b_count++;
	if(b_count == 3)
	{
		printf("finished a molecule\n");
		fflush(stdout);
		b_count = 0;
		pthread_mutex_unlock(&mutex);
	}
	pthread_mutex_unlock(&b_mutex);
	
	free((int*)input);
	return (void*)NULL;
}

int main(int argc, char** argv)
{
	static struct option long_options[] = {
		{"molecules", required_argument, 0, 'm'}, // total number of molecules for the run
		{"hydrogen", required_argument, 0, 'h'}, // total number of hydrogen to spawn at a time
		{"oxygen", required_argument, 0, 'o'}, // total number of oxygen to spawn at a time
		{"seed", required_argument, 0, 's'} // ssed of rand (defaults to current time)
	};

	int num_molecules = 20;
	int num_hydrogen = 5;
	int num_oxygen = 3;

	srand(time(NULL));

	printf("\n");
	printf("EECS 338 HW5\n");
	printf("Devin Schwab (dts34)\n");
	printf("----------------------\n");
	fflush(stdout);

	// parse arguments
	int long_index = 0;
	char opt;
	while((opt = getopt_long(argc, argv, "m:h:o:s:", long_options, &long_index)) != -1) {
		switch(opt) {
		case 'm':
			num_molecules = atoi(optarg);
			printf("Number of Molecules: %d\n", num_molecules);
			break;
		case 'h':
			num_hydrogen = atoi(optarg);
			printf("Number of Hydrogen in group: %d\n", num_hydrogen);
			break;
		case 'o':
			num_oxygen = atoi(optarg);
			printf("Number of Oxygen in group: %d\n", num_oxygen);
			break;
		case 's':
			srand(atoi(optarg));
			printf("seed: %d\n", atoi(optarg));
			break;
		default:
			print_usage();
			exit(1);
		}
	}
	
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

	pthread_t* hydrogen_threads = malloc(num_molecules*2*sizeof(pthread_t));
	pthread_t* oxygen_threads = malloc(num_molecules*sizeof(pthread_t));
	
	int status, *pstatus = &status;


	int hydrogen_spawned = 0;
	int oxygen_spawned = 0;
	while(hydrogen_spawned + oxygen_spawned < 3*num_molecules)
	{
		sleep(rand() % 6); // sleep for a random amount of time

		int i;
		for(i=0; i<num_hydrogen && hydrogen_spawned < 2*num_molecules; i++)
		{
			int* arg = malloc(sizeof(int));
			*arg = hydrogen_spawned;
			if(pthread_create(&hydrogen_threads[hydrogen_spawned], NULL, hydrogenThread, (void*)arg) != 0)
			{
				perror("pthread_create hydrogen thread: ");
				exit(1);
			}
			hydrogen_spawned++;
		}

		for(i=0; i<num_oxygen && oxygen_spawned < num_molecules; i++)
		{
			int* arg = malloc(sizeof(int));
			*arg = oxygen_spawned;
			if(pthread_create(&oxygen_threads[oxygen_spawned], NULL, oxygenThread, (void*)arg) != 0)
			{
				perror("pthread_create oxygen thread: ");
				exit(1);
			}
			oxygen_spawned++;
		}
	}

	int i;
	for(i=0; i<hydrogen_spawned; i++)
	{
		pthread_join(hydrogen_threads[i], (void**)pstatus);
	}
	for(i=0; i<oxygen_spawned; i++)
	{
		pthread_join(oxygen_threads[i], (void**)pstatus);
	}

	printf("All threads have finished\n");

	free(hydrogen_threads);
	free(oxygen_threads);
	
	return 0;
}

void print_usage()
{
	printf("\n");
	printf("--molecules (-m): Number of molecules to make this run (defaults to 20)\n");
	printf("--hydrogen (-h): Number of hydrogen threads to spawn at a time (defaults to 5)\n");
	printf("--oxygen (-o): Number of oxygen threads to spawn at a time (defaults to 3)\n");
	printf("--seed (-s): Seed for random number generator (defaults to current time)\n");
	printf("\n");
}
