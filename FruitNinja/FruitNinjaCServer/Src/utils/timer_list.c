#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "timer.h"
#include "timer_list.h"

#define my_malloc malloc
#define my_free free

// ƽ̨���
#ifdef WIN32
#include <windows.h>
static unsigned int
get_cur_ms() {
	return GetTickCount();
}
#else
static unsigned int
get_cur_ms() {
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);

	return ((tv.tv_usec / 1000) + tv.tv_sec * 1000);
}
#endif


struct timer {
	void(*on_timer)(void* udata);
	void* udata;

	unsigned int duration;   //��δ��� ʱ����
	unsigned int end_time_stamp; //����ʱ�� ����
	int repeat;         //1һ�� -1���޴�
	unsigned int timer_id;
	struct timer* hash_timer_next;  // ָ��ͬhashֵ��һ���ڵ�;
}; 

// hash�����������ǵ�timer, hash value timerid;
#define HASH_COUNT 1024

struct timer_list {  //hash��洢timer
	unsigned int global_timer_id;  //��ǰ����id������������

	struct timer* running_timer;  // ������ ��ֹ���õ�ʱ����Լ�ɾ��
	struct timer* hash_timers[HASH_COUNT];  //1024 hash values
};

struct timer_list*
create_timer_list(){
	struct timer_list* list = my_malloc(sizeof(struct timer_list));
	memset(list, 0, sizeof(struct timer_list));

	return list;
}

unsigned int
add_once_timer(struct timer_list* list, void(*on_timer)(void* udata),
	void* udata, float after_sec)
{
	int timer_id = list->global_timer_id++;
	struct timer* t = my_malloc(sizeof(struct timer));
	memset(t, 0, sizeof(struct timer));
	t->repeat = 1;
	t->duration = (unsigned int)(after_sec * 1000);
	t->end_time_stamp = get_cur_ms() + t->duration; //���ٺ����ִ��offset
	t->timer_id = timer_id;
	t->udata = udata;
	t->on_timer = on_timer;


	// hash���� �¼ӵķ���ͷ
	//int hash_index = timer_id % HASH_COUNT;
	//t->hash_timer_next = list->hash_timers[hash_index];
	//list->hash_timers[hash_index] = t;


	// ��hash���� �¼ӵķ���β
	int hash_index = timer_id % HASH_COUNT;
	struct timer** walk = &list->hash_timers[hash_index];
	while (*walk) {
		walk = &(*walk)->hash_timer_next;
	}
	*walk = t;
	// end 

	return timer_id;
}

unsigned int
add_schedule_timer(struct timer_list* list, void(*on_timer)(void* udata),
	void* udata, float after_sec)
{
	int timer_id = list->global_timer_id++;
	struct timer* t = my_malloc(sizeof(struct timer));
	memset(t, 0, sizeof(struct timer));
	t->repeat = -1;
	t->duration = (unsigned int)(after_sec * 1000);
	t->end_time_stamp = get_cur_ms() + t->duration; //���ٺ����ִ��offset
	t->timer_id = timer_id;
	t->udata = udata;
	t->on_timer = on_timer;


	// hash���� �¼ӵķ���ͷ
	//int hash_index = timer_id % HASH_COUNT;
	//t->hash_timer_next = list->hash_timers[hash_index];
	//list->hash_timers[hash_index] = t;


	// ��hash���� �¼ӵķ���β
	int hash_index = timer_id % HASH_COUNT;
	struct timer** walk = &list->hash_timers[hash_index];
	while (*walk) {
		walk = &(*walk)->hash_timer_next;
	}
	*walk = t;
	// end 

	return timer_id;
}
/*
(1) ֱ��ȡ��: ȡ��timer1--->running_timer = NULL;
(2) �����Ļص����� ȡ��, --->����ȡ���Ѿ���������� if (list->running_timer != NULL && list->running_timer->timer_id == timer_id) {
(3) ����ȡ��  timer2��ȡ��timer1  timer1 running_timer �Ѿ�Ϊ NULL
*/
void
cancel_timer(struct timer_list* list, unsigned int timer_id)
{
	// (2)������ timer1ȡ��timer2��������  timer1ȡ��timer1�쳣
	if (list->running_timer != NULL && list->running_timer->timer_id == timer_id) {
		list->running_timer->repeat = 1;
		return;
	}

	int hash_index = timer_id % HASH_COUNT;
	struct timer** walk = &list->hash_timers[hash_index];

	while (*walk){
		if ((*walk)->timer_id == timer_id){
			struct timer* t = (*walk);
			*walk = (*walk)->hash_timer_next;

			my_free(t);
			return;
		}
		walk = &(*walk)->hash_timer_next;
	}
}

void
destory_timer_list(struct timer_list * list){
	for (int i = 0; i < HASH_COUNT; i++){
		struct timer** walk = &list->hash_timers[i];
		while (*walk){ //�Ƴ�timer
			struct timer* t = (*walk);
			*walk = (*walk)->hash_timer_next;
			my_free(t);
		}
	}
	my_free(list);
}

int
update_timer_list(struct timer_list * list){
	unsigned int todo_min_sec = UINT_MAX;
	struct timer* todo_timer = NULL;

	unsigned int startline_min_sec = get_cur_ms();
	list->running_timer = NULL;                //running_timer ��
	//ִ�е�ʱ���timer
	for (int i = 0; i < HASH_COUNT; i++){
		struct timer** walk = &list->hash_timers[i];
		while (*walk){
			unsigned int cur_time = get_cur_ms();
			if ((*walk)->end_time_stamp <= cur_time){  //�����¼�
				list->running_timer = (*walk);                //running_timer ��
				if( (*walk)->on_timer ) {
					(*walk)->on_timer((*walk)->udata);
				}
				list->running_timer = NULL;                //running_timer ��

				if( (*walk)->repeat > 0 ) {//add_once_timer
					(*walk)->repeat--;
					if ((*walk)->repeat == 0){ //����1�κ��Ƴ�timer
						struct timer* t = (*walk);
						*walk = (*walk)->hash_timer_next;
						my_free(t);
					}
					else { //repeart+1 
						(*walk)->end_time_stamp = get_cur_ms() + (*walk)->duration;
						if ((*walk)->end_time_stamp - startline_min_sec < todo_min_sec){ //����todo_min_sec todo_timer
							todo_timer = *walk;
							todo_min_sec = (*walk)->end_time_stamp - startline_min_sec;
						}
						walk = &((*walk)->hash_timer_next);
					}
				}
				else { //add_schedule_timer
					(*walk)->end_time_stamp = get_cur_ms() + (*walk)->duration;
					if ((*walk)->end_time_stamp - startline_min_sec < todo_min_sec){ //����todo_min_sec todo_timer
						todo_timer = *walk; //----------------------------
						todo_min_sec = (*walk)->end_time_stamp - startline_min_sec;
					}
					walk = &((*walk)->hash_timer_next);
				}
			}
			else {  //�޴����¼�
				//(*walk)->end_time_stamp = get_cur_ms() + (*walk)->duration;

				if (((*walk)->end_time_stamp - startline_min_sec) < todo_min_sec)  //����todo_min_sec todo_timer
				{
					todo_timer = *walk;
					todo_min_sec = (*walk)->end_time_stamp - startline_min_sec;
				}

				walk = &((*walk)->hash_timer_next);
			}
		}
	}

	if (todo_timer != NULL)
	{
		return ( todo_timer->end_time_stamp - get_cur_ms() );  //���ٺ����ִ��
	}

	return -1; //��ʱû���¼�
}
