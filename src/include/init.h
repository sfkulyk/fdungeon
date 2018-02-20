/* $Id: init.h,v 1.1.1.1 2001/01/22 12:41:33 cvsuser Exp $
 *
 * 2000, unicorn
 */

#ifndef __INIT_H__
#define __INIT_H__

#define CTOR(uniq) \
static void __ ## uniq ## _init() __attribute__ ((constructor)); \
static void __ ## uniq ## _init()

#define DTOR(uniq) \
static void __ ## uniq ## _done() __attribute__ ((destructor)); \
static void __ ## uniq ## _done()

#endif /* __INIT_H__ */
