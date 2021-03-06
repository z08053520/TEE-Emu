/*****************************************************************************
** Copyright (C) 2014 Tanel Dettenborn                                      **
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

#include <gelf.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "elf_read.h"
#include "tee_logging.h"

#define MAX_SEC_NAME_LEN 20

int get_data_from_elf(const char *elf_file, const char *sec_name, void *buf, size_t *buf_len)
{
	int fd, is_sec_found = 1;
	Elf *e;
	char *name;
	Elf_Scn *scn = NULL;
	Elf_Data *data = NULL;
	GElf_Shdr shdr;
	size_t shstrndx;

	if (elf_version(EV_CURRENT) == EV_NONE) {
		OT_LOG(LOG_ERR, "Elf version");
		return 1;
	}

	fd = open(elf_file, O_RDONLY, 0);
	if (!fd) {
		OT_LOG(LOG_ERR, "Open file");
		return 1;
	}

	e = elf_begin(fd, ELF_C_READ, NULL);
	if (!e) {
		OT_LOG(LOG_ERR, "Elf begin");
		goto end;
	}

	if (elf_kind(e) != ELF_K_ELF) {
		OT_LOG(LOG_ERR, "elf kind");
		goto end;
	}

	if (elf_getshdrstrndx(e, &shstrndx) != 0) {
		OT_LOG(LOG_ERR, "elf kind");
		goto end;
	}

	while (1) {

		scn = elf_nextscn(e, scn);
		if (!scn)
			break;

		if (gelf_getshdr(scn, &shdr) != &shdr) {
			OT_LOG(LOG_ERR, "gelf getshdr");
			goto end;
		}

		name = elf_strptr(e, shstrndx, shdr.sh_name);
		if (!name) {
			OT_LOG(LOG_ERR, "elf_strptr");
			goto end;
		}

		if (!strncasecmp(sec_name, name, MAX_SEC_NAME_LEN)) {
			data = elf_getdata(scn, data);
			if (!data) {
				OT_LOG(LOG_ERR, "elf_getdata");
				goto end;
			}

			if (*buf_len >= data->d_size) {
				memcpy(buf, data->d_buf, data->d_size);
				is_sec_found = 0;
				*buf_len = data->d_size;

			} else {
				OT_LOG(LOG_ERR, "Buffer too small");
				goto end;
			}
		}
	}

end:
	if (is_sec_found)
		OT_LOG(LOG_ERR, "section not found");

	elf_end(e);
	close(fd);
	return is_sec_found;
}
