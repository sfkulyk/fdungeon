/* $Id: cmd.c,v 1.39 2001/02/26 22:30:21 cvsuser Exp $
 *
 * 2000, unicorn
 */

#include <cmd.h>

#if 0
CMD(
/* name     */	msystem,
/* nice     */	50,
/* rname    */	"unknown", 
/* position */	POS_DEAD, 
/* level    */	0, 
/* log      */	LOG_NEVER, 
/* flags    */	MORPH|HIDE|NOMOB|NOLOG,
/* descript */	"Test command"
/* void ARGUSED (CHAR_DATA *ch, char *argument)*/) {
	char buf[MAX_STRING_LENGTH];
	
	if( get_trust(ch) < 109 && !IS_SET(ch->comm, COMM_CODER) ) {
		stc("This command is only for coders.\r\n", ch);
		return;
	}

	snprintf(buf, sizeof(buf), "%s 2>&1 | ./romtell %s &", argument, ch->name);
	system(buf);
}
#endif


CMD(
/* name     */	modtest,
/* nice     */	50,
/* rname    */	"unknown", 
/* position */	POS_DEAD, 
/* level    */	0, 
/* log      */	LOG_NEVER, 
/* flags    */	MORPH|HIDE|NOMOB|NOLOG,
/* descript */	"Test command"
/* void ARGUSED (CHAR_DATA *ch, char *argument)*/) {
	char name[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	CHAR_DATA *wch;
	int number;
	int count;

	if( get_trust(ch) < 109 && !IS_SET(ch->comm, COMM_CODER) ) {
		stc("This command is only for coders.\r\n", ch);
		return;
	}

	argument = one_argument( argument, buf );
	if(!*buf) {
		for(wch = char_list; wch ; wch = wch->next)
/*		for(wch = ch->in_room->people; wch ;wch = wch->next_in_room)
*/			if(wch->in_room && !IS_NPC(wch))
				ptc(ch, "{W%s{x\r\n", wch->name);

		return;
	}

	number = number_argument( buf, name );
	count  = 0;
	for (wch = char_list; wch ; wch = wch->next) {
		if (!wch->in_room || !is_name(name, wch->name) || IS_NPC(wch))
			continue;

		if (++count == number)
			break;
	}

	if(wch)
		tell(ch, wch, argument);
	else
		stc("Oops\r\n", ch);
}


CMD(
		rebuild,
		50,
		"unknown",
		POS_DEAD,
		0,
		LOG_NEVER,
		MORPH|HIDE|NOMOB|NOLOG,
		"Rebuild loadable modules"
) {
	char buff[1024];

	if( get_trust(ch) < 109 && !IS_SET(ch->comm, COMM_CODER) ) {
		stc("This command only for coders.\r\n", ch);
		return;
	}

	snprintf(buff, sizeof(buff),
		"(cd ../src/modules; ./rebuild) 2>&1 | ./romtell %s &",
		ch->name);

	system(buff);
}

