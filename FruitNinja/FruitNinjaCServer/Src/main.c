#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef WIN32
#include <WinSock2.h>
#include <Windows.h>
#pragma comment(lib, "WSOCK32.LIB")
#endif

#include "utils/timer_list.h"
#include "utils/timer.h"

struct timer_list* t_list = NULL;

static void
on_timer(void* udata) {
	printf("schedule called\n");
	//unsigned int time_id = (unsigned int)(udata);
	//cancel_timer(t_list, time_id);
	//add_once_timer(t_list, on_timer, NULL, 2.0f);
}

static void
on_cancel_timer(void* udata) {
	printf("on_cancel_timer called");
	unsigned int time_id = (unsigned int)(udata);
	cancel_timer(t_list, time_id);
}


int
main(int argc, char** argv) {
#ifdef WIN32
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);
#endif
	// 创建一个socket,用来监听我们的客服端的连接。TCP的socket
	int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == INVALID_SOCKET) {
		goto failed;
	}

	int port = 800;
	printf("starting bind socket at port %d ...\n", port);
	struct sockaddr_in addr;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	int ret = bind(server_socket, (const struct sockaddr*)&addr, sizeof(addr));
	if (ret != 0) {
		printf("bind %s:%d error\n", "127.0.0.1", port);
		goto failed;
	}
	printf("bind %s:%d success\n", "127.0.0.1", port);

	printf("starting listener on %d ...\n", port);

	ret = listen(server_socket, 128);
	if (ret != 0) {
		printf("listening on port failed\n", port);
		goto failed;
	}
	printf("listening on port success\n", port);

	//end 创建一个socket,用来监听我们的客服端的连接。TCP的socket

	t_list = create_timer_list();
	unsigned int timer_id_1 = add_schedule_timer(t_list, on_timer, NULL, 1.0f);
	unsigned int timer_id_2 = add_once_timer(t_list, on_cancel_timer, (void*)timer_id_1, 3.0f);

	fd_set server_fd_set;
	while (1) { // select模型来等待处理socket事件
		FD_ZERO(&server_fd_set);
		FD_SET(server_socket, &server_fd_set);
		int msec = update_timer_list(t_list);

		int ret;
		if (msec < 0) {
			ret = select(0, &server_fd_set, NULL, NULL, NULL);
		}
		else {
			struct timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = msec * 1000;

			ret = select(0, &server_fd_set, NULL, NULL, &timeout);
		}
		if (ret < 0) {
			printf("select error\n");
			// assert(1==0);
		}
		else if (ret == 0) { // 超过时；实现应用程序的定时器
			continue;
		}
		printf("waint for data\n");

	}

	destory_timer_list(t_list);

failed:
	if (server_socket != INVALID_SOCKET) {
		closesocket(server_socket);
	}
#ifdef WIN32
	WSACleanup();
#endif

	return 0;
}

