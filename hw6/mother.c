#include <stdio.h>
#include "Cookie.h"
#include <rpc/rpc.h>

static int num_cookies = 20;
static int tina_count = 0;
static char shutdown_requests[2] = {0, 0};

int* get_my_cookie_1(int* which_child, CLIENT* client)
{
	static int result;

	if(num_cookies <= 0)
	{
		result = -2;
		return &result;
	}
	
	if(*which_child == TINA)
	{
		num_cookies--;
		tina_count++;
		result = 1;
		return &result;
	}
	else
	{
		if(tina_count < 2)
		{
			result = -1;
			return &result;
		}
		else
		{
			num_cookies--;
			tina_count = 0;
			result = 1;
			return &result;
		}
	}
}

int* get_my_cookie_1_svc(int* which_child, struct svc_req* req)
{
	CLIENT* client = NULL;
	return (get_my_cookie_1(which_child, client));
}

int* shutdown_1(int* which_child, CLIENT* client)
{
	static shutdown = 0;
	printf("Received a shutdown request\n");
	shutdown_requests[*which_child] = 1;
	if(shutdown_requests[0] && shutdown_requests[1])
	{
		printf("Mother is shutting down\n");
		exit(0);
	}

	return &shutdown;
}

int* shutdown_1_svc(int* which_child, struct svc_req* req)
{
	CLIENT* client = NULL;
	return (shutdown_1(which_child, client));
}
