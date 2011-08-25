/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * print.c
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
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include "node.h"
#include "print.h"

#define MAX(a,b) ((a < b) ?  (b) : (a))
#define MIN(a,b) ((a > b) ?  (b) : (a))

int formator_printf(const char *format, ...)
{
	va_list arg;
	int done;

	va_start (arg, format);
	done = formator_vfprintf (stdout, format, arg);
	va_end (arg);

	return done;
}

int formator_sprintf(char *str, const char *format, ...)
{
	va_list arg;
	int done;

	va_start (arg, format);
	done = formator_vsprintf (str, format, arg);
	va_end (arg);

	return done;
}

int formator_snprintf(char *str, size_t size, const char *format, ...)
{
	va_list arg;
	int done;

	va_start (arg, format);
	done = formator_vsnprintf (str, size, format, arg);
	va_end (arg);

	return done;
}

int formator_asprintf(char **strp, const char *format, ...)
{
	va_list arg;
	int done;

	va_start (arg, format);
	done = formator_vasprintf (strp, format, arg);
	va_end (arg);

	return done;
}

int formator_fprintf(FILE *stream, const char *format, ...)
{
	va_list arg;
	int done;

	va_start (arg, format);
	done = formator_vfprintf (stream, format, arg);
	va_end (arg);

	return done;
}


int formator_vprintf(const char *format, va_list arg)
{
	return formator_vfprintf (stdout, format, arg);
}

int formator_vsprintf(char *str, const char *format, va_list arg)
{
	return formator_vsnprintf(str, SIZE_MAX, format, arg);
}

int formator_vsnprintf(char *str, size_t size, const char *format, va_list arg)
{
	int done;
	FILE *stream = NULL;

	stream = formator_fmemopen (str, size, "rb+");
	if(!stream)
		return -1;
	done = formator_vfprintf (stream, format, arg);
	fclose(stream);

	return done;
}

int formator_vasprintf(char **strp, const char *format, va_list arg)
{
	size_t strlen;
	int done;
	FILE *stream = NULL;

	stream = formator_open_memstream(strp, &strlen);
	if(!stream)
		return -1;
	done = formator_vfprintf (stream, format, arg);
	fclose(stream);

	return done;
}

int formator_print_string(FILE *stream, const char* string, formator_printf_info info)
{
	if(!string)
	{
		if(info.prec==-1 || info.prec >= 6)
			string = "(null)";
		else
			string = "";
	}
	int done = 0;
	int len = strlen(string);
	if(info.prec == -1)
		info.prec = len;
	int max = MAX(info.width,info.prec);
	int min = MIN(len, info.prec);
	char *buf = (char*)malloc(max+1);
	if (!buf)
		return -1;
	memset(buf, info.pad, max);
	buf[max] = '\0';
	char *s;
	if (info.left)
		s = buf;
	else
		s = buf+max-min;
	strncpy (s, string, min); 
	done = fputs(buf, stream);
	free(buf);
	if(done == EOF)
		return -1;
	return max;
}

int formator_print_char (FILE *stream, const char c, formator_printf_info info)
{
	if(info.prec < 1)
		return formator_print_string(stream, &c, info);
	char str[info.prec+1];
	memset (str, c, info.prec);
	str[info.prec] = '\0';
	return formator_print_string(stream, str, info);
}

int formator_print_int(FILE *stream, intmax_t i, int sign, int base, int basechar, formator_printf_info info)
{
	/* max converted length = 64 (int 64, base 2) + 2 sign + \0 */
	char *buf;
	int maxlen = 67;
	int start = maxlen;
	char *s, *s2;
	char sg = '\0';
	int t, done, len, neg = 0;
	uintmax_t u = i;

	if (info.left)
	{
		maxlen = 2*maxlen-5;/*to be able to add pad char at the end of the string*/
		if (info.width*2+1 > maxlen)
		{
			maxlen = info.width*2+1;
			start = info.width+1;
		}
	}
	else if(info.width+1 > maxlen)
		maxlen = info.width*2+1;
	if (info.prec+3 > maxlen)
		maxlen = info.prec + 3;/* 2 sign + \0 */

	buf = (char*)malloc(maxlen);
	if(!buf)
		return -1;

	s = buf + start;
	*s = '\0';

	if (sign && base == 10 && i < 0)
	{
		neg = 1;
		u = -i;
	}

	if (sign)
	{
		if (neg)
			sg = '-';
		else if (info.showsign)
			sg = '+';
		else if (info.space)
			sg = ' ';
	}
	if (info.alt && base == 16)
	{
		sg = basechar+23;/* x or X */
	}

	if(i==0)
		*--s = '0';

	while (u) {
		t = u % base;
		if( t >= 10 )
			t += basechar - '0' - 10;
		*--s = t + '0';
		u /= base;
	}

	len = strlen(s);
	while (info.prec > len)
	{
		*--s = '0'; 
		++len;
	}

	if(info.alt && base == 8 && *s != '0')
		*--s = '0';

	if (sg)
	{
		if (info.pad == ' ')
		{
			*--s = sg;
			if(sg == basechar+23)/* x or X */
				*--s = '0';
		}
		else
		{
			--info.width;
			if(sg == basechar+23)/* x or X */
				--info.width;
		}
	}

	len = strlen(s);
	if (info.left)
	{
		s2 = buf + start -1;
		while (info.width > len)
		{
			*++s2 = info.pad; 
			++len;
		}
		*++s2 = '\0';
	}
	else
		while (info.width > len)
		{
			*--s = info.pad; 
			++len;
		}

	if (sg && info.pad != ' ')
	{
		*--s = sg;
		if(sg == basechar+23) /* x or X */
			*--s = '0';
	}
	done = fputs (s, stream);
	if(done == EOF)
		done = -1;
	else
		done = strlen(s);
	free(buf);
	return done;
}

int formator_print_node (FILE *stream, formator_node_t *node, formator_printf_info info)
{
	if(!node || !node->type)
		return 0;
	if(node->type->printf)
	{
		int done = node->type->printf(stream, node, info);
		if (done > 0)
			return done;
	}
	return formator_fprintf (stream, "<%s %p %ju/%ju>", node->type->name, node->data, node->size, node->total_size);
}

int formator_vfprintf(FILE *stream, const char *format, va_list arg)
{
	int count = 0, done = 0;
	const char *p, *f;
	void *ptr;
	formator_printf_info info;

	for (p = format; *p; p++)
	{
		if (*p != '%')
		{
print:
			fputc (*p, stream);
			count++;
			continue;
		}
		f = p;
		p++;
		if (*p == '\0')
			break;
		if (*p == '%')
			goto print;
		memset(&info, 0, sizeof(formator_printf_info));
		info.prec=-1;
		info.pad=' ';
		while (*p == '0' || *p == '-' || *p == '+' || *p == ' ' || *p == '#' || *p == '\'')
		{
			switch(*p)
			{
				case '0': info.pad='0';break;
				case '-': info.left=1;break;
				case '+': info.showsign=1;break;
				case ' ': info.space=1;break;
				case '#': info.alt=1;break;
				case '\'': info.group=1;break;
			}
			p++;
		}
		if (info.left) info.pad=' ';
		/*if (info.showsign) info.space=0;*/
		if (*p == '*')
		{
			p++;
			int j = va_arg(arg, int);
			if (j >= 0)
				info.width = j;
			else
			{
				info.width = -j;
				info.left = 1;
			}
		}
		else if (*p>='0' && *p<='9')
		{
			unsigned int uj = *p++ - '0';
			while (*p>='0' && *p<='9')
				uj = 10*uj + (unsigned int)(*p++ - '0');
			info.width = uj;
		}
		if (*p == '.')
		{
			p++;
			if (*p == '*')
			{
				p++;
				int j = va_arg(arg, int);
				if (j >= 0)
					info.prec = j;
				else
					info.prec = -1;
			}
			else if (*p>='0' && *p<='9')
			{
				unsigned int uj = *p++ - '0';
				while (*p>='0' && *p<='9')
					uj = 10*uj + (unsigned int)(*p++ - '0');
				info.prec = uj;
			}
		}
		switch(*p)
		{
			case 'h':
				p++;
				if(*p == 'h')
				{
					info.is_char = 1;
					p++;
				}
				else
					info.is_short = 1;
				break;
			case 'l':
				p++;
				if(*p == 'l')
				{
					info.is_long_double = 1;
					p++;
				}
				else
					info.is_long = 1;
				break;
			case 'L':
			case 'q':
				p++;
				info.is_long_double = 1;
				break;
			case 'j':
				p++;
				info.is_max = 1;
				break;
			case 'z':
				p++;
				info.is_size = 1;
				break;
			case 't':
				p++;
				info.is_ptrdiff = 1;
				break;
		}
		info.spec = *p;
		switch (info.spec) {
			case 'i':
			case 'd':
				if (info.is_char)
					done = formator_print_int (stream, (int8_t)va_arg(arg, int), 1, 10, 0, info);
				else if (info.is_short)
					done = formator_print_int (stream, (int16_t)va_arg(arg, int), 1, 10, 0, info);
				else if (info.is_long)
					done = formator_print_int (stream, va_arg(arg, int32_t), 1, 10, 0, info);
				else if (info.is_long_double)
					done = formator_print_int (stream, va_arg(arg, int64_t), 1, 10, 0, info);
				else if (info.is_size)
					done = formator_print_int (stream, va_arg(arg, size_t), 0, 10, 0, info);
				else if (info.is_max)
					done = formator_print_int (stream, va_arg(arg, intmax_t), 1, 10, 0, info);
				else if (info.is_ptrdiff)
					done = formator_print_int (stream, va_arg(arg, ptrdiff_t), 0, 10, 0, info);
				else
					done = formator_print_int (stream, va_arg(arg, int), 1, 10, 0, info);
				if (done < 0)
					return -1;
				count += done;
				continue;
			case 'u':
				if (info.is_char)
					done = formator_print_int (stream, (uint8_t)va_arg(arg, int), 0, 10, 0, info);
				else if (info.is_short)
					done = formator_print_int (stream, (uint16_t)va_arg(arg, int), 0, 10, 0, info);
				else if (info.is_long)
					done = formator_print_int (stream, va_arg(arg, uint32_t), 0, 10, 0, info);
				else if (info.is_long_double)
					done = formator_print_int (stream, va_arg(arg, uint64_t), 0, 10, 0, info);
				else if (info.is_size)
					done = formator_print_int (stream, va_arg(arg, size_t), 0, 10, 0, info);
				else if (info.is_max)
					done = formator_print_int (stream, va_arg(arg, uintmax_t), 0, 10, 0, info);
				else if (info.is_ptrdiff)
					done = formator_print_int (stream, va_arg(arg, ptrdiff_t), 0, 10, 0, info);
				else
					done = formator_print_int (stream, va_arg(arg, unsigned int), 0, 10, 0, info);
				if (done < 0)
					return -1;
				count += done;
				continue;
			case 'x':
			case 'X':
				if (info.is_char)
					done = formator_print_int (stream, (uint8_t)va_arg(arg, int), 0, 16, info.spec-23, info);
				else if (info.is_short)
					done = formator_print_int (stream, (uint8_t)va_arg(arg, int), 0, 16, info.spec-23, info);
				else if (info.is_long)
					done = formator_print_int (stream, va_arg(arg, uint32_t), 0, 16, info.spec-23, info);
				else if (info.is_long_double)
					done = formator_print_int (stream, va_arg(arg, uint64_t), 0, 16, info.spec-23, info);
				else if (info.is_size)
					done = formator_print_int (stream, va_arg(arg, size_t), 0, 16, info.spec-23, info);
				else if (info.is_max)
					done = formator_print_int (stream, va_arg(arg, uintmax_t), 0, 16, info.spec-23, info);
				else if (info.is_ptrdiff)
					done = formator_print_int (stream, va_arg(arg, ptrdiff_t), 0, 16, info.spec-23, info);
				else
					done = formator_print_int (stream, va_arg(arg, unsigned int), 0, 16, info.spec-23, info);
				if (done < 0)
					return -1;
				count += done;
				continue;
			case 'o':
				if (info.is_char)
					done = formator_print_int (stream, (uint8_t)va_arg(arg, int), 0, 8, 0, info);
				else if (info.is_short)
					done = formator_print_int (stream, (uint8_t)va_arg(arg, int), 0, 8, 0, info);
				else if (info.is_long)
					done = formator_print_int (stream, va_arg(arg, uint32_t), 0, 8, 0, info);
				else if (info.is_long_double)
					done = formator_print_int (stream, va_arg(arg, uint64_t), 0, 8, 0, info);
				else if (info.is_size)
					done = formator_print_int (stream, va_arg(arg, size_t), 0, 8, 0, info);
				else if (info.is_max)
					done = formator_print_int (stream, va_arg(arg, uintmax_t), 0, 8, 0, info);
				else if (info.is_ptrdiff)
					done = formator_print_int (stream, va_arg(arg, ptrdiff_t), 0, 8, 0, info);
				else
					done = formator_print_int (stream, va_arg(arg, unsigned int), 0, 8, 0, info);
				if (done < 0)
					return -1;
				count += done;
				continue;
			case 'b':
				if (info.is_char)
					done = formator_print_int (stream, (uint8_t)va_arg(arg, int), 0, 2, 0, info);
				else if (info.is_short)
					done = formator_print_int (stream, (uint8_t)va_arg(arg, int), 0, 2, 0, info);
				else if (info.is_long)
					done = formator_print_int (stream, va_arg(arg, uint32_t), 0, 2, 0, info);
				else if (info.is_long_double)
					done = formator_print_int (stream, va_arg(arg, uint64_t), 0, 2, 0, info);
				else if (info.is_size)
					done = formator_print_int (stream, va_arg(arg, size_t), 0, 2, 0, info);
				else if (info.is_max)
					done = formator_print_int (stream, va_arg(arg, uintmax_t), 0, 2, 0, info);
				else if (info.is_ptrdiff)
					done = formator_print_int (stream, va_arg(arg, ptrdiff_t), 0, 2, 0, info);
				else
					done = formator_print_int (stream, va_arg(arg, unsigned int), 0, 2, 0, info);
				if (done < 0)
					return -1;
				count += done;
				continue;
			case 'c':
				done = formator_print_char (stream, va_arg(arg, int), info);
				if (done < 0)
					return -1;
				count += done;
				continue;
			case 's':
				done = formator_print_string (stream, va_arg(arg, char*), info);
				if (done < 0)
					return -1;
				count += done;
				continue;
			case 'p':
				info.alt = 1;
				ptr = va_arg(arg, void*);
				if(ptr)
					done = formator_print_int (stream, (uintptr_t)ptr, 0, 16, 'a', info);
				else if (info.prec==-1 || info.prec >= 5)
					done = formator_print_string (stream, "(nil)", info);
				else
					done = formator_print_string (stream, "", info);
				if (done < 0)
					return -1;
				count += done;
				continue;
			case 'n':
				ptr = (void*)va_arg(arg, int*);
				if(ptr)
					*((int*)ptr) = count;
				continue;
			case 'm':
				done = formator_print_string (stream, strerror(errno), info);
				if (done < 0)
					return -1;
				count += done;
				continue;
			case 'N': /* here is the only reason why all thoses functions exists :p */
				done = formator_print_node (stream, va_arg(arg, formator_node_t*), info);
				if (done < 0)
					return -1;
				count += done;
				continue;
			default:
				va_arg(arg, void);
				p=f;
				goto print;
		}
	}
	return count;
}