#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Cookie.h"
#include <rpc/rpc.h>
#include <sys/time.h>

static int num_cookies = 20;
static int tina_count = 0;
static char shutdown_requests[2] = {0, 0};

static struct timeval tv;

int* get_my_cookie_1(int* which_child, CLIENT* client)
{
	static int result;

	if(num_cookies <= 0)
	{
		gettimeofday(&tv, NULL);
		printf("%d.%d cookies: %d [mother] Sorry kids all out of cookies\n", (int)tv.tv_sec, (int)tv.tv_usec, num_cookies);
		result = -2;
		return &result;
	}
	
	if(*which_child == TINA)
	{
		gettimeofday(&tv, NULL);
		printf("%d.%d cookies: %d [mother] Here you go Tina have a cookie\n", (int)tv.tv_sec, (int)tv.tv_usec, num_cookies-1);
		num_cookies--;
		tina_count++;
		result = 1;
		return &result;
	}
	else
	{
		if(tina_count < 2)
		{
			gettimeofday(&tv, NULL);
			printf("%d.%d cookies: %d [mother] Judy wait your turn. Your sister gets to have a cookie first\n", (int)tv.tv_sec, (int)tv.tv_usec, num_cookies);
			result = -1;
			return &result;
		}
		else
		{
			gettimeofday(&tv, NULL);
			printf("%d.%d cookies: %d [mother] Here you go Judy have a cookie\n", (int)tv.tv_sec, (int)tv.tv_usec, num_cookies-1);
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
	gettimeofday(&tv, NULL);
	printf("%d.%d [mother] Received a shutdown request\n", (int)tv.tv_sec, (int)tv.tv_usec);
	shutdown_requests[*which_child] = 1;
	if(shutdown_requests[0] && shutdown_requests[1])
	{
		gettimeofday(&tv, NULL);
		printf("%d.%d [mother] Mother is shutting down\n", (int)tv.tv_sec, (int)tv.tv_usec);
		exit(0);
	}

	return &shutdown;
}

int* shutdown_1_svc(int* which_child, struct svc_req* req)
{
	CLIENT* client = NULL;
	return (shutdown_1(which_child, client));
}
