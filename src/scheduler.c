/**
 * @file scheduler.c
 * @author Aakash Kumar
 * @brief Very minimal scheduler implementation, events are 
 * scheduled to run by setting a bit in a 32bit number
 * the lowest bit is checked to run first which makes it
 * the highest priority event. Hence the lower the event 
 * bit position the higher the priority.
 * 
 * **** USAGE ****
 * The scheduler is initialized by calling scheduler_init()
 * A function pointer can be passed with the event number
 * to scheduler_event_register() to register the event to run
 * The event is triggered from an ISR when schedule_event_ISR()
 * is called with the event number
 * Calling scheduler run within an infinite loop will execute the 
 * pending events in order of priority.
 * This is done to prevent arch specific 
 * sleep functions to be in main or macroed in another library.
 * 
 * @version 0.1
 * @date 2019-02-02
 * 
 * @copyright Copyright (c) 2019
 * 
 */


/*
 * TODO: Possible improvement,
 *
 * -- Change all returns to enums
 *
 * -- Instead of get_next_event, add tasks to a queue
 * and pop from queue to execute. That should enable
 * handling of nested interrupts
 *
 * -- Add possibility to pass a void * pointer (pthread style)
 * to the event function, then the event function can
 * act on parameters passed to it.
 */
#include "scheduler.h"

/**
 * @brief Scheduler book keeping structure,
 */
static struct scheduler schedule_keeper;


/**
 *
 * @return 0 for success, -1 for failure
 */
int scheduler_init()
{
	if(schedule_keeper.current_state != Uninitialized)
	{
		LOG_ERROR("Scheduler already initialized");
		return -1;
	}
	else
	{
		START_CRITICAL();

		schedule_keeper.pending_events = 0;
		schedule_keeper.registered_task_num = 0; // Unused for now
		memset(schedule_keeper.events,0,sizeof(void *));
		schedule_keeper.next_event = TASK_NUMBER;
		schedule_keeper.current_state = Initialized;

		END_CRITICAL();
		LOG_DEBUG("Scheduler Initialized");
	}
	return 0;
}

/**
 * @brief called from ISR, needs an event specific wrapper
 * with the event_num to be triggered from that particular ISR
 * 
 * @param event_num 
 */
void schedule_event_ISR(event_t event_num)
{
	/*
	 * Check for the event number being in range
	 * Does not require critical section here
	 * unless there is a need to handle
	 * nested interrupts
	 */
	CHECK_EVENT_INRANGE(event_num)
	{
		schedule_keeper.pending_events |= EVENT(event_num);
	}
	else
		//TODO: Handle this better, printf from ISR now -- bad
		LOG_ERROR("event num out of range");
}


void schedule_event(event_t event_num)
{
	/*
	 * Check for the event number being in range
	 */
	CHECK_EVENT_INRANGE(event_num)
	{
		START_CRITICAL();
		schedule_keeper.pending_events |= EVENT(event_num);
		END_CRITICAL();
	}
	else
	{
		LOG_ERROR("event num out of range");
	}
}


/*
 * Events start from 0
 * parameter event_num takes 0 to 32
 */
int scheduler_event_register(void (*_event)(),
		event_t event_num)
{
	if(schedule_keeper.current_state == Uninitialized)
	{
		/*
		 * Error if scheduler is uninitialized
		 * before registering events
		 */
		LOG_ERROR("Scheduler uninitialized");
		return -1;
	}
	else CHECK_EVENT_OUT_OF_RANGE(event_num)
	{
		/*
		 * Error if event number exceeds the number of
		 * events allowed or negative event number passed
		 */
		LOG_ERROR("event number outside limits");
		return -1;
	}
	else
	{
		START_CRITICAL();
		schedule_keeper.events[event_num] = _event;
		schedule_keeper.current_state = Events_Registerd;
		END_CRITICAL();
		LOG_DEBUG("Event number %d registered",event_num);
	}

	return 0;
}


/**
 * @brief Get the next event object
 * gets the highest priority in the next_event
 *  event to be executed and clears out the 
 * pending flag for that event 
 
 * 
 * @return int 
 * returns 0 when there is an event to execute
 * if function returns -1, then nothing
 * to execute and next_event will
 * have max number of tasks possible
 */
int get_next_event()
{
	/*
	 * Counts trailing zeroes, this will get the lowest
	 * set bit and that will be the task number to be executed
	 */
	START_CRITICAL();
	schedule_keeper.next_event = 
				__builtin_ctz(schedule_keeper.pending_events);
	END_CRITICAL();

	if(schedule_keeper.next_event < TASK_NUMBER )
	{
		/*
		 * Zeroing out the lowest set bit,
		 * this will be the highest priority
		 * task to be executed, clearing the bit
		 * once the task is queued for execution
		 */
		START_CRITICAL();
		schedule_keeper.pending_events &=
				(schedule_keeper.pending_events-1);
		END_CRITICAL();
		return 0;
	}

	return -1;
}

/*
 * Executes a passed function pointer
 */
void run_event(void (*_event)())
{
	_event();
}

/**
 * @brief Executes next pending event
 * 
 */
void execute_next_event(void)
{
	if(schedule_keeper.events[schedule_keeper.next_event] != NULL)
		run_event(schedule_keeper.events[schedule_keeper.next_event]);

	else
	{
		LOG_DEBUG("Unregistered Event Execution Tried");
	}
}


/**
 * @brief Runs events that have been scheduled to run
 * loops till no event is remaining to be executed
 * 
 */
void scheduler_run(void)
{
	if(schedule_keeper.current_state != Events_Registerd)
		LOG_ERROR("No events registered");
	else
	{
		while(get_next_event() == 0)
		{
			execute_next_event();
		}
	}

}
