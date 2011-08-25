/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * nodetype.c
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "print.h"
#include "node.h"
#include "nodetype.h"

int formator_printf_node_type_string(FILE *stream, formator_node_t *node, formator_printf_info info);
int formator_printf_node_type_int(FILE *stream, formator_node_t *node, formator_printf_info info);
int formator_printf_node_type_hex(FILE *stream, formator_node_t *node, formator_printf_info info);

formator_node_type_t FormatorNodeTypeRoot = {"root", "Root node", 1};
formator_node_type_t FormatorNodeTypeData = {"data", "Generic data node", sizeof(uint8_t)};
formator_node_type_t FormatorNodeTypeHex = {"hex", "Hexadecimal chars", sizeof(uint8_t), UNSPECIFIED, formator_printf_node_type_hex};
formator_node_type_t FormatorNodeTypeString = {"string", "String node", sizeof(char), UNSPECIFIED, formator_printf_node_type_string};

formator_node_type_t FormatorNodeTypeUInt8 = {"uint8", "Unsigned int8 node", sizeof(uint8_t), UNSPECIFIED, formator_printf_node_type_int};
formator_node_type_t FormatorNodeTypeUInt16 = {"uint16", "Unsigned int16 node", sizeof(uint16_t), UNSPECIFIED, formator_printf_node_type_int};
formator_node_type_t FormatorNodeTypeUInt32 = {"uint32", "Unsigned int32 node", sizeof(uint32_t), UNSPECIFIED, formator_printf_node_type_int};
formator_node_type_t FormatorNodeTypeUInt64 = {"uint64", "Unsigned int64 node", sizeof(uint64_t), UNSPECIFIED, formator_printf_node_type_int};

formator_node_type_t FormatorNodeTypeInt8 = {"int8", "Signed int8 node", sizeof(int8_t), UNSPECIFIED, formator_printf_node_type_int};
formator_node_type_t FormatorNodeTypeInt16 = {"int16", "Signed int16 node", sizeof(int16_t), UNSPECIFIED, formator_printf_node_type_int};
formator_node_type_t FormatorNodeTypeInt32 = {"int32", "Signed int32 node", sizeof(int32_t), UNSPECIFIED, formator_printf_node_type_int};
formator_node_type_t FormatorNodeTypeInt64 = {"int64", "Signed int64 node", sizeof(int64_t), UNSPECIFIED, formator_printf_node_type_int};

formator_node_type_t FormatorNodeTypeUIntbe16 = {"uintbe16", "Big endian unsigned int16 node", sizeof(uint16_t), BIG, formator_printf_node_type_int};
formator_node_type_t FormatorNodeTypeUIntbe32 = {"uintbe32", "Big endian unsigned int32 node", sizeof(uint32_t), BIG, formator_printf_node_type_int};
formator_node_type_t FormatorNodeTypeUIntbe64 = {"uintbe64", "Big endian unsigned int64 node", sizeof(uint64_t), BIG, formator_printf_node_type_int};

formator_node_type_t FormatorNodeTypeIntbe16 = {"intbe16", "Big endian signed int16 node", sizeof(int16_t), BIG, formator_printf_node_type_int};
formator_node_type_t FormatorNodeTypeIntbe32 = {"intbe32", "Big endian signed int32 node", sizeof(int32_t), BIG, formator_printf_node_type_int};
formator_node_type_t FormatorNodeTypeIntbe64 = {"intbe64", "Big endian signed int64 node", sizeof(int64_t), BIG, formator_printf_node_type_int};

formator_node_type_t FormatorNodeTypeUIntle16 = {"uintle16", "Little endian unsigned int16 node", sizeof(uint16_t), LITTLE, formator_printf_node_type_int};
formator_node_type_t FormatorNodeTypeUIntle32 = {"uintle32", "Little endian unsigned int32 node", sizeof(uint32_t), LITTLE, formator_printf_node_type_int};
formator_node_type_t FormatorNodeTypeUIntle64 = {"uintle64", "Little endian unsigned int64 node", sizeof(uint64_t), LITTLE, formator_printf_node_type_int};

formator_node_type_t FormatorNodeTypeIntle16 = {"intle16", "Little endian signed int16 node", sizeof(int16_t), LITTLE, formator_printf_node_type_int};
formator_node_type_t FormatorNodeTypeIntle32 = {"intle32", "Little endian signed int32 node", sizeof(int32_t), LITTLE, formator_printf_node_type_int};
formator_node_type_t FormatorNodeTypeIntle64 = {"intle64", "Little endian signed int64 node", sizeof(int64_t), LITTLE, formator_printf_node_type_int};

int formator_nodetype_cmp(formator_node_type_t *type1, formator_node_type_t *type2)
{
	if(type1 == type2 || (strcmp(type1->name, type2->name) == 0 && type1->blocksize == type2->blocksize))
		return 1;
	return 0;
}

int formator_printf_node_type_string(FILE *stream, formator_node_t *node, formator_printf_info info)
{
	if(!node)
		return -1;
	if (formator_nodetype_cmp(node->type, &FormatorNodeTypeString))
	{
		char *str = (char*)malloc(node->size+1);
		if(!str)
			return -1;
		strncpy (str, formator_node_get_string(node), node->size);
		str[node->size] = '\0';
		int done = formator_print_string (stream, str, info);
		free(str);
		return done;
	}
	return -1;
}

int formator_printf_node_type_int(FILE *stream, formator_node_t *node, formator_printf_info info)
{
	intmax_t i;
	int sign;
	if (!node)
		return -1;
	if (formator_nodetype_cmp(node->type, &FormatorNodeTypeInt8))
	{
		i = formator_node_get_int8(node);
		sign = 1;
	}
	else if (formator_nodetype_cmp(node->type, &FormatorNodeTypeUInt8))
	{
		i = formator_node_get_uint8(node);
		sign = 0;
	}
	else if (formator_nodetype_cmp(node->type, &FormatorNodeTypeInt16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntbe16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntle16))
	{
		i = formator_node_get_int16(node);
		sign = 1;
	}
	else if (formator_nodetype_cmp(node->type, &FormatorNodeTypeUInt16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntbe16)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntle16))
	{
		i = formator_node_get_uint16(node);
		sign = 0;
	}
	else if (formator_nodetype_cmp(node->type, &FormatorNodeTypeInt32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntbe32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntle32))
	{
		i = formator_node_get_int32(node);
		sign = 1;
	}
	else if (formator_nodetype_cmp(node->type, &FormatorNodeTypeUInt32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntbe32)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntle32))
	{
		i = formator_node_get_uint32(node);
		sign = 0;
	}
	else if (formator_nodetype_cmp(node->type, &FormatorNodeTypeInt64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntbe64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeIntle64))
	{
		i = formator_node_get_int64(node);
		sign = 1;
	}
	else if (formator_nodetype_cmp(node->type, &FormatorNodeTypeUInt64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntbe64)
		  || formator_nodetype_cmp(node->type, &FormatorNodeTypeUIntle64))
	{
		i = formator_node_get_uint64(node);
		sign = 0;
	}
	else
		return -1;
	return formator_print_int (stream, i, sign, 10, 0, info);
}

int formator_printf_node_type_hex(FILE *stream, formator_node_t *node, formator_printf_info info)
{
	if(!node || !formator_nodetype_cmp(node->type, &FormatorNodeTypeHex))
		return -1;
	void *data = formator_node_get (node);
	if(!data)
		return -1;
	if(info.alt)
	{  /* 'hexdump -Cv'-like output : 
		* 00000000  01 02 03 04 05 06 07 08  09 0a 0b 0c 0d 0e 0f 10  |................| */
		char ascii[20];
		off_t i,j,len,total=0;
		
		memset(&ascii, 0, sizeof(ascii));
		memset(&info, 0, sizeof(formator_printf_info));
		info.prec=-1;
		info.pad=' ';
		info.left=1;

		len = formator_print_string(stream, "Hexdump:\n", info); /*go to new line so all line will be aligned*/
		if(len == -1) return -1;
		total += len;
		
		info.prec=8;
		info.width=10;
		formator_print_int (stream, 0, 0, 16, 'a', info);

		for (i = 0, j = 0; i < node->size; i++, j++)
		{
			if (j == 16)
			{
				ascii[0] = '|';
				ascii[j+1] = '|';
				ascii[j+2] = '\n';
				ascii[j+3] = '\0';
				info.prec=-1;
				info.width=0;
				len = formator_print_string(stream, ascii, info);
				if(len == -1) return -1;
				total += len;

				info.prec=8;
				info.width=10;
				formator_print_int (stream, i, 0, 16, 'a', info);
				j=0;
			}

			info.prec=2;
			info.width= ((j%8)==7?4:3);
			formator_print_int (stream, 0xff & *(uint8_t*)(data+i), 0, 16, 'a', info);
			if ((*(uint8_t*)(data+i) > 31) && (*(uint8_t*)(data+i) < 127))
				ascii[j+1] = *(uint8_t*)(data+i);
			else
				ascii[j+1] = '.';
		}
		if (j != 0)
		{
			ascii[0] = '|';
			ascii[j+1] = '|';
			ascii[j+2] = '\n';
			ascii[j+3] = '\0';

			info.left=0;
			info.prec=-1;
			
			info.width=50-(j*3+(j/8))+strlen(ascii);/* what's wrong with that? */
			len = formator_print_string(stream, ascii, info);
			if(len == -1) return -1;
			total += len;
		}
		info.prec=8;
		info.width=10;
		info.left=1;
		len = formator_print_int (stream, i, 0, 16, 'a', info); /* no \n here, user should use '%#N\n' */
		if(len == -1) return -1;
		return total + len;
	}
	else
	{
		memset(&info, 0, sizeof(formator_printf_info));
		info.prec=2;
		info.width=3;
		info.pad=' ';
		info.left=1;
		for(int i=0; i < node->size; ++i)
			if (formator_print_int (stream, 0xff & *(uint8_t*)(data+i), 0, 16, 'a', info) == -1)
				return -1;
		return node->size*3;
	}
}