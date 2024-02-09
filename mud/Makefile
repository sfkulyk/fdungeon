#
# Don't forget to `make clean'.

#-DWITH_UNICORN

#-g  Produce debugging information in the operating system's native format (stabs, COFF, XCOFF, or DWARF).  GDB can
#           work with this debugging information.

#With -O, the compiler tries to reduce code size and execution time, without performing any optimizations that
#           take a great deal of compilation time.
#-O0 no optimization

C_FLAGS = -O1  -I../src/include -I/usr/include -I. -DOLD_RAND -Wall

L_FLAGS = -Wl,-E -L../lib -lcrypt

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o astellar.o bit.o charset.o \
	clan.o comm.o const.o db.o db2.o fdweb.o fight.o guild.o handler.o interp.o \
	io.o magic.o magic2.o mem.o mob_cmds.o mob_prog.o music.o note.o olc.o olc_act.o \
	olc_mpcode.o olc_save.o quest.o recycle.o save.o saboteur.o saboteur2.o skills.o \
	social_edit.o special.o string.o tables.o update.o

H_FILES = db.h fdweb.h files.h interp.h magic.h merc_def.h merc.h \
	merc_str.h mob_cmds.h music.h olc.h recycle.h tables.h telnet.h unicorn.h \
	cmd.h rpc.h cmdio.h init.h mod.h remote.x sortedlist.h special.h
 
rom: $(O_FILES)
	@rm -f rom
	@echo ""
	gcc --pipe -o rom $(O_FILES) $(L_FLAGS)
	@chmod 755 rom
	@ls -lsFa rom
        
install: rom
	rm -f ../areas/rom
	mv rom ../areas/rom

clean:
	rm -f rom *.o
  
.c.o: tables.h merc.h
	@echo -n $<" "
	@gcc --pipe -c $(C_FLAGS) $<
	
