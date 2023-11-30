/*
 * scheduler.h
 *
 *  Created on: Nov 30, 2023
 *      Author: Truc Le
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include <stdint.h>
extern uint8_t TICK;
typedef struct {
	// Pointer to the Task (mus be a 'void(void)' function)
	void (* pTask)(void);
	// Delay until the function will next be run
	uint32_t Delay;
	// Period between two runs
	uint32_t Period;
	// Incremented by scheduler when a task is due to execute
	uint32_t RunMe;
	// TaskID to manage tasks
	uint32_t TaskID;
} sTask;

#define SCH_MAX_TASKS	10
#define NO_TASK_ID		0

// Prototype of functions needed to operate a scheduler

// Initialization function
void SCH_Init(void);

// Add task function to add tasks to the task array
void SCH_Add_Task(void (* pFunction)(), uint32_t DELAY, uint32_t PERIOD);

// Update function, put in timer interupt
void SCH_Update(void);

// Dispatch function to execute tasks that are due to run
void SCH_Dispatch_Tasks(void);

// Delete task function to delete task from the task array
void SCH_Delete_Tasks(const unsigned char TASK_INDEX);

#endif /* INC_SCHEDULER_H_ */
