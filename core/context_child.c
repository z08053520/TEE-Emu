/*****************************************************************************
** Copyright (C) 2013 Intel Corporation.                                    **
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

#include "context_child.h"
#include <syslog.h>
#include <unistd.h>

#include "tee_internal_api.h"

void context_handler_loop(int childfd)
{
	ssize_t n;
	char *buff;

	buff = TEE_Malloc(1024, 0);
	if (!buff) {
		syslog(LOG_DEBUG, "Malloc failed");
		return;
	}

	while ((n = (read(childfd, buff, 1024 - 1))) > 0) {
		buff[n] = '\0';
		syslog(LOG_DEBUG, "%s", buff);
	}

	TEE_Free(buff);
	return;
}