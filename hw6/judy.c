#include <stdio.h>
#include <rpc/rpc.h>
#include "Cookie.h"

const int ID = 0;

int main(int argc, char** argv)
{
	printf("Starting judy\n");

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
	int* result = get_my_cookie_1((int*)(&ID), client);
	if(result == (int*)NULL)
	{
		clnt_perror(client, server);
		exit(1);
	}

	printf("Get My Cookie result: %d\n", *result);

	if(*result == -2)
	{
		shutdown_1((int*)(&ID), client);
	}

	clnt_destroy(client);
	
	return 0;
}
