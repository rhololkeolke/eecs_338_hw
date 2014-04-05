#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <rpc/rpc.h>
#include "Cookie.h"
#include <unistd.h>

const int ID = 1;

struct timeval tv;

int main(int argc, char** argv)
{
	printf("starting tina\n");

	srand(time(NULL));
	
	char* server;
	if(argc != 2)
	{
		fprintf(stderr, "usage: %s <host>\n", argv[0]);
		exit(1);
	}

	server = argv[1];

	// create client handle
	CLIENT* client = clnt_create(server, COOKIE_PROGRAM, COOKIE_V1, "udp");
	if(client == (CLIENT *)NULL)
	{
		clnt_pcreateerror(server);
		exit(1);
	}
	int* result;
	while(*(result = get_my_cookie_1((int*)(&ID), client)) != -2)
	{
		if(result == (int*)NULL)
		{
			clnt_perror(client, server);
			exit(1);
		}

		gettimeofday(&tv, NULL);
		printf("%d.%d [Tina] Yummy!!!\n", (int)tv.tv_sec, (int)tv.tv_usec);

		sleep(rand() % 10);
		
	}

	shutdown_1((int*)(&ID), client);

	clnt_destroy(client);
	
	return 0;
}
