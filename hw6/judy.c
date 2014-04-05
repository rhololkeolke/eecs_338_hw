#include <stdio.h>
#include <rpc/rpc.h>
#include "Cookie.h"

int main(int argc, char** argv)
{
	printf("Starting judy\n");

	char* server;
	int value;
	if(argc != 3)
	{
		fprintf(stderr, "usage: %s <host> <int>\n", argv[0]);
		exit(1);
	}

	server = argv[1];
	value = atoi(argv[2]);

	// create client handle
	CLIENT* client = clnt_create(server, COOKIE_PROGRAM, COOKIE_V1, "udp");
	if(client == (CLIENT *)NULL)
	{
		clnt_pcreateerror(server);
		exit(1);
	}
	int* result = increment_1(&value, client);
	if(result == (int*)NULL)
	{
		clnt_perror(client, server);
		exit(1);
	}

	printf("Increment result: %d\n", *result);

	result = decrement_1(&value, client);
	if(result == (int*)NULL)
	{
		clnt_perror(client, server);
		exit(1);
	}

	printf("Decrement result: %d\n", *result);

	clnt_destroy(client);
	
	return 0;
}
