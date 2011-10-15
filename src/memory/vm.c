/* vm.c: Virtual memory management
 * Copyright © 2011 Fritz Grimpen
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

#include "vm.h"
#include <memory/kmalloc.h>
#include <lib/print.h>

#define FIND_NODE(node, cond) { \
	while (!(cond) && node != NULL) \
		node = node->next; \
}

struct vm_context_node
{
	struct vm_page *page;
	struct vm_context_node *next;
};

struct vm_context
{
	union {
		struct vm_context_node *first_node;
		struct vm_context_node *node;
	};
	struct vm_context_node *last_node;
	union {
		uint32_t pages;
		uint32_t nodes;
	};
};

/* Initialize kernel context */
void vm_init()
{
	struct vm_context *ctx = vm_new();
	
	for (int i = 0; i < 0xf424000; i += 4096)
	{
		struct vm_page *page = vm_new_page();
		page->section = VM_SECTION_KERNEL;
		page->cow = 0;
		page->allocated = 1;
		page->virt_addr = (void *)i;
		page->phys_addr = (void *)i;

		vm_add_page(ctx, page);
	}

	vm_kernelContext = ctx;
}

struct vm_context *vm_new()
{
	struct vm_context *ctx = kmalloc(sizeof(struct vm_context));

	ctx->first_node = NULL;
	ctx->last_node = NULL;
	ctx->node = NULL;
	ctx->pages = 0;

	return ctx;
}

struct vm_page *vm_new_page()
{
	struct vm_page *page = kmalloc(sizeof(struct vm_page));
	memset(page, 0, sizeof(struct vm_page));
	return page;
}

int vm_iterate(struct vm_context *ctx, vm_iterator_t iterator)
{
	struct vm_context_node *node = ctx->first_node;
	int i = 0;
	while (node != NULL && i < ctx->nodes)
	{
		iterator(ctx, node->page, i++);
		node = node->next;
	}

	return i;
}

int vm_add_page(struct vm_context *ctx, struct vm_page *pg)
{
	struct vm_context_node *node = kmalloc(sizeof(struct vm_context_node));
	node->page = pg;
	node->next = NULL;
	if (ctx->node == NULL)
	{
		ctx->pages = 1;
		ctx->first_node = node;
		ctx->last_node = node;
		ctx->node = node;
		return 0;
	}

	ctx->last_node->next = node;
	ctx->last_node = node;
	++ctx->pages;

	return 0;
}

struct vm_page *vm_get_page_phys(struct vm_context *ctx, void *phys_addr)
{
	struct vm_context_node *node = ctx->node;

	FIND_NODE(node, node->page->phys_addr == phys_addr);

	if (node == NULL)
		return NULL;
	return node->page;
}

struct vm_page *vm_get_page_virt(struct vm_context *ctx, void *virt_addr)
{
	struct vm_context_node *node = ctx->node;

	FIND_NODE(node, node->page->virt_addr == virt_addr);

	if (node == NULL)
		return NULL;
	return node->page;
}

struct vm_page *vm_get_page(struct vm_context *ctx, uint32_t offset)
{
	struct vm_context_node *node = ctx->node;

	while (offset > 0 && node != NULL)
	{
		node = node->next;
		--offset;
	}

	if (node == NULL)
		return NULL;
	return node->page;
}

struct vm_page *vm_rm_page_phys(struct vm_context *ctx, void *phys_addr)
{
	struct vm_context_node *node = ctx->node;
	struct vm_context_node *prev_node = NULL;

	while (node != NULL && node->page->phys_addr != phys_addr)
	{
		prev_node = node;
		node = node->next;
	}

	if (node == NULL)
		return NULL;

	if (node->next == NULL)
		ctx->last_node = (prev_node != NULL) ? prev_node : NULL;
	if (prev_node == NULL)
		ctx->first_node = NULL;
	else
		prev_node->next = node->next;

	--ctx->pages;
	struct vm_page *retval = node->page;
	kfree(node);
	
	return retval;
}

struct vm_page *vm_rm_page_virt(struct vm_context *ctx, void *virt_addr)
{
	struct vm_context_node *node = ctx->node;
	struct vm_context_node *prev_node = NULL;

	while (node != NULL && node->page->virt_addr != virt_addr)
	{
		prev_node = node;
		node = node->next;
	}

	if (node == NULL)
		return NULL;

	if (node->next == NULL)
		ctx->last_node = (prev_node != NULL) ? prev_node : NULL;
	if (prev_node == NULL)
		ctx->first_node = NULL;
	else
		prev_node->next = node->next;

	--ctx->pages;
	
	struct vm_page *retval = node->page;
	kfree(node);
	
	return retval;
}
