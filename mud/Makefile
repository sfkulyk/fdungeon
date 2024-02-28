# C_FLAGS
# -g  Produce debugging information for GDB
# -O0 no optimization
# -fcommon (using global definitions)
# L_FLAGS
# --std=gnu89
# -lcrypt -L../lib -Wl, -E
C_FLAGS = -O1 -I/usr/include -I. -DOLD_RAND -Wall -fcommon

L_FLAGS = -Wl,-E -Lcrypt

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o astellar.o bit.o charset.o clan.o comm.o \
	const.o db.o db2.o fdweb.o fight.o guild.o handler.o interp.o io.o magic.o magic2.o mem.o mob_cmds.o \
 	mob_prog.o music.o note.o olc.o olc_act.o olc_mpcode.o olc_save.o quest.o recycle.o save.o saboteur.o \
	skills.o social_edit.o special.o string.o tables.o update.o

.c.o:
	@echo -n $<" "
	@gcc --pipe -c $(C_FLAGS) $<

rom: $(O_FILES)
	@rm -f rom
	@echo -e "\nLinking: "
	gcc --pipe -o rom $(O_FILES) $(L_FLAGS) 
	@chmod 755 rom
	@echo -n "Result: "
	@ls -lsFa rom
        
install: rom
	rm -f ../areas/rom
	mv rom ../areas/rom

clean:
	rm -f rom *.o
