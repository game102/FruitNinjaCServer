#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "httpc.h"

//�ɹ�����0 ʧ��-1 ���û�ж˿�ʹ��Ĭ��80

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
