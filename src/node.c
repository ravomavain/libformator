/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * node.c
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
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "node.h"

formator_node_t *formator_create_root(void *data, off_t size, Endianness *endianness)
{
	formator_node_t *root = (formator_node_t*)malloc(sizeof(formator_node_t));
	if(!root)
		return NULL;
	memset(root, 0, sizeof(formator_node_t));

	root->parent = NULL;
	root->firstchild = NULL;
	root->lastchild = NULL;
	root->next = NULL;
	root->prev = NULL;
	root->data = data;
	root->data_start = data;
	root->allocated = size;
	root->file = NULL;
	root->offset = 0;
	root->flags.mem_allocated = 1;
	root->size = size;
	root->total_size = size;
	root->type = &FormatorNodeTypeRoot;
	root->extra = NULL;
	root->root = root;
	root->depth = 0;
	root->endianness = endianness;
	return root;
}

formator_node_t *formator_create_root_file(FILE *file, off_t size, Endianness *endianness)
{
	formator_node_t *root = (formator_node_t*)malloc(sizeof(formator_node_t));
	if(!root)
		return NULL;
	memset(root, 0, sizeof(formator_node_t));

	root->parent = NULL;
	root->firstchild = NULL;
	root->lastchild = NULL;
	root->next = NULL;
	root->prev = NULL;
	root->data = NULL;
	root->data_start = NULL;
	root->allocated = 0;
	root->file = file;
	root->offset = 0;
	root->flags.from_file = 1;
	root->size = size;
	root->total_size = size;
	root->type = &FormatorNodeTypeRoot;
	root->extra = NULL;
	root->root = root;
	root->depth = 0;
	root->endianness = endianness;
	return root;
}

formator_node_t *formator_new_node(formator_node_type_t *type, off_t length, formator_node_t *parent, void *extra)
{
	if(!parent)
		return NULL;
	if(parent->size < length*type->blocksize)
		return NULL;
	formator_node_t *node = (formator_node_t*)malloc(sizeof(formator_node_t));
	if(!node)
		return NULL;
	memset(node, 0, sizeof(formator_node_t));

	node->type = type;
	node->size = length*type->blocksize;
	node->parent = parent;
	node->firstchild = NULL;
	node->lastchild = NULL;
	node->next = NULL;
	node->prev = parent->lastchild;
	
	if(parent->lastchild)
			parent->lastchild->next = node;
	parent->lastchild = node;
	if(!parent->firstchild)
			parent->firstchild = node;

	node->file = parent->file;
	node->data = NULL;
	node->data_start = NULL;
	node->offset = parent->offset;
	parent->offset += node->size;

	if (parent->flags.from_file)
	{
		node->flags.from_file = 1;
		if (parent->flags.mem_allocated)
		{
			free(parent->data_start);
			parent->data = NULL;
			parent->flags.mem_allocated = 0;
			parent->allocated = 0;
		}
	}
	else if(parent->flags.mem_allocated)
	{
		node->flags.mem_allocated = 1;
		node->data = parent->data;
		parent->data += node->size;
	}

	parent->size -= node->size;
	node->total_size = node->size;

	node->extra = extra;
	node->root = parent->root;
	node->depth = parent->depth + 1;
	node->endianness = parent->endianness;

	if (parent->flags.dyn_size && parent->size < 0)
			formator_node_set_size (parent, parent->total_size - parent->size);

	return node;
}

formator_node_t *formator_new_dyn_node(formator_node_type_t *type, formator_node_t *size_node, formator_node_t *parent, void *extra)
{
	if(!parent)
		return NULL;
	formator_node_t *node = (formator_node_t*)malloc(sizeof(formator_node_t));
	if(!node)
		return NULL;
	memset(node, 0, sizeof(formator_node_t));

	if(size_node)
		node->size = (off_t)formator_node_get_int (size_node);
	node->size_node = size_node;
	node->flags.dyn_size = 1;

	node->type = type;
	node->parent = parent;
	node->firstchild = NULL;
	node->lastchild = NULL;
	node->next = NULL;
	node->prev = parent->lastchild;

	if(parent->lastchild)
			parent->lastchild->next = node;
	parent->lastchild = node;
	if(!parent->firstchild)
			parent->firstchild = node;

	node->file = parent->file;
	node->data = NULL;
	node->data_start = NULL;
	node->offset = parent->offset;
	parent->offset += node->size;

	if (parent->flags.from_file)
	{
		node->flags.from_file = 1;
		if (parent->flags.mem_allocated)
		{
			free(parent->data_start);
			parent->data = NULL;
			parent->flags.mem_allocated = 0;
			parent->allocated = 0;
		}
	}
	else if(parent->flags.mem_allocated)
	{
		node->flags.mem_allocated = 1;
		node->data = parent->data;
		parent->data += node->size;
	}

	parent->size -= node->size;
	node->total_size = node->size;

	node->extra = extra;
	node->root = parent->root;
	node->depth = parent->depth + 1;
	node->endianness = parent->endianness;

	if (parent->flags.dyn_size && parent->size < 0)
			formator_node_set_size (parent, parent->total_size - parent->size);

	return node;
}

void formator_node_free(formator_node_t *node)
{
	if((node->flags.mem_allocated && node->flags.from_file)
	 || node == node->root)
		free(node->data_start);	
}

void *formator_node_get(formator_node_t *node)
{
	return formator_node_get_data(node, node->size);
}

void *formator_node_get_data(formator_node_t *node, size_t size) /* size will sometime be set by a off_t var (which is bigger than size_t) but we can't malloc more than a size_t */
{
	if(!node->flags.from_file)
		return node->data;
	if(size > node->size)
		size = node->size;
	if(!node->flags.mem_allocated)
	{
		node->data = malloc(size);
		if (node->data)
		{
			fseeko (node->file, node->offset, SEEK_SET);
			fread (node->data, sizeof(char), size, node->file);
			if(errno)
			{
				free(node->data);
				return node->data = NULL;
			}
			node->data_start = node->data;
			node->flags.mem_allocated = 1;
			node->allocated = size;
		}
	}
	else if(node->allocated > 0 && node->allocated < size)
	{
		void *temp = realloc(node->data_start, size);
		if(temp)
		{
			node->data = temp;
			fseeko (node->file, node->offset, SEEK_SET);
			fread (node->data, sizeof(char), size, node->file);
			if(errno)
			{
				free(node->data);
				node->allocated = 0;
				node->flags.mem_allocated = 0;
				return node->data = NULL;
			}
			node->data_start = node->data;
			node->allocated = size;
		}
	}
	return node->data;/* may return NULL...*/
}

char *formator_node_get_string(formator_node_t *node)
{
	char *str = (char*)formator_node_get(node);
	if(str)
		return  str;
	return "";
}

uint8_t formator_node_get_uint8(formator_node_t *node)
{
	uint8_t *ret = (uint8_t*)formator_node_get_data(node, sizeof(uint8_t));
	if(!ret)
		return 0;
	return *ret;
}

int8_t formator_node_get_int8(formator_node_t *node)
{
	int8_t *ret = (int8_t*)formator_node_get_data(node, sizeof(int8_t));
	if(!ret)
		return 0;
	return *ret;
}

uint16_t formator_node_get_uint16(formator_node_t *node)
{
	uint16_t *ret = (uint16_t*)formator_node_get_data(node, sizeof(uint16_t));
	if(!ret)
		return 0;/* hope this one can't be messed up with endianness :D*/
	if (node->type->endianness == BIG)
		return be16toh(*ret);
	if (node->type->endianness == LITTLE)
		return le16toh(*ret);
	if (*node->endianness == BIG)
		return be16toh(*ret);
	if (*node->endianness == LITTLE)
		return le16toh(*ret);
	return *ret;
}

int16_t formator_node_get_int16(formator_node_t *node)
{
	uint16_t *ret = (uint16_t*)formator_node_get_data(node, sizeof(uint16_t));
	if(!ret)
		return 0;
	if (node->type->endianness == BIG)
		return (int16_t)be16toh(*ret);
	if (node->type->endianness == LITTLE)
		return (int16_t)le16toh(*ret);
	if (*node->endianness == BIG)
		return (int16_t)be16toh(*ret);
	if (*node->endianness == LITTLE)
		return (int16_t)le16toh(*ret);
	return *(int16_t*)ret;
}

uint32_t formator_node_get_uint32(formator_node_t *node)
{
	uint32_t *ret = (uint32_t*)formator_node_get_data(node, sizeof(uint32_t));
	if(!ret)
		return 0;
	if (node->type->endianness == BIG)
		return be32toh(*ret);
	if (node->type->endianness == LITTLE)
		return le32toh(*ret);
	if (*node->endianness == BIG)
		return be32toh(*ret);
	if (*node->endianness == LITTLE)
		return le32toh(*ret);
	return *ret;
}

int32_t formator_node_get_int32(formator_node_t *node)
{
	uint32_t *ret = (uint32_t*)formator_node_get_data(node, sizeof(uint32_t));
	if(!ret)
		return 0;
	if (node->type->endianness == BIG)
		return (int32_t)be32toh(*ret);
	if (node->type->endianness == LITTLE)
		return (int32_t)le32toh(*ret);
	if (*node->endianness == BIG)
		return (int32_t)be32toh(*ret);
	if (*node->endianness == LITTLE)
		return (int32_t)le32toh(*ret);
	return *(int32_t*)ret;
}

uint64_t formator_node_get_uint64(formator_node_t *node)
{
	uint64_t *ret = (uint64_t*)formator_node_get_data(node, sizeof(uint64_t));
	if(!ret)
		return 0;
	if (node->type->endianness == BIG)
		return be64toh(*ret);
	if (node->type->endianness == LITTLE)
		return le64toh(*ret);
	if (*node->endianness == BIG)
		return be64toh(*ret);
	if (*node->endianness == LITTLE)
		return le64toh(*ret);
	return *ret;
}

int64_t formator_node_get_int64(formator_node_t *node)
{
	uint64_t *ret = (uint64_t*)formator_node_get_data(node, sizeof(uint64_t));
	if(!ret)
		return 0;
	if (node->type->endianness == BIG)
		return (int64_t)be64toh(*ret);
	if (node->type->endianness == LITTLE)
		return (int64_t)le64toh(*ret);
	if (*node->endianness == BIG)
		return (int64_t)be64toh(*ret);
	if (*node->endianness == LITTLE)
		return (int64_t)le64toh(*ret);
	return *(int64_t*)ret;
}

uintmax_t formator_node_get_uint(formator_node_t *node)
{
	if (formator_nodetype_cmp(node->type, &FormatorNodeTypeInt8)
	 || formator_nodetype_cmp(node->type, &FormatorNodeTypeUInt8))
		return (uintmax_t)formator_node_get_uint8(node);
	else if (formator_nodetype_cmp(node->type, &FormatorNodeTypeInt16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntbe16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntle16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUInt16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntbe16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntle16))
		return (uintmax_t)formator_node_get_uint16(node);
	else if (formator_nodetype_cmp(node->type, &FormatorNodeTypeInt32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntbe32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntle32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUInt32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntbe32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntle32))
		return (uintmax_t)formator_node_get_uint32(node);
	else if (formator_nodetype_cmp(node->type, &FormatorNodeTypeInt64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntbe64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntle64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUInt64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntbe64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntle64))
		return (uintmax_t)formator_node_get_uint64(node);
	 else
		return (uintmax_t)0;
}

intmax_t formator_node_get_int(formator_node_t *node)
{
	if (formator_nodetype_cmp(node->type, &FormatorNodeTypeInt8)
	 || formator_nodetype_cmp(node->type, &FormatorNodeTypeUInt8))
		return (intmax_t)formator_node_get_int8(node);
	else if (formator_nodetype_cmp(node->type, &FormatorNodeTypeInt16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntbe16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntle16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUInt16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntbe16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntle16))
		return (intmax_t)formator_node_get_int16(node);
	else if (formator_nodetype_cmp(node->type, &FormatorNodeTypeInt32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntbe32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntle32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUInt32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntbe32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntle32))
		return (intmax_t)formator_node_get_int32(node);
	else if (formator_nodetype_cmp(node->type, &FormatorNodeTypeInt64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntbe64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntle64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUInt64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntbe64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntle64))
		return (intmax_t)formator_node_get_int64(node);
	 else
		return (intmax_t)0;
}

void formator_node_set_data(formator_node_t *node, void *data, size_t size)
{
	if(node->flags.from_file)
	{
		if(node->flags.mem_allocated && node->allocated > 0)
			free(node->data_start);
		node->data = data;
		if(node->parent)
			node->parent->total_size += size - node->size;
		node->size = size;
		node->data_start = node->data;
		if (data && size > 0)
		{
			node->flags.mem_allocated = 1;
			node->allocated = size;
		}
	}
	else {/*TODO*/}
}

void formator_node_set_string(formator_node_t *node, const char *value)
{
	int len = strlen(value);
	char *data = (char*)malloc(len+1);
	strncpy(data, value, len);
	data[len] = '\0';
	formator_node_set_data(node, data, len+1);
}

void formator_node_set_uint8(formator_node_t *node, uint8_t value)
{
	uint8_t *data = (uint8_t*)malloc(sizeof(uint8_t));
	if(!data)
		return;
	*data = value;
	formator_node_set_data(node, data, sizeof(uint8_t));
}

void formator_node_set_uint16(formator_node_t *node, uint16_t value)
{
	uint16_t *data = (uint16_t*)malloc(sizeof(uint16_t));
	if(!data)
		return;
	if (node->type->endianness == BIG)
		*data = htobe16(value);
	else if (node->type->endianness == LITTLE)
		*data = htole16(value);
	else if (*node->endianness == BIG)
		*data = htobe16(value);
	else if (*node->endianness == LITTLE)
		*data = htole16(value);
	else
		*data = value;
	formator_node_set_data(node, data, sizeof(uint16_t));
}

void formator_node_set_uint32(formator_node_t *node, uint32_t value)
{
	uint32_t *data = (uint32_t*)malloc(sizeof(uint32_t));
	if(!data)
		return;
	if (node->type->endianness == BIG)
		*data = htobe32(value);
	else if (node->type->endianness == LITTLE)
		*data = htole32(value);
	else if (*node->endianness == BIG)
		*data = htobe32(value);
	else if (*node->endianness == LITTLE)
		*data = htole32(value);
	else
		*data = value;
	formator_node_set_data(node, data, sizeof(uint32_t));
}

void formator_node_set_uint64(formator_node_t *node, uint64_t value)
{
	uint64_t *data = (uint64_t*)malloc(sizeof(uint64_t));
	if(!data)
		return;
	if (node->type->endianness == BIG)
		*data = htobe64(value);
	else if (node->type->endianness == LITTLE)
		*data = htole64(value);
	else if (*node->endianness == BIG)
		*data = htobe64(value);
	else if (*node->endianness == LITTLE)
		*data = htole64(value);
	else
		*data = value;
	formator_node_set_data(node, data, sizeof(uint64_t));
}

void formator_node_set_int8(formator_node_t *node, int8_t value)
{
	int8_t *data = (int8_t*)malloc(sizeof(int8_t));
	if(!data)
		return;
	*data = value;
	formator_node_set_data(node, data, sizeof(int8_t));
}

void formator_node_set_int16(formator_node_t *node, int16_t value)
{
	int16_t *data = (int16_t*)malloc(sizeof(int16_t));
	if(!data)
		return;
	if (node->type->endianness == BIG)
		*data = (int16_t)htobe16(value);
	else if (node->type->endianness == LITTLE)
		*data = (int16_t)htole16(value);
	else if (*node->endianness == BIG)
		*data = (int16_t)htobe16(value);
	else if (*node->endianness == LITTLE)
		*data = (int16_t)htole16(value);
	else
		*data = value;
	formator_node_set_data(node, data, sizeof(int16_t));
}

void formator_node_set_int32(formator_node_t *node, int32_t value)
{
	int32_t *data = (int32_t*)malloc(sizeof(int32_t));
	if(!data)
		return;
	if (node->type->endianness == BIG)
		*data = (int32_t)htobe32(value);
	else if (node->type->endianness == LITTLE)
		*data = (int32_t)htole32(value);
	else if (*node->endianness == BIG)
		*data = (int32_t)htobe32(value);
	else if (*node->endianness == LITTLE)
		*data = (int32_t)htole32(value);
	else
		*data = value;
	formator_node_set_data(node, data, sizeof(int32_t));
}

void formator_node_set_int64(formator_node_t *node, int64_t value)
{
	int64_t *data = (int64_t*)malloc(sizeof(int64_t));
	if(!data)
		return;
	if (node->type->endianness == BIG)
		*data = (int64_t)htobe64(value);
	else if (node->type->endianness == LITTLE)
		*data = (int64_t)htole64(value);
	else if (*node->endianness == BIG)
		*data = (int64_t)htobe64(value);
	else if (*node->endianness == LITTLE)
		*data = (int64_t)htole64(value);
	else
		*data = value;
	formator_node_set_data(node, data, sizeof(int64_t));
}

void formator_node_set_int(formator_node_t *node, off_t value)
{
	if (formator_nodetype_cmp(node->type, &FormatorNodeTypeInt8)
	 || formator_nodetype_cmp(node->type, &FormatorNodeTypeUInt8))
		formator_node_set_int8(node, value);
	else if (formator_nodetype_cmp(node->type, &FormatorNodeTypeInt16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntbe16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntle16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUInt16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntbe16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntle16))
		formator_node_set_int16(node, value);
	else if (formator_nodetype_cmp(node->type, &FormatorNodeTypeInt32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntbe32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntle32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUInt32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntbe32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntle32))
		formator_node_set_int32(node, value);
	else if (formator_nodetype_cmp(node->type, &FormatorNodeTypeInt64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntbe64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntle64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUInt64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntbe64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntle64))
		formator_node_set_int64(node, value);
}

void formator_node_set_uint(formator_node_t *node, uintmax_t value)
{
	if (formator_nodetype_cmp(node->type, &FormatorNodeTypeInt8)
	 || formator_nodetype_cmp(node->type, &FormatorNodeTypeUInt8))
		formator_node_set_uint8(node, value);
	else if (formator_nodetype_cmp(node->type, &FormatorNodeTypeInt16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntbe16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntle16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUInt16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntbe16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntle16))
		formator_node_set_uint16(node, value);
	else if (formator_nodetype_cmp(node->type, &FormatorNodeTypeInt32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntbe32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntle32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUInt32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntbe32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntle32))
		formator_node_set_uint32(node, value);
	else if (formator_nodetype_cmp(node->type, &FormatorNodeTypeInt64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntbe64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntle64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUInt64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntbe64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntle64))
		formator_node_set_uint64(node, value);
}

void formator_node_set_size (formator_node_t *node, off_t size)
{
	node->size += size - node->total_size;
	node->total_size = size;
	if(node->flags.dyn_size && node->size_node)
		formator_node_set_int(node->size_node, size);
}