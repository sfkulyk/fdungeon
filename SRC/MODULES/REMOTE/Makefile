# $Id: Makefile,v 1.1 2001/01/28 19:33:01 cvsuser Exp $
#
# 2000, unicorn

MODULE = remote
SRCS = remote.c remote_svc.c remote_xdr.c remote.h
CLEANFILES += remote_svc.c remote_xdr.c remote.h
XFILE = $(.CURDIR)/../../include/remote.x

remote.h: $(XFILE)
	rpcgen -C -h -o $@ $(XFILE)

remote_xdr.c: $(XFILE)
	rpcgen -C -c -o $@ $(XFILE)

remote_svc.c: $(XFILE)
	rpcgen -C -m -o $@ $(XFILE)

.include <../Makefile.inc>

