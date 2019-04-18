#ifndef __HTTP_C_H__
#define __HTTP_C_H__


//返回值 是否成功,错误码
int http_get(const char* url, const char* params, char** response, int* len);

int http_post(const char* url, const char* params, char** response, int* len);

void http_free_response(char* response);
#endif