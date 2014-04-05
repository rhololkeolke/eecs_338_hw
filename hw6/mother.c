#include <stdio.h>
#include "Cookie.h"
#include <rpc/rpc.h>

int* increment_1(int* value, CLIENT* client)
{
	static int result;
	printf("Calling increment with %d\n", *value);

	result = *value + 1;
	return &result;
}

int* increment_1_svc(int* value, struct svc_req* req)
{
	CLIENT* client = NULL;
	return (increment_1(value, client));
}

int* decrement_1(int* value, CLIENT* client)
{
	static int result;

	printf("Calling decrement with %d\n", *value);

	result = *value - 1;
	return &result;
}

int* decrement_1_svc(int* value, struct svc_req* req)
{
	CLIENT* client = NULL;
	return (decrement_1(value, client));
}
