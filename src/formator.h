/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * lib.h
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

#ifndef FORMATOR_FORMATOR_H
#define FORMATOR_FORMATOR_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined FORMATOR_LIBRARY
#  include "nodetype.h"
#  include "node.h"
#  include "print.h"
#  include "nodeiterator.h"
#else
#  include <formator/nodetype.h>
#  include <formator/node.h>
#  include <formator/print.h>
#  include <formator/nodeiterator.h>
#endif

typedef struct formator
{
	void *data;
	FILE *file;
	off_t size;
	formator_node_t *tree;
	Endianness endianness;
	enum {LOC_FILE,LOC_DATA} location;
} formator_t;

formator_t *formator_open_data(void *data, off_t size, Endianness endianness);
formator_t *formator_open_fd(int fd, off_t size, Endianness endianness);
formator_t *formator_open_file(const char* filename, Endianness endianness);

#ifdef __cplusplus
}
#endif

#endif /*FORMATOR_FORMATOR_H*/