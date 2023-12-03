/*
 * scheduler.c
 *
 *  Created on: Nov 30, 2023
 *      Author: Truc Le
 */


#include "scheduler.h"
#include "main.h"

uint8_t TICK;

// An array to store tasks's information
sTask SCH_tasks_G[SCH_MAX_TASKS];

void SCH_Init(void)
{
	// Loop through each element of the SCH_tasks_G array
	for (int i = 0; i < SCH_MAX_TASKS; i++) {
		SCH_tasks_G[i].pTask = 0x0000;
		SCH_tasks_G[i].Delay = 0;
		SCH_tasks_G[i].Period = 0;
		SCH_tasks_G[i].RunMe = 0;
	}
}

void SCH_Update(void)
{
	if(SCH_tasks_G[0].pTask && SCH_tasks_G[0].RunMe == 0)
	{
		if(SCH_tasks_G[0].Delay > 0)
		{
			SCH_tasks_G[0].Delay--;
		} else {
			SCH_tasks_G[0].RunMe = 1;
		}
	}
}

void SCH_Add_Task(void (* pFunction)(), uint32_t DELAY_IN_MS, uint32_t PERIOD)
{
	uint32_t DELAY = DELAY_IN_MS/TICK;

	uint8_t newTaskIndex = 0;
	uint32_t sumDelay = 0;
	uint32_t newTaskDelay = 0;

	for (newTaskIndex = 0; newTaskIndex < SCH_MAX_TASKS; ++newTaskIndex) {
		sumDelay = sumDelay + SCH_tasks_G[newTaskIndex].Delay;

		if(sumDelay > DELAY)
		{
			newTaskDelay = DELAY - (sumDelay - SCH_tasks_G[newTaskIndex].Delay);
			SCH_tasks_G[newTaskIndex].Delay = sumDelay - DELAY;

			for (uint8_t i = SCH_MAX_TASKS-1; i > newTaskIndex ; --i) {
				if(SCH_tasks_G[i-1].pTask != 0x000)
				{
					SCH_tasks_G[i].pTask = SCH_tasks_G[i-1].pTask;
					SCH_tasks_G[i].Delay = SCH_tasks_G[i-1].Delay;
					SCH_tasks_G[i].Period = SCH_tasks_G[i-1].Period;
					// except delay == 0 which will be set afterwards, other next task still have RunMe == 0 so the below line is not necessary
					// SCH_tasks_G[i].RunMe = SCH_tasks_G[i-1].RunMe;
				}
			}

			SCH_tasks_G[newTaskIndex].pTask = pFunction;
			SCH_tasks_G[newTaskIndex].Delay = newTaskDelay;
			SCH_tasks_G[newTaskIndex].Period = PERIOD/TICK;

			if(SCH_tasks_G[newTaskIndex].Delay == 0)
			{
				SCH_tasks_G[newTaskIndex].RunMe = 1;
			} else {
				SCH_tasks_G[newTaskIndex].RunMe = 0;
			}

			return;
		} else {
			// check if there is empty slot for the new task that have Delay > sumDelay
			if(SCH_tasks_G[newTaskIndex].pTask == 0x0000)
			{
				SCH_tasks_G[newTaskIndex].pTask = pFunction;
				SCH_tasks_G[newTaskIndex].Delay = DELAY - sumDelay;
				SCH_tasks_G[newTaskIndex].Period = PERIOD/TICK;

				if(SCH_tasks_G[newTaskIndex].Delay == 0)
				{
					SCH_tasks_G[newTaskIndex].RunMe = 1;
				} else {
					SCH_tasks_G[newTaskIndex].RunMe = 0;
				}

				return;
			}
		}
	}
}


void SCH_Dispatch_Tasks(void)
{
	if(SCH_tasks_G[0].RunMe > 0)
	{
		(* SCH_tasks_G[0].pTask)();
		SCH_tasks_G[0].RunMe = 0;
		sTask temp = SCH_tasks_G[0];
		SCH_Delete_Tasks(0);

		// readd the task that is not oneshot
		if(temp.Period != 0)
		{
			SCH_Add_Task(temp.pTask, temp.Period*TICK, temp.Period*TICK);
		}
	}
}

void SCH_Delete_Tasks(uint8_t TASK_INDEX)
{
	uint8_t	taskIndex;
	uint8_t j;

	for (taskIndex = 0; taskIndex < SCH_MAX_TASKS; ++taskIndex) {
		if(taskIndex == TASK_INDEX)
		{
			if(taskIndex > 0 && taskIndex < SCH_MAX_TASKS - 1)
			{
				if(SCH_tasks_G[taskIndex+1].pTask != 0x0000)
				{
					// add deleted task's Delay to the next task so the next task still have correct Delay
					SCH_tasks_G[taskIndex+1].Delay += SCH_tasks_G[taskIndex].Delay;
				}
			}


			for (j = taskIndex; j < SCH_MAX_TASKS - 1; ++j) {
				SCH_tasks_G[j].pTask = SCH_tasks_G[j+1].pTask;
				SCH_tasks_G[j].Delay = SCH_tasks_G[j+1].Delay;
				SCH_tasks_G[j].Period = SCH_tasks_G[j+1].Period;
			}

			SCH_tasks_G[j].pTask = 0x0000;
			SCH_tasks_G[j].Delay = 0;
			SCH_tasks_G[j].Period = 0;
			SCH_tasks_G[j].RunMe = 0;

			return;
		}
	}
}
