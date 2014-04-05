/*************************
 *  Devin Schwab (dts34) *
 *                       *
 *  EECS 338 HW 1        *
 *************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

// wait for a child process to terminate
// then print the child process number and exit status
void wait_for_child_to_terminate(int child_process)
{
	int status;
	pid_t pid = wait(&status);

	int exit_status;
	if((exit_status = WEXITSTATUS(status)) != 0)
	{
		printf("Child process %d with pid %d terminated UNSUCCESSFULLY with exit code %d\n", child_process, pid, exit_status);
	}
	else
	{
		printf("Child process %d with pid %d terminated successfully :)\n", child_process, pid);
	}

}

int main()
{
	int pid; // pid of the current process
	int child_process = -1; // index of child process

	int A = 100; // homework variable
	
	for(int i=0; i<3; i++)
	{
		pid = fork();
		if(pid >= 0) // fork succeeded
		{
			// children shouldn't fork
			if(pid == 0)
			{
				child_process = i + 1;
				break;
			}
		}
		else
		{
			// in case of error print to the correct place
			fprintf(stderr, "Failed to fork process %d\n", i);
			exit(1);
		}
	}

	if(child_process == 1)
	{
		sleep(1);
		printf("\nRunning first child process\n");

		A += 100;
		printf("[%d] A: %d\n", child_process, A);
		fflush(stdout);

		char* username = getlogin();
		if(username == NULL)
			perror("Error getting username in process 1");
		else
			printf("[%d] username: %s\n", child_process, username);
		printf("[%d] parent process: %d\n", child_process, getppid());

		char hostname[100]; // buffer for hostname string
		if(gethostname(hostname, 100) < 0)
			perror("Error getting hostname in process 1");
		else
			printf("[%d] hostname: %s\n", child_process, hostname);
	} 
	else if(child_process == 2)
	{	
		sleep(5); // wait 5 seconds for the first process to run
		printf("\nRunning second child process\n");

		A += 100;
		printf("[%d] A: %d\n", child_process, A);
		fflush(stdout);

		struct timeval time;
		if(gettimeofday(&time, NULL) < 0)
			perror("Error getting time of day in process 2");
		else
			printf("[%d] time: %d s %d us\n", child_process, (int)time.tv_sec, (int)time.tv_usec);
		char* time_str = ctime(&time.tv_sec);
		printf("[%d] time: %s", child_process, time_str);
		printf("[%d] uid: %d\n", child_process, (int)getuid());
	}
	else if(child_process == 3)
	{
		sleep(10); // wait 10 seconds for the first and second processes to run
		printf("\nRunning 3rd child process\n");
		
		A += 100;
		printf("[%d] A: %d\n", child_process, A);
		fflush(stdout);

		printf("[%d] gid: %d\n", child_process, (int)getgid());
		struct rusage usage;
		if(getrusage(RUSAGE_SELF, &usage))
			perror("Error getting usage in process 3");
		else
		{
			printf("[%d] user cpu time: %ds %us\n", child_process, (int)usage.ru_utime.tv_sec, (int)usage.ru_utime.tv_usec);
			printf("[%d] system cpu time: %ds %us\n", child_process, (int)usage.ru_stime.tv_sec, (int)usage.ru_stime.tv_usec);
			printf("[%d] maximum resident set size: %ld\n", child_process, usage.ru_maxrss);
			printf("[%d] integral shared memory size: %ld\n", child_process, usage.ru_ixrss);
			printf("[%d] integral unshared data size: %ld\n", child_process, usage.ru_idrss);
			printf("[%d] integral unshared stack size: %ld\n", child_process, usage.ru_isrss);
			printf("[%d] page reclaims (soft page faults): %ld\n", child_process, usage.ru_minflt);
			printf("[%d] page faults (hard page faults): %ld\n", child_process, usage.ru_majflt);
			printf("[%d] swaps: %ld\n", child_process, usage.ru_nswap);
			printf("[%d] block input operations: %ld\n", child_process, usage.ru_inblock);
			printf("[%d] block output operations: %ld\n", child_process, usage.ru_oublock);
			printf("[%d] IPC messages sent: %ld\n", child_process, usage.ru_msgsnd);
			printf("[%d] IPC messages received: %ld\n", child_process, usage.ru_msgrcv);
			printf("[%d] signals received: %ld\n", child_process, usage.ru_nsignals);
			printf("[%d] voluntary context switches: %ld\n", child_process, usage.ru_nvcsw);
			printf("[%d] involuntary context switches: %ld\n", child_process, usage.ru_nivcsw);
		}
	}
	else
	{
		wait_for_child_to_terminate(1);
		wait_for_child_to_terminate(2);
		wait_for_child_to_terminate(3);

		/*
		  when fork is called the main process is copied
		  into a child process. That means directly after fork
		  finishes the main process and the child process have copies
		  of the same data. When the child process modifies A it modifies
		  that child's copy, not the main thread's copy. So A doesn't change value in the main thread.
		*/
		printf("\n");
		printf("[main] A: %d\n", A);

		if(execlp("last", "", (char *)NULL) < 0)
		{
			perror("Error running exec(last) in main");
			exit(1);
		}
	}

	return 0;
}
