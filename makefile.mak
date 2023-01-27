#
# Der Clou! Open Source Project
# Makefile for lcc-win32 by LucyG
#

#========================================
# Compiler
#========================================
CC = lcc
CFLGS = -O -unused -errout=errout.log
CINCS = -I. -Ic:\programme\developer\lcc\include\sdl

#========================================
# Linker
#========================================
LD = lcclnk
LDFLG = -s -x -errout errout.log -subsystem windows

#========================================
# Remove
#========================================
RM = rm -f

#========================================
# Linklibs
#========================================
LIBS = SDLmain.obj SDL.lib

#========================================
# Target
#========================================
PRG = ClouDlx.exe

#========================================
# Objects
#========================================
OBJS = anim\sysanim.obj base\base.obj data\dataappl.obj data\database.obj data\datacalc.obj data\relation.obj \
dialog\dialog.obj dialog\talkappl.obj disk\disk.obj error\error.obj gameplay\gp.obj gameplay\gp_app.obj \
gameplay\loadsave.obj gameplay\tcreques.obj gfx\gfx.obj gfx\gfxmcga.obj gfx\gfxnch4.obj inphdl\inphdl.obj \
landscap\access.obj landscap\hardware.obj landscap\init.obj landscap\landscap.obj landscap\raster.obj \
landscap\scroll.obj landscap\spot.obj list\list.obj living\bob.obj living\living.obj memory\memory.obj \
organisa\organisa.obj planing\graphics.obj planing\guards.obj planing\io.obj planing\main.obj \
planing\planer.obj planing\player.obj planing\prepare.obj planing\support.obj planing\sync.obj planing\system.obj \
present\interac.obj present\present.obj random\random.obj scenes\cars.obj scenes\dealer.obj scenes\done.obj \
scenes\evidence.obj scenes\inside.obj scenes\invest.obj scenes\scenes.obj scenes\tools.obj sound\fx.obj \
sound\newsound.obj story\story.obj text\text.obj port\port.obj

#========================================
# 'Make all' rule
#========================================
all: $(PRG)

#========================================
# Compiler rule
#========================================
.c.obj:
	$(CC) $(CFLGS) $(CINCS) $< -Fo$@

#========================================
# Linker rule
#========================================
$(PRG): $(OBJS)
	$(LD) $(LDFLG) $(OBJS) $(LIBS) -o $@
	nullcave $@
	upx -9 $@

#========================================
# 'Make clean' rule
#========================================
clean:
	$(RM) $(OBJS) $(PRG) errout.log

#========================================
# Dependencies
#========================================
