#ifndef __TIMER_H__
#define __TIMER_H__

//��ͬһ�߳����timer������Ψһtimer_id һ��ִ��
unsigned int
add_once_timer(struct timer_list* list, void(*on_timer)(void* udata),
	void* udata, float after_sec);

//��ͬһ�߳����timer������Ψһtimer_id  ���ִ��
unsigned int
add_schedule_timer(struct timer_list* list, void(*on_timer)(void* udata),
	void* udata, float after_sec);


void
cancel_timer(struct timer_list* list, unsigned int timer_id);
#endif