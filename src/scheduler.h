/**
 * @file scheduler.h
 * @author Aakash Kumar
 * @brief Rudimentary event scheduler
 * @version 0.1
 * @date 2019-02-02
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef SRC_SCHEDULER_H_
#define SRC_SCHEDULER_H_

#include <stdint.h>
#include <string.h>

#include "log.h"

// For critical section macro
#include "em_core.h"


typedef uint32_t sched_event_t;

#define EVENT(n) ((sched_event_t)0x01<<(n))

#define BITS_IN_BYTE 8
#define TASK_NUMBER \
	(sizeof(sched_event_t) * BITS_IN_BYTE)

typedef enum return_values
{
	Success,
	No_event,
	Scheduler_Uninitialized,
	Event_Unregistered,
	Error
}return_t;


typedef enum eventtype
{
	Task0,	Task1,	Task2,	Task3,	Task4,	Task5,
	Task6,	Task7,	Task8,	Task9,	Task10,	Task11,
	Task12,	Task13,	Task14,	Task15,	Task16,	Task17,
	Task18,	Task19,	Task20,	Task21,	Task22,	Task23,
	Task24,	Task25,	Task26,	Task27,	Task28,	Task29,
	Task30,	Task31
} event_t;

/**
 * @brief Making critical section 
 * markers architecture independent
 * 
 */
#define START_CRITICAL() CORE_CriticalDisableIrq()
#define END_CRITICAL()	 CORE_CriticalEnableIrq()

/**
 * @brief Scheduler states
 * 
 */
enum schdule_states
{
	Uninitialized,
	Initialized,
	Events_Registerd,
	Running
};

/**
 * @brief 
 * 
 */
struct scheduler
{
	volatile sched_event_t pending_events;
	void (*events[TASK_NUMBER]);
	event_t registered_task_num;
	enum schdule_states current_state;
	event_t next_event;
};


#define CHECK_EVENT_INRANGE(event_num) if(((event_num) < TASK_NUMBER) && ((event_num) >= 0))
#define CHECK_EVENT_OUT_OF_RANGE(event_num) if(((event_num) > TASK_NUMBER) || ((event_num) < 0))

int scheduler_init();

void scheduler_run(void);

void schedule_event_ISR(event_t event_num);

void schedule_event(event_t event_num);

int scheduler_event_register(void (*_event)(),event_t event_num);

#endif /* SRC_SCHEDULER_H_ */
