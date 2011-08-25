
AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\"

AM_CFLAGS =\
	 -Wall\
	 -g \
	 -std=gnu99 \
	 -DFORMATOR_LIBRARY \
	 -D_GNU_SOURCE \
	 -D_FILE_OFFSET_BITS=64

bin_PROGRAMS =  \
	png_test

png_test_SOURCES =  \
	png_test.c

png_test_LDADD = -lformator

lib_LTLIBRARIES = libformator.la


libformator_la_SOURCES = \
	node.c \
	nodetype.c \
	formator.c \
	print.c \
	nodeiterator.c \
	fmemopen.c


libformator_includedir=$(includedir)/formator

libformator_include_HEADERS = \
	formator.h \
	node.h \
	nodetype.h \
	print.h \
	nodeiterator.h \
	fmemopen.h


pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libformator.pc

EXTRA_DIST = \
	libformator.pc.in
