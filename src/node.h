/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * node.h
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
#ifndef FORMATOR_NODE_H
#define FORMATOR_NODE_H

#if defined FORMATOR_LIBRARY
#  include "nodetype.h"
#else
#  include <formator/nodetype.h>
#endif

typedef enum DataLocation
{
	DATALOC_MEMORY,
	DATALOC_MEMORY_AND_FILE,
	DATALOC_FILE
} DataLocation;

typedef struct formator_node
{
	struct formator_node *parent;
	struct formator_node *firstchild;
	struct formator_node *lastchild;
	struct formator_node *next;
	struct formator_node *prev;
	struct formator_node *root;

	void *data;
	void *data_start;
	FILE *file;
	off_t allocated;
	off_t size;
	off_t offset;
	off_t total_size;
	int depth;
	formator_node_type_t *type;
	void *extra;
	Endianness *endianness;
	DataLocation location;
} formator_node_t;

formator_node_t *formator_new_node(formator_node_type_t *type, off_t length, formator_node_t *parent, void *extra);
formator_node_t *formator_create_root(void *data, off_t size, Endianness *endianness);
formator_node_t *formator_create_root_file(FILE *file, off_t size, Endianness *endianness);

void formator_node_free(formator_node_t *node);

void *formator_node_get(formator_node_t *node);
void *formator_node_get_data(formator_node_t *node, size_t size);
char *formator_node_get_string(formator_node_t *node);
uint8_t formator_node_get_uint8(formator_node_t *node);
uint16_t formator_node_get_uint16(formator_node_t *node);
uint32_t formator_node_get_uint32(formator_node_t *node);
uint64_t formator_node_get_uint64(formator_node_t *node);
int8_t formator_node_get_int8(formator_node_t *node);
int16_t formator_node_get_int16(formator_node_t *node);
int32_t formator_node_get_int32(formator_node_t *node);
int64_t formator_node_get_int64(formator_node_t *node);

#endif /*FORMATOR_NODE_H*/