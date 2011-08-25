/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * formator.c
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
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "formator.h"

formator_t *formator_open_data(void *data, off_t size, Endianness endianness)
{
	formator_t *formator = (formator_t*)malloc(sizeof(formator_t));
	if(!formator)
		return NULL;
	memset(formator, 0, sizeof(formator_t));
	formator->data = data;
	formator->location = LOC_DATA;
	formator->size = size;
	formator->endianness = endianness;
	formator->tree = formator_create_root(data, size, &formator->endianness);
	if(!formator->tree)
		return NULL;
	return formator;
}

formator_t *formator_open_file(const char *filename, Endianness endianness)
{
	FILE* file;
	off_t size;
	formator_t *formator;

	file = fopen(filename, "rb");
	if (file)
	{
		fseeko (file, 0, SEEK_END);
		size = ftello (file);
		rewind (file);
		if(!errno && size >= 0)
		{
			formator = (formator_t*)malloc(sizeof(formator_t));
			if(formator)
			{
				memset(formator, 0, sizeof(formator_t));
				formator->file = file;
				formator->data = NULL;
				formator->location = LOC_FILE;
				formator->size = size;
				formator->endianness = endianness;
				formator->tree = formator_create_root_file(file, size, &formator->endianness);
				if(formator->tree)
					return formator;
				free(formator);
			}
		}
		fclose(file);
	}
	perror("Error opening file");
	return NULL;
}
