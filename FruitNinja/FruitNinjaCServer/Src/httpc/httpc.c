#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "httpc.h"

//成功返回0 失败-1 如果没有端口使用默认80

static int
http_parser_url(const char*url, char* host, int* port)
{


}

int http_get(const char * url, const char * params, char ** response, int * len)
{
	return 0;
}

int http_post(const char * url, const char * params, char ** response, int * len)
{
	return 0;
}

void http_free_response(char * response)
{
}
