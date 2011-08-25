/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * nodetype.h
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
#ifndef NODETYPE_H
#define NODETYPE_H

#include <stdio.h>
#include <stdint.h>

typedef enum
{
	UNSPECIFIED = 0,
	BIG,
	LITTLE,
} Endianness;

struct formator_node;
struct formator_printf_info;

typedef int (*formator_node_printf)(FILE *stream, struct formator_node *node, struct formator_printf_info info);

typedef struct formator_node_type
{
	const char *name;
	const char *description; 
	size_t blocksize;
	Endianness endianness;
	formator_node_printf printf;
} formator_node_type_t;

int formator_nodetype_cmp(formator_node_type_t *type1, formator_node_type_t *type2);

extern formator_node_type_t FormatorNodeTypeRoot;
extern formator_node_type_t FormatorNodeTypeData;
extern formator_node_type_t FormatorNodeTypeHex;
extern formator_node_type_t FormatorNodeTypeString;

extern formator_node_type_t FormatorNodeTypeUInt8;
extern formator_node_type_t FormatorNodeTypeUInt16;
extern formator_node_type_t FormatorNodeTypeUInt32;
extern formator_node_type_t FormatorNodeTypeUInt64;

extern formator_node_type_t FormatorNodeTypeInt8;
extern formator_node_type_t FormatorNodeTypeInt16;
extern formator_node_type_t FormatorNodeTypeInt32;
extern formator_node_type_t FormatorNodeTypeInt64;

extern formator_node_type_t FormatorNodeTypeUIntbe16;
extern formator_node_type_t FormatorNodeTypeUIntbe32;
extern formator_node_type_t FormatorNodeTypeUIntbe64;

extern formator_node_type_t FormatorNodeTypeIntbe16;
extern formator_node_type_t FormatorNodeTypeIntbe32;
extern formator_node_type_t FormatorNodeTypeIntbe64;

extern formator_node_type_t FormatorNodeTypeUIntle16;
extern formator_node_type_t FormatorNodeTypeUIntle32;
extern formator_node_type_t FormatorNodeTypeUIntle64;

extern formator_node_type_t FormatorNodeTypeIntle16;
extern formator_node_type_t FormatorNodeTypeIntle32;
extern formator_node_type_t FormatorNodeTypeIntle64;

#endif /*NODETYPE_H*/