/*****************************************************************************
** Copyright (C) 2014 Intel Corporation.                                    **
**                                                                          **
** Licensed under the Apache License, Version 2.0 (the "License");          **
** you may not use this file except in compliance with the License.         **
** You may obtain a copy of the License at                                  **
**                                                                          **
**      http://www.apache.org/licenses/LICENSE-2.0                          **
**                                                                          **
** Unless required by applicable law or agreed to in writing, software      **
** distributed under the License is distributed on an "AS IS" BASIS,        **
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. **
** See the License for the specific language governing permissions and      **
** limitations under the License.                                           **
*****************************************************************************/

#ifndef __TA_EXTERN_RESOURCES_H__
#define __TA_EXTERN_RESOURCES_H__

#include <pthread.h>

#include "tee_list.h"

/* Struct ta_task will be used for communication between thread */
struct ta_task {
	struct list_head list;
	void *msg;
	int msg_len;
};

/* These are for tasks received from the caller going to the TA */
extern struct ta_task tasks_todo;

/* These are for tasks that are complete and are being returned to the caller */
extern struct ta_task tasks_done;

/* Interface TA funcitons */
extern struct ta_interface *interface;

/* we have 2 threads to synchronize so we can achieve this with static condition and statix mutex */
extern pthread_mutex_t todo_list_mutex;
extern pthread_mutex_t done_list_mutex;
extern pthread_cond_t condition;

/* Use eventfd to notify the io_thread that the TA thread has finished processing a task */
extern int event_fd;

#endif /* __TA_EXTERN_RESOURCES_H__ */
