/* task.c: Task execdata setup
 * Copyright © 2018 Lukas Martini
 *
 * This file is part of Xelix.
 *
 * Xelix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Xelix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Xelix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <tasks/task.h>
#include <memory/vmem.h>
#include <memory/kmalloc.h>
#include <string.h>

#define EXECDATA_LOCATION 0x5000

struct execdata {
	uint32_t pid;
	uint32_t ppid;
	uint32_t argc;
	uint32_t envc;
	void** argv;
	void** env;
};

/* Sets up a single page of runtime data for the program, including PID, argv,
 * environment etc.
 *
 * Page layout:
 * - struct execdata
 * - char** argv
 * - argv strings
 * - char** environ
 * - environ strings / free space for new environment variables
 */
void task_setup_execdata(task_t* task) {
	void* page = tmalloc_a(PAGE_SIZE, task);
	vmem_map(task->memory_context, (void*)EXECDATA_LOCATION, page, PAGE_SIZE, VMEM_SECTION_DATA);

	struct execdata* exc = (struct execdata*)page;
	char** argv = (char**)exc + sizeof(struct execdata);
	intptr_t args = (intptr_t)argv + sizeof(char*) * (task->argc + 1);

	uint32_t offset = 0;
	for(int i = 0; i < task->argc; i++) {
		strncpy((char*)(args + offset), task->argv[i], 200);
		argv[i] = (char*)vmem_translate(task->memory_context, args + offset, true);
		offset += strlen(task->argv[i]) + 1;
	}

	char** environ = (char**)args + offset;
	intptr_t env = (intptr_t)environ + sizeof(char*) * (task->envc + 1);

	offset = 0;
	for(int i = 0; i < task->envc; i++) {
		strncpy((char*)(env + offset), task->environ[i], 200);
		environ[i] = (char*)vmem_translate(task->memory_context, env + offset, true);
		offset += strlen(task->environ[i]) + 1;
	}

	exc->pid = task->pid;
	exc->ppid = task->parent ? task->parent->pid : 0;
	exc->argc = task->argc;
	exc->envc = task->envc;
	exc->argv = (void*)vmem_translate(task->memory_context, (intptr_t)argv, true);
	exc->env = (void*)vmem_translate(task->memory_context, (intptr_t)environ, true);
}
