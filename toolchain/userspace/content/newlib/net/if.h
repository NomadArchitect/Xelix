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

#ifndef _NET_IF_H
#define _NET_IF_H

#define IF_NAMESIZE 20

struct if_nameindex {
	unsigned if_index;
	char* if_name;
};

unsigned if_nametoindex(const char*);
char* if_indextoname(unsigned, char*);
struct if_nameindex* if_nameindex(void);
void if_freenameindex(struct if_nameindex*);

#endif /* _NET_IF_H */
