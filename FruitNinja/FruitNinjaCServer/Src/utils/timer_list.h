#ifndef __TIMER_LIST_H__
#define __TIMER_LIST_H__

struct timer_list*
create_timer_list();

void
destory_timer_list(struct timer_list* list);

int
update_timer_list(struct timer_list* list);  //ɨ��list���ٺ�����һ��timer�¼�ִ��

#endif
