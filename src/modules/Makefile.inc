# $Id: Makefile.inc,v 1.3 2001/01/28 19:33:00 cvsuser Exp $
#
# 2000, unicorn


LIB = MODULE
LIBMODE = 755
SHLIB_NAME = $(MODULE).so

TOPDIR = $(.CURDIR)/../../..
DEBUG_FLAGS = -g
STRIP=
CFLAGS = -I$(TOPDIR)/mud -I$(TOPDIR)/src/include -I.

NOPROFILE = yes
INTERNALLIB = sure

LIBDIR = $(TOPDIR)/libexec

beforeinstall: $(LIBDIR)

$(LIBDIR):
	mkdir $@

.include <bsd.lib.mk>

