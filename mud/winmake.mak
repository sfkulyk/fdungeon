############################################################################## 
# Makefile to compile ROM to MSVC 4.2 and above
# (C) Saboteur aka Sergii Kulyk

# in make.bat you should add "SET LNKDIR=[Drive][path]" to your MSC directory
# example: SET LNKDIR=c:\!work\link
CC=cl.exe

#CFLAGS= /nologo /ML /W3 /O2g /D "WIN32" /YX /c
CFLAGS= /nologo /ML /W4 /Od /Zi /Yd /D "WIN32" /c /I$(LNKDIR)\include
LINK32= link.exe

#LFLAGS= /LIBPATH:$(LNKDIR)/lib wsock32.lib /subsystem:console /DEBUG /incremental:no /machine:I386 /out:"fdungeon.exe"
LFLAGS= /LIBPATH:win32-libs wsock32.lib /subsystem:console /incremental:no /machine:I386 /out:"fdungeon.exe"
########
ALL : fdungeon.exe

CLEAN :
        -@del *.obj
        -@del ..\areas\*.pdb
	-@del *.pdb
        -@del fdungeon.exe

CFILES = act_comm.c act_info.c act_move.c act_obj.c act_wiz.c \
          astellar.c comm.c const.c db.c db2.c fight.c \
	  handler.c interp.c note.c magic.c magic2.c \
	  music.c recycle.c save.c skills.c special.c tables.c \
	  update.c olc.c olc_act.c olc_save.c bit.c mem.c string.c charset.c \
	  quest.c clan.c mob_cmds.c mob_prog.c olc_mpcode.c social_edit.c\
	  io.c saboteur.c saboteur2.c guild.c fdweb.c

OFILES = act_comm.obj act_info.obj act_move.obj act_obj.obj act_wiz.obj \
          astellar.obj comm.obj const.obj db.obj db2.obj fight.obj \
	  handler.obj interp.obj note.obj magic.obj magic2.obj \
	  music.obj recycle.obj save.obj skills.obj special.obj tables.obj \
	  update.obj olc.obj olc_act.obj olc_save.obj bit.obj mem.obj string.obj charset.obj \
	  quest.obj clan.obj mob_cmds.obj mob_prog.obj olc_mpcode.obj social_edit.obj \
	  io.obj saboteur.obj saboteur2.obj guild.obj fdweb.obj
#########
"fdungeon.exe" : $(DEF_FILE) $(OFILES)
    $(LINK32) @<<
  $(LFLAGS) $(OFILES)
<<

.c.obj: 
    $(CC) $(CFLAGS) $<
