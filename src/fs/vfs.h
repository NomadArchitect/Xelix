#pragma once

/* Copyright © 2010, 2011 Lukas Martini
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

#include <lib/generic.h>

typedef struct {
   uint64_t num;
   char path[512];
   char mount_path[512];
   uint32_t offset;
   uint32_t mountpoint;
} vfs_file_t;

typedef void* (*vfs_read_callback_t)(char* path, uint32_t offset);

vfs_file_t* vfs_get_from_id(uint32_t id);
void* vfs_read(vfs_file_t* fp);
vfs_file_t* vfs_open(char* path);
int vfs_mount(char* path, vfs_read_callback_t read_callback);