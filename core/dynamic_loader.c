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

#include "dynamic_loader.h"

#include <syslog.h>
#include <dlfcn.h>
#include <string.h>

#define MAX_PATH 255

/*!
 * \brief get_library
 * Based on the UUID of the TA try and find the library where it is implemented
 * \param id The ID of the TA
 * \param lib_path The path where the library is located on success
 * \param size The available space in lib_path
 * \return TEE_SUCCESS on success another value on failure.
 */
static TEE_Result get_library(const TEE_UUID id, char *lib_path, uint32_t size)
{
	//TODO this whole function :)
	char *path = "/home/brian/code/ccpp/project-build/qtc_Desktop-debug/libtest_applet.so";

	if (!lib_path)
		return TEE_ERROR_BAD_PARAMETERS;

	(void)id;
	memcpy(lib_path, path, size);
	return TEE_SUCCESS;
}

TEE_Result load_ta(const TEE_UUID id, struct ta_interface **callbacks)
{
	struct ta_interface tmp_cb;
	char path[MAX_PATH];
	TEE_Result ret;
	char *err = NULL;

	memset((void *)&tmp_cb, 0, sizeof(struct ta_interface));
	*callbacks = NULL;

	if ((ret = get_library(id, path, MAX_PATH))) {
		return ret;
	}

	dlerror();

	tmp_cb.library = dlopen(path, RTLD_LAZY);
	if (!tmp_cb.library) {
		syslog(LOG_DEBUG, "Failed to load library : %s : %s", path, dlerror());
		return TEE_ERROR_GENERIC;
	}

	/* To be a valid TA it must not have any errors when loading the lbrary AND
	 * it MUST provide each of the 5 entry functions listed below!!
	 */

	*(void **)(&tmp_cb.create) = dlsym(tmp_cb.library, "TA_CreateEntryPoint");
	if ((err = dlerror()) != NULL || !tmp_cb.create) {
		syslog(LOG_DEBUG, "Failed to find CreateEntryPoint : %s : %s", path, err);
		goto err_cleanup;
	}

	*(void **)(&tmp_cb.destroy) = dlsym(tmp_cb.library, "TA_DestroyEntryPoint");
	if ((err = dlerror()) != NULL || !tmp_cb.destroy) {
		syslog(LOG_DEBUG, "Failed to find DestroyEntryPoint : %s : %s", path, err);
		goto err_cleanup;
	}

	*(void **)(&tmp_cb.open_session) = dlsym(tmp_cb.library, "TA_OpenSessionEntryPoint");
	if ((err = dlerror()) != NULL || !tmp_cb.open_session) {
		syslog(LOG_DEBUG, "Failed to find OpenSessionEntryPoint : %s : %s", path, err);
		goto err_cleanup;
	}

	*(void **)(&tmp_cb.invoke_cmd) = dlsym(tmp_cb.library, "TA_InvokeCommandEntryPoint");
	if ((err = dlerror()) != NULL || !tmp_cb.invoke_cmd) {
		syslog(LOG_DEBUG, "Failed to find InvokeCommandEntryPoint : %s : %s", path, err);
		goto err_cleanup;
	}

	*(void **)(&tmp_cb.close_session) = dlsym(tmp_cb.library, "TA_CloseSessionEntryPoint");
	if ((err = dlerror()) != NULL || !tmp_cb.close_session) {
		syslog(LOG_DEBUG, "Failed to find CloseSession Entry point : %s : %s", path, err);
		goto err_cleanup;
	}

	*callbacks = TEE_Malloc(sizeof(struct ta_interface), 0);
	if (!*callbacks) {
		syslog(LOG_DEBUG, "Out of memory");
		goto err_cleanup;
	}

	memcpy(*callbacks, (void *)&tmp_cb, sizeof(struct ta_interface));

err_cleanup:
	if (err || !*callbacks) {
		if (tmp_cb.destroy)
			tmp_cb.destroy();
		dlclose(tmp_cb.library);
		return TEE_ERROR_GENERIC;
	}

	return TEE_SUCCESS;
}

void unload_ta(struct ta_interface *callbacks)
{
	if (!callbacks)
		return;

	dlerror();

	/* Call the TA cleanup routine */
	callbacks->destroy();

	if (dlclose(callbacks->library)) {
		syslog(LOG_DEBUG, "Error while closing library : %s", dlerror());
	}

	TEE_Free(callbacks);

	return;
}