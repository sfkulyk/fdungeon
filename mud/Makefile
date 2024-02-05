#
# Don't forget to `make depend'.. 
# or `make clean' if you have much time.
PROF	= -O -g

#-DWITH_UNICORN
#--Werror=unused-but-set-variable -Werror=unused-variable
C_FLAGS = $(PROF) -I../src/include -I/usr/include -I. -DOLD_RAND -Wall -Wno-unused-result -Wno-address -Wincompatible-pointer-types -Wpointer-sign

# fbsd variant
#DDD DSO_FLG =  -Wl,-E -L../lib -R../lib -lrdso
#DSO_FLG =  -Wl,-E -L../lib  -lrdso
DSO_FLG = -Wl,-E -L../lib

L_FLAGS =  $(DSO_FLG) -lcrypt

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o \
	astellar.o comm.o const.o db.o db2.o fight.o \
	handler.o interp.o note.o magic.o magic2.o \
	music.o recycle.o save.o skills.o special.o tables.o \
	update.o olc.o olc_act.o olc_save.o bit.o mem.o string.o charset.o \
	quest.o clan.o mob_cmds.o mob_prog.o olc_mpcode.o social_edit.o \
	io.o saboteur.o saboteur2.o guild.o fdweb.o

H_FILES = db.h fdweb.h files.h interp.h magic.h merc_def.h merc.h \
	merc_str.h mob_cmds.h music.h olc.h recycle.h tables.h telnet.h unicorn.h
 
rom: $(O_FILES)
	rm -f rom
	gcc -o rom $(O_FILES) $(L_FLAGS)
	chmod 700 rom
        
install: rom
	rm -f ../areas/rom
	mv rom ../areas/rom

clean:
	rm -f rom *.o
  
.c.o: tables.h merc.h
	gcc -c $(C_FLAGS) $<
