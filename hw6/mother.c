#include <stdio.h>
#include "Cookie.h"
#include <rpc/rpc.h>

int* increment_1(int* value, CLIENT* client)
{
	static int result = 10;

	printf("Calling increment with %d\n", *value);

	return &result;
}

int* increment_1_svc(int* value, struct svc_req* req)
{
	CLIENT* client;
	return (increment_1(value, client));
}

int* decrement_1(int* value, CLIENT* client)
{
	static int result = 9;

	printf("Calling decrement with %d\n", *value);

	return &result;
}

int* decrement_1_svc(int* value, struct svc_req* req)
{
	CLIENT* client;
	return (decrement_1(value, client));
}
