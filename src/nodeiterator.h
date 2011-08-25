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
#ifndef FORMATOR_NODEITERATOR_H
#define FORMATOR_NODEITERATOR_H

#if defined FORMATOR_LIBRARY
#  include "node.h"
#else
#  include <formator/node.h>
#endif

typedef enum formator_node_iterator_order
{
	PARENT_FIRST=0, /*This is only for default value, shouldn't be checked as flag*/
	CHILDREN_FIRST=1,
	REVERT_ORDER=2
} formator_node_iterator_order_t;

typedef struct formator_node_iterator
{
	formator_node_t *root;
	formator_node_t *node;
	formator_node_t *(*next)(struct formator_node_iterator* iterator);
	formator_node_iterator_order_t order;
} formator_node_iterator_t;

formator_node_iterator_t *formator_create_iterator(formator_node_t *root, formator_node_iterator_order_t order);
formator_node_t *formator_iterator_next(formator_node_iterator_t *iterator);

#endif /*FORMATOR_NODEITERATOR_H*/