# $Id: Makefile,v 1.3 2001/01/28 04:12:45 cvsuser Exp $
#
# 2000, unicorn

LIB= rdso
SHLIB_MAJOR=    1
SHLIB_MINOR=    0

NOPROFILE = oops;

TOPDIR = $(.CURDIR)/../..

DEBUG_FLAGS= -g
STRIP=
CFLAGS = -I$(TOPDIR)/mud -I$(TOPDIR)/src/include

SRCS = cmd.c mod.c rpc.c special.c

LIBDIR = $(TOPDIR)/lib

beforeinstall: $(LIBDIR)

$(LIBDIR):
	mkdir $@

.include <bsd.lib.mk>
