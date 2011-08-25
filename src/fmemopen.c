/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * fmemopen.c
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

#include "fmemopen.h"

#ifdef BSD

#define BUF_SIZE 128

static fpos_t formator_fmo_seek(void *cookie, fpos_t offset, int whence)
{
	size_t pos = 0;
	fmem_t *mem = cookie;

	switch (whence) {
		case SEEK_SET:
			pos = offset
			break;
		case SEEK_CUR:
			pos = mem->pos + offset;
			break;
		case SEEK_END:
			pos = mem->size + offset;
			break;
	}
	if (pos >= 0 && pos <= mem->size)
		return mem->pos = pos;
	return -1;
}

static fpos_t formator_oms_seek(void *cookie, fpos_t offset, int whence)
{
	size_t pos = 0;
	fmem_t *mem = cookie;

	switch (whence) {
		case SEEK_SET:
			pos = offset
			break;
		case SEEK_CUR:
			pos = mem->pos + offset;
			break;
		case SEEK_END:
			pos = mem->end + offset;
			break;
	}
	if (pos < 0)
		return -1;
	if (pos > mem->size)
	{
		size_t newsize = (pos/BUF_SIZE+1)*BUF_SIZE;
		void *temp = realloc (mem->buf, newsize);
		if(!temp)
			return -1;
		mem->buf = temp;
		memset(buf+mem->size, 0, newsize-mem->size);
		mem->size = newsize;
	}
	mem->pos = pos;
	if (mem->pos > mem->end)
		mem->end = mem->pos;
	return -1;
}

static int formator_fmo_read(void *cookie, char *buf, int size)
{
	size_t count = 0;
	fmem_t *mem = cookie;
	size_t available = mem->size - mem->pos;

	if (size < 0)
		return -1;

	if ((size_t)size > available)
		size = available;

	while (count < (size_t)size)
		buf[count++] = mem->buffer[mem->pos++];

	return count;
}

static int formator_oms_read(void *cookie, char *buf, int size)
{
	size_t count = 0;
	fmem_t *mem = cookie;
	size_t available = mem->end - mem->pos;

	if (size < 0)
		return -1;

	if ((size_t)size > available)
		size = available;

	while (count < (size_t)size)
		buf[count++] = mem->buffer[mem->pos++];

	return count;
}

static int formator_fmo_write(void *cookie, const char *buf, int size)
{
	size_t count = 0;
	fmem_t *mem = cookie;
	size_t available = mem->size - mem->pos;

	if (size < 0)
		return - 1;

	if ((size_t)size > available)
		size = available;

	while (count < (size_t)size)
		mem->buffer[mem->pos++] = buf[count++];

	return count;
}

static int formator_oms_write(void *cookie, const char *buf, int size)
{
	size_t count = 0;
	fmem_t *mem = cookie;
	size_t available = mem->end - mem->pos;

	if (size < 0)
		return - 1;

	if ((size_t)size > available)
	{
		size_t newsize = ((mem->pos+size)/BUF_SIZE+1)*BUF_SIZE;
		void *temp = realloc (mem->buf, newsize);
		if(!temp)
			size = available;
		else
		{
			mem->buf = temp;
			memset(buf+mem->size, 0, newsize-mem->size);
			mem->size = newsize;
		}
	}

	while (count < (size_t)size)
		mem->buffer[mem->pos++] = buf[count++];

	if (mem->pos > mem->end)
		mem->end = mem->pos;

	return count;
}

static int formator_fmo_close(void *cookie)
{
	free (cookie);/* :D */
	return 0;
}

static int formator_fmo_close_free(void *cookie)
{
	free (mem->buf);
	free (cookie);
	return 0;
}

static int formator_oms_close(void *cookie)
{
	void *temp = realloc (mem->buf, mem->end+1);
	if(temp)
	{
		mem->buf = temp;
		mem->size = mem->end+1;
	}
	*(mem->ptr) = mem->buf;
	*(mem->sizeloc) = mem->end;/*returned size doesn't include last \0*/
	free (cookie);
	return 0;
}

FILE *formator_fmemopen(void *buf, size_t size, const char *mode)
{
	fmem_t *mem = (fmem_t *) malloc(sizeof(fmem_t));
	if(!mem)
		return NULL;
	memset(mem, 0, sizeof(fmem_t));
	mem->size = size;
	if(buf)
	{
		mem->buffer = buf;
		return funopen(mem, formator_fmo_read, formator_fmo_write, formator_fmo_seek, formator_fmo_close);
	}
	else
	{
		mem->buf = (char*)malloc(size);
		if(!mem->buf)
			return -1;
		return funopen(mem, formator_fmo_read, formator_fmo_write, formator_fmo_seek, formator_fmo_close_free);
	}
}

FILE *formator_open_memstream(char **ptr, size_t *sizeloc);
{
	fmem_t *mem = (fmem_t *) malloc(sizeof(fmem_t));
	if(!mem)
		return NULL;
	memset(mem, 0, sizeof(fmem_t));
	mem->ptr = ptr;
	mem->sizeloc = sizeloc;
	mem->buf = (char*)malloc(BUF_SIZE);
	if(!mem->buf)
		return -1;
	*(mem->ptr) = mem->buf;
	*(mem->sizeloc) = 0;
	mem->size = BUF_SIZE;
	mem->end = 0;
	return funopen(mem, formator_osm_read, formator_osm_write, formator_osm_seek, formator_osm_close);
}

#endif /* BSD */
