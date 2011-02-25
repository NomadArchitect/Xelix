/* generic.c: Generic CPU-specific commands.
 * Copyright © 2010 Lukas Martini
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

#include "interface.h"

#include "fault.h"
#include <common/log.h>

int protected;

// Check if CPU is running in protected mode.
static bool isProtected()
{
	uint32 v;
	asm ( "mov %%cr0, %0":"=a"(v) );
	if(v & 1) return true;
	else return false;
}

// Set CPU in protected mode
// Todo: Implement it
static void setProtected()
{
	PANIC("Can't [yet] switch to protected mode. Use a bootloader which automatically enables protected mode such as GNU GRUB.\n");
	// log("cpu: Switched to protected mode.\n");
}

// Check if CPU is 32 bit
// Todo: Implement me
bool cpu_is32Bit()
{
	return true;
}

// Initialize the CPU (set it to protected mode)
void cpu_init()
{
	protected = isProtected();
	if(!protected)
		setProtected();
	else
		log("cpu: Already in protected mode.\n");
		
	cpu_initFaultHandler();
}
