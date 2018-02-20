/* $Id: memutils.c,v 1.1.1.1 2001/01/22 12:45:29 cvsuser Exp $
 *
 * 2000, unicorn
 */

#include <sys/types.h>
#include <stdio.h>

#include <cmd.h>
#include <mod.h>

MOD(memutils, 0, "Different tools for memory usage testing.")

#define  MAX_MEM_LIST    11

extern void *rgFreeList[];
extern const int rgSizeList[];

extern AREA_DATA        * area_free;
extern EXTRA_DESCR_DATA * extra_descr_free;
extern EXIT_DATA        * exit_free;
extern ROOM_INDEX_DATA  * room_index_free;
extern OBJ_INDEX_DATA   * obj_index_free;
extern SHOP_DATA        * shop_free;
extern MOB_INDEX_DATA   * mob_index_free;
extern RESET_DATA       * reset_free;
extern HELP_DATA        * help_free;
extern HELP_DATA        * help_last;
extern MPROG_CODE       * mpcode_free;
/*
extern NOTE_DATA *note_free;
extern BAN_DATA *ban_free;
extern DESCRIPTOR_DATA *descriptor_free;
extern GEN_DATA *gen_data_free;
extern EXTRA_DESCR_DATA *extra_descr_free;
extern AFFECT_DATA *affect_free;
extern OBJ_DATA *obj_free;
extern CHAR_DATA *char_free;
extern PC_DATA *pcdata_free;
extern MEM_DATA *mem_data_free;
extern BUFFER *buf_free;
extern MPROG_LIST *mprog_free;
extern CLAN_DATA *clan_free;
extern PENALTY_DATA *penalty_free;
extern NEWS_DATA *news_free;
*/

CMD(heapcheck, 0, "unknown", POS_DEAD, 110, WIZ_NOTES, SHOW|MORPH|HIDE|NOMOB|NOLOG, "Heap check.") {
	int i;
	void **ptr;
	AREA_DATA *area;
	EXTRA_DESCR_DATA *desc;
	EXIT_DATA *exit;
	ROOM_INDEX_DATA *room;
	OBJ_INDEX_DATA *obj;
	SHOP_DATA *shop;
	MOB_INDEX_DATA *mob;
	RESET_DATA *reset;
	MPROG_CODE *mpcode;

	OBJ_DATA *o;

	stc("Checking objs...\n\r", ch);
	for(o=object_list;o;o=o->next);

	stc("---- testing freed memmory ----\n\r", ch);
	
	stc("Checking mpgs...\n\r", ch);
	for(mpcode=mpcode_free;mpcode;mpcode=mpcode->next);
	
	stc("Checking resets...\n\r", ch);
	for(reset=reset_free;reset;reset=reset->next);
	
	stc("Checking resets...\n\r", ch);
	for(reset=reset_free;reset;reset=reset->next);
	
	stc("Checking mobs...\n\r", ch);
	for(mob=mob_index_free;mob;mob=mob->next);
	
	stc("Checking shops...\n\r", ch);
	for(shop=shop_free;shop;shop=shop->next);
	
	stc("Checking objects...\n\r", ch);
	for(obj=obj_index_free;obj;obj=obj->next);
	
	stc("Checking rooms...\n\r", ch);
	for(room=room_index_free;room;room=room->next);
	
	stc("Checking exits...\n\r", ch);
	for(exit=exit_free;exit;exit=exit->next);
		
	stc("Checking descriptions...\n\r", ch);
	for(desc=extra_descr_free;desc;desc=desc->next);
		
	stc("Checking areas...\n\r", ch);
	for(area=area_free;area;area=area->next);

	stc("Checking free strings, buffs, etc...\n\r", ch);
	for(i=0;i<MAX_MEM_LIST;i++)
		if(rgFreeList[i])
			for(ptr=rgFreeList[i];ptr;ptr=(void**)*ptr);
}

