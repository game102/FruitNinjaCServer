#ifndef __TIMER_H__
#define __TIMER_H__

//在同一线程添加timer并返回唯一timer_id 一次执行
unsigned int
add_once_timer(struct timer_list* list, void(*on_timer)(void* udata),
	void* udata, float after_sec);

//在同一线程添加timer并返回唯一timer_id  多次执行
unsigned int
add_schedule_timer(struct timer_list* list, void(*on_timer)(void* udata),
	void* udata, float after_sec);


void
cancel_timer(struct timer_list* list, unsigned int timer_id);
#endif