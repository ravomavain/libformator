/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * png_test.c
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
#include <errno.h>

#if defined FORMATOR_LIBRARY
#  include "formator.h"
#else
#  include <formator/formator.h>
#endif

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		printf("Usage: %s <file>\n", argv[0]);
		return 1;
	}
	formator_t *formator = formator_open_file (argv[1], BIG);
	if(!formator)
	{
		return 1;
	}
	formator_node_t *header = formator_new_node (&FormatorNodeTypeData, 8, formator->tree, "PNG header");
	formator_new_node (&FormatorNodeTypeHex, 1, header, "First byte PNG header");
	formator_new_node (&FormatorNodeTypeString, 3, header, "Ascii letters 'PNG'");
	formator_new_node (&FormatorNodeTypeHex, 2, header, "CRLF chars");
	formator_new_node (&FormatorNodeTypeHex, 1, header, "EOF char");
	formator_new_node (&FormatorNodeTypeHex, 1, header, "LF char");
	formator_node_t *chunks = formator_new_dyn_node (&FormatorNodeTypeData, 0x0, formator->tree, "PNG Chunks");
	while(chunks && chunks->size > 4)
	{
		formator_node_t *chunk = formator_new_dyn_node (&FormatorNodeTypeData, 0x0, chunks, "PNG chunk");
		formator_node_t *chunk_size = formator_new_node (&FormatorNodeTypeUInt32, 1, chunk, "Chunk length");
		formator_new_node (&FormatorNodeTypeString, 4, chunk, "Chunk type");
		formator_new_dyn_node (&FormatorNodeTypeHex, chunk_size, chunk, "Chunk data");
		formator_new_node (&FormatorNodeTypeHex, 4, chunk, "Chunk CRC");
	}

	formator_node_iterator_t *iter = formator_create_iterator(formator->tree, PARENT_FIRST);

	do
	{
		formator_printf("%*s-> %s(%ju) -> %s\n%*s    %#N\n", iter->node->depth, "", iter->node->type->name, iter->node->size, (char*)iter->node->extra, iter->node->depth, "", iter->node);
	} while (iter->next(iter));

	return 0;
}
