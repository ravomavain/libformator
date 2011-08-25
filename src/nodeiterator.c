/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * nodeiterator.h
 * Copyright (C) Romain Labolle 2011 <ravomavain@gmail.com>
 * 
 * formator is free software copyrighted by Romain Labolle.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Romain Labolle'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 
 * formator IS PROVIDED BY Romain Labolle ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Romain Labolle OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nodeiterator.h"

formator_node_iterator_t *formator_create_iterator(formator_node_t *root, formator_node_iterator_order_t order)
{
	formator_node_iterator_t *iterator = (formator_node_iterator_t*)malloc(sizeof(formator_node_iterator_t));
	if(!iterator || !root)
		return NULL;
	memset (iterator, '\0', sizeof(formator_node_iterator_t));
	iterator->root=root;
	iterator->next=formator_iterator_next;
	iterator->order=order;
	iterator->node=root;
	if (iterator->order&CHILDREN_FIRST)
	{
		if (iterator->order&REVERT_ORDER)
			while(iterator->node->lastchild)
				iterator->node = iterator->node->lastchild;
		else
			while(iterator->node->firstchild)
				iterator->node = iterator->node->firstchild;
	}
	return iterator;
}

formator_node_t *formator_iterator_next(formator_node_iterator_t *iterator)
{
	formator_node_t *node = iterator->node;
	if (iterator->order&CHILDREN_FIRST)
	{
		if(node == iterator->root)
		{
			node = NULL;
		}
		else if (iterator->order&REVERT_ORDER)
		{
			if (node->prev)
			{
				node = node->prev;
				while (node->lastchild)
					node = node->lastchild;
			}
			else if (node->parent)
				node = node->parent;
			else
				node = NULL;
		}
		else
		{
			if (node->next)
			{
				node = node->next;
				while (node->firstchild)
					node = node->firstchild;
			}
			else if (node->parent)
				node = node->parent;
			else
				node = NULL;
		}
	}
	else
	{
		if (iterator->order&REVERT_ORDER)
		{
			if(node->lastchild)
				node = node->lastchild;
			else if(node->prev)
				node = node->prev;
			else if(node->parent)
			{
				while(node->parent && !node->parent->prev && node->parent->depth > iterator->root->depth)
					node = node->parent;
				if (node->parent && node->parent->depth > iterator->root->depth)
					node = node->parent->prev;
				else
					node = NULL;
			}
		}
		else
		{
			if(node->firstchild)
				node = node->firstchild;
			else if(node->next)
				node = node->next;
			else if(node->parent)
			{
				while(node->parent && !node->parent->next && node->parent->depth > iterator->root->depth)
					node = node->parent;
				if (node->parent && node->parent->depth > iterator->root->depth)
					node = node->parent->next;
				else
					node = NULL;
			}
		}
	}
	iterator->node = node;
	return node;
}