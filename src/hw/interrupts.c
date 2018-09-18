/* interrupts.c: Initialization of and interface to interrupts.
 * Copyright © 2011-2018 Lukas Martini
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
 * along with Xelix.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <hw/interrupts.h>
#include <string.h>
#include <hw/idt.h>
#include <tasks/scheduler.h>
#include <memory/vmem.h>
#include <memory/paging.h>
#include <memory/gdt.h>

#define debug(args...) log(LOG_DEBUG, "interrupts: " args)

/* This one get's called from the architecture-specific interrupt
 * handlers, which do fiddling like EOIs (i386).
 */
cpu_state_t* __attribute__((fastcall)) interrupts_callback(cpu_state_t* regs) {
	interrupt_handler_t handler = interrupt_handlers[regs->interrupt];
	task_t* task = scheduler_get_current();

	#ifdef INTERRUPTS_DEBUG
	debug("state before:\n");
	dump_cpu_state(LOG_DEBUG, regs);
	#endif

	if(handler != NULL)
		handler(regs);

	// Timer interrupt
	if(scheduler_state != SCHEDULER_OFF && (regs->interrupt == IRQ0 || (task && task->interrupt_yield)))
	{
		if((task && task->interrupt_yield)) {
			task->interrupt_yield = false;
		}

		task_t* new_task = scheduler_select(regs);

		if(new_task && new_task->state && new_task->memory_context)
		{
			#ifdef INTERRUPTS_DEBUG
			debug("state after (task selection):\n");
			dump_cpu_state(LOG_DEBUG, new_task->state);
			#endif

			gdt_set_tss(new_task->kernel_stack + PAGE_SIZE);
			return new_task->state;
		}
	}

	#ifdef INTERRUPTS_DEBUG
	debug("state after:\n");
	dump_cpu_state(LOG_DEBUG, regs);
	#endif
	return regs;
}

void interrupts_init() {
	idt_init();
	bzero(interrupt_handlers, sizeof(interrupt_handlers));
	interrupts_enable();
}
