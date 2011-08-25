/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * print.h
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

/* Support :
 * d,i,u,x,X,o,b : hh,h,l,ll, width, prec, *, -, ' ', +, # (b is for binary)
 * s,c : width, prec (for c, the char is repeated prec times if prec is spécified)
 * p : print pointer adresse (like %#x)
 * n : store number of printed char to int* passed as arg.
 * m : print strerror(errno) (from gnu implementation)
 * TODO: double & float : a, A, e, E, f, F, g, G
 *		 wide chars : ls/S, lc/C
 */

#ifndef FORMATOR_PRINT_H
#define FORMATOR_PRINT_H

#if defined FORMATOR_LIBRARY
#  include "fmemopen.h"
#  include "node.h"
#else
#  include <formator/fmemopen.h>
#  include <formator/node.h>
#endif

typedef struct formator_printf_info
{
	int prec;						/* Precision.  */
	int width;						/* Width.  */
	char spec;						/* Format letter.  */
	unsigned int is_char:1;			/* hh flag.  */
	unsigned int is_short:1;		/* h flag.  */
	unsigned int is_long:1;			/* l flag.  */
	unsigned int is_long_double:1;	/* ll flag.  */
	unsigned int is_max:1;			/* j flag.  */
	unsigned int is_size:1;			/* z flag.  */
	unsigned int is_ptrdiff:1;		/* t flag.  */
	unsigned int alt:1;				/* # flag.  */
	unsigned int space:1;			/* Space flag.  */
	unsigned int left:1;			/* - flag.  */
	unsigned int showsign:1;		/* + flag.  */
	unsigned int group:1;			/* ' flag.  */
	char pad;						/* Padding character.  */
} formator_printf_info;

typedef int (*formator_node_printf)(FILE *stream, formator_node_t *node, formator_printf_info info);

int formator_printf(const char *format, ...);
int formator_sprintf(char *str, const char *format, ...);
int formator_snprintf(char *str, size_t size, const char *format, ...);
int formator_asprintf(char **strp, const char *fmt, ...);
int formator_fprintf(FILE *stream, const char *format, ...);

int formator_vprintf(const char *format, va_list ap);
int formator_vsprintf(char *str, const char *format, va_list ap);
int formator_vsnprintf(char *str, size_t size, const char *format, va_list ap);
int formator_vasprintf(char **strp, const char *fmt, va_list ap);
int formator_vfprintf(FILE *stream, const char *format, va_list ap);

int formator_print_string (FILE *stream, const char* string, formator_printf_info info);
int formator_print_char (FILE *stream, const char c, formator_printf_info info);
int formator_print_int (FILE *stream, intmax_t i, int sign, int base, int basechar, formator_printf_info info);
int formator_print_node (FILE *stream, formator_node_t *node, formator_printf_info info);

#endif /*FORMATOR_PRINT_H*/