# $Id: Makefile,v 1.1 2001/01/28 19:33:02 cvsuser Exp $
#
# 2001, unicorn

XOBJS = remote_xdr.o remote_clnt.o
XFILE = $(.CURDIR)/../include/remote.x
TARGETS = romtell romreboot
CLEANFILES = *.o remote.h remote_xdr.c remote_clnt.c $(TARGETS)

all: $(TARGETS)

romtell: tell.o $(XOBJS)
	cc -o $@ tell.o $(XOBJS)
	strip $@

romreboot: reboot.o $(XOBJS)
	cc -o $@ reboot.o $(XOBJS)
	strip $@

remote.h: $(XFILE)
	rpcgen -C -h -o $@ $(XFILE)

remote_xdr.c: $(XFILE)
	rpcgen -C -c -o $@ $(XFILE)

remote_clnt.c: $(XFILE)
	rpcgen -C -l -o $@ $(XFILE)

install: all
	cp $(TARGETS) $(.CURDIR)/../../areas/

clean:
	rm -f $(CLEANFILES)

depend:
	echo "Not implemented (yet)"

reboot.o: remote.h
tell.o: remote.h
remote_xdr.o: remote.h
remote_clnt.o: remote.h
