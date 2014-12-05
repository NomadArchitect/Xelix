/* syscall.c: Syscalls
 * Copyright © 2011 Lukas Martini, Fritz Grimpen
 * Copyright © 2012-2014 Lukas Martini
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

#include "syscall.h"
#include <lib/generic.h>
#include <interrupts/interface.h>
#include <lib/log.h>
#include <tasks/scheduler.h>
#include <lib/print.h>
#include <lib/panic.h>
#include <memory/vmem.h>

#include "syscalls.h"

/* Resolves a virtual address from within the virtual address space of the
 * current task to the corresponding physical address we can use.
 */
intptr_t task_resolve_address(intptr_t virt_address)
{
	task_t* task = scheduler_get_current();

	int diff = virt_address % PAGE_SIZE;
	virt_address -= diff;

	struct vmem_page* page = vmem_get_page_virt(task->memory_context, (void*)virt_address);

	if(!page)
		return NULL;

	return (intptr_t)page->phys_addr + diff;
}

static void int_handler(cpu_state_t* regs)
{
	task_t* current_task = scheduler_get_current();
	if(!current_task) {
		panic("Don't have a current task while in a syscall - This is very wrong.\n");
	}

	// Save state of the current task so we can conveniently use it in the handler.
	// (The other place where the state of a task is saved is the scheduling routine).
	current_task->state = regs;

	struct syscall syscall;
	syscall.num = regs->eax;
	syscall.params[0] = regs->ebx;
	syscall.params[1] = regs->ecx;
	syscall.params[2] = regs->edx;
	syscall.params[3] = regs->esi;
	syscall.params[4] = regs->edi;
	syscall.params[5] = (int)regs->ebp;

	syscall_t call = syscall_table[syscall.num];
	if (syscall.num >= sizeof(syscall_table) / sizeof(syscall_t) || call == NULL)
	{
		log(LOG_INFO, "syscall: Invalid syscall %d\n", syscall.num);
		syscall.num = -1;
		return;
	}

	/*log(LOG_INFO, "Syscall %s called with %d %d %d %d %d.\n",
		syscall_name_table[syscall.num],
		regs->ebx,
		regs->ecx,
		regs->edx,
		regs->esi,
		regs->edi,
		regs->ebp);*/

	regs->eax = call(syscall);
}

void syscall_init()
{
	interrupts_registerHandler(SYSCALL_INTERRUPT, int_handler);
}
