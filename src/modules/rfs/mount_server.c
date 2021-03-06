/* $Id: mount_server.c,v 1.1.1.1 2001/01/22 12:45:12 cvsuser Exp $
 *
 * 2000, unicorn
 * 
 * based on sample code generated by rpcgen.
 */

#include "mount.h"
#include "const.h"

#ifndef lint
/*static char sccsid[] = "from: @(#)mount.x 1.2 87/09/18 Copyr 1987 Sun Micro";*/
/*static char sccsid[] = "from: @(#)mount.x	2.1 88/08/01 4.0 RPCSRC";*/
static const char rcsid[] =
  "$FreeBSD: src/include/rpcsvc/mount.x,v 1.6 1999/08/27 23:45:08 peter Exp $";
#endif /* not lint */

void *
mountproc_null_1(argp, rqstp)
	void *argp;
	struct svc_req *rqstp;
{
	static char * result;

	/*
	 * insert server code here
	 */

	return((void *) &result);
}

fhstatus *
mountproc_mnt_1(argp, rqstp)
	dirpath *argp;
	struct svc_req *rqstp;
{
	int i;
	static fhstatus  result;

	/*
	 * insert server code here
	 */

	FH_SET(result.fhstatus_u.fhs_fhandle, &vmt_cdir, &root, 0);

	return (&result);
}

mountlist *
mountproc_dump_1(argp, rqstp)
	void *argp;
	struct svc_req *rqstp;
{
	static mountlist  result;

	/*
	 * insert server code here
	 */

	return (&result);
}

void *
mountproc_umnt_1(argp, rqstp)
	dirpath *argp;
	struct svc_req *rqstp;
{
	static char * result;

	/*
	 * insert server code here
	 */

	return((void *) &result);
}

void *
mountproc_umntall_1(argp, rqstp)
	void *argp;
	struct svc_req *rqstp;
{
	static char * result;

	/*
	 * insert server code here
	 */

	return((void *) &result);
}

exports *
mountproc_export_1(argp, rqstp)
	void *argp;
	struct svc_req *rqstp;
{
	static exports  result;

	/*
	 * insert server code here
	 */

	return (&result);
}

exports *
mountproc_exportall_1(argp, rqstp)
	void *argp;
	struct svc_req *rqstp;
{
	static exports  result;

	/*
	 * insert server code here
	 */

	return (&result);
}
