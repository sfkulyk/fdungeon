/* $Id: sortedlist.h,v 1.1.1.1 2001/01/22 12:41:13 cvsuser Exp $
 *
 * 2000, unicorn
 *
 * based on singly-linked list by Berkley
 * general purpose: enable particular string election with priority. 
 * i.e.
 * if we have two strings `quit' and `quest' we can set higher priority on
 * `quit', which forces user to type `qui' explicitly.
 * One can set `extrem' priority to be sure user type _exact_ command name
 * (this enables current command processing)
 */

#ifndef __SORTEDLIST_H__
#define __SORTEDLIST_H__

#define	SORTEDLIST_HEAD(name, type)					\
typedef int (* type ## _cmp_fn_t)(struct type *, struct type *);	\
struct name {								\
	type ## _cmp_fn_t slh_cmp_fn; /* compare function ptr */	\
	struct type *slh_first;	 /* first element */			\
};

#define	SORTEDLIST_HEAD_INITIALIZER(head)				\
	{ NULL, NULL }

#define	SORTEDLIST_ENTRY(type)						\
struct {								\
	struct type *sle_next;	/* next element */			\
}
 
/*
 * Singly-linked List functions.
 */

#define SORTEDLIST_CMP(head, key1, key2) ((head)->slh_cmp_fn(key1, key2))

#define	SORTEDLIST_EMPTY(head)	((head)->slh_first == NULL)

#define	SORTEDLIST_FIRST(head)	((head)->slh_first)

#define	SORTEDLIST_FOREACH(var, head, field)				\
	for ((var) = SORTEDLIST_FIRST((head));				\
	    (var);							\
	    (var) = SORTEDLIST_NEXT((var), field))

#define	SORTEDLIST_INIT(head) do {						\
	SORTEDLIST_FIRST((head)) = NULL;					\
} while (0)

#define SORTEDLIST_INSERT(head, elm, type, field) do { \
	if(SORTEDLIST_EMPTY((head))) \
		SORTEDLIST_INSERT_HEAD((head), (elm), field); \
	else { \
		if(SORTEDLIST_CMP((head), \
					SORTEDLIST_FIRST((head)), \
					(elm)) < 0) { \
			struct type *tmp = SORTEDLIST_FIRST((head)); \
			while( \
				SORTEDLIST_NEXT(tmp, field) && \
				SORTEDLIST_CMP((head), \
						SORTEDLIST_NEXT(tmp, field), \
						(elm)) < 0) \
				tmp=SORTEDLIST_NEXT(tmp, field); \
			SORTEDLIST_INSERT_AFTER(tmp, (elm), field); \
		} else \
			SORTEDLIST_INSERT_HEAD((head), (elm), field); \
	} \
} while (0)

#define	SORTEDLIST_INSERT_AFTER(slistelm, elm, field) do {			\
	SORTEDLIST_NEXT((elm), field) = SORTEDLIST_NEXT((slistelm), field);	\
	SORTEDLIST_NEXT((slistelm), field) = (elm);				\
} while (0)

#define	SORTEDLIST_INSERT_HEAD(head, elm, field) do {			\
	SORTEDLIST_NEXT((elm), field) = SORTEDLIST_FIRST((head));			\
	SORTEDLIST_FIRST((head)) = (elm);					\
} while (0)

#define	SORTEDLIST_NEXT(elm, field)	((elm)->field.sle_next)

#define	SORTEDLIST_REMOVE(head, elm, type, field) do {			\
	if (SORTEDLIST_FIRST((head)) == (elm)) {			\
		SORTEDLIST_REMOVE_HEAD((head), field);			\
	}								\
	else {								\
		struct type *curelm = SORTEDLIST_FIRST((head));		\
		while (SORTEDLIST_NEXT(curelm, field) != (elm))		\
			curelm = SORTEDLIST_NEXT(curelm, field);	\
		SORTEDLIST_NEXT(curelm, field) =			\
		    SORTEDLIST_NEXT(SORTEDLIST_NEXT(curelm, field), field);	\
	}								\
} while (0)

#define	SORTEDLIST_REMOVE_HEAD(head, field) do {			\
	SORTEDLIST_FIRST((head)) = SORTEDLIST_NEXT(SORTEDLIST_FIRST((head)), field);	\
} while (0)

#endif /* __SORTEDLIST_H__ */
