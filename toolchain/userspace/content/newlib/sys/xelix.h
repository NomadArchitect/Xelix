/* Copyright © 2018 Lukas Martini
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

#ifndef _SYS_XELIX_H
#define _SYS_XELIX_H

struct _xelix_execdata {
	uint32_t pid;
	uint32_t ppid;
	uint32_t argc;
	uint32_t envc;

	// Contains argc arguments, then environment until NULL.
	char* argv_environ[];
};

struct _xelix_execdata* _xelix_execdata;

pid_t execnew(const char* path, char* __argv[], char* __env[]);
int _xelix_getexecdata();

#endif /* _SYS_XELIX_H */