#ifndef __TIMER_LIST_H__
#define __TIMER_LIST_H__

struct timer_list*
create_timer_list();

void
destory_timer_list(struct timer_list* list);

int
update_timer_list(struct timer_list* list);  //扫描list多少毫秒下一个timer事件执行

#endif
