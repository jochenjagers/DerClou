#
# Der Clou! Open Source Project
# Makefile for lcc-win32 by LucyG
#

SDLDIR = D:\Dev\SDL-1.2.15

#========================================
# Compiler
#========================================
CC = lcc
CFLGS = -O -unused -errout=errout.log
CINCS = -I. -I$(SDLDIR)\include

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
LIBS = $(SDLDIR)\lib\x86\SDL.lib

#========================================
# Target
#========================================
PRG = cosp.exe

#========================================
# Objects
#========================================
OBJS = sound\mxr.obj sound\mxr_voc.obj sound\mxr_wav.obj sound\mxr_hsc.obj sound\fx.obj sound\newsound.obj sound\fmopl.obj \
anim\sysanim.obj base\base.obj data\dataappl.obj data\database.obj data\datacalc.obj data\relation.obj \
dialog\dialog.obj dialog\talkappl.obj disk\disk.obj error\error.obj gameplay\gp.obj gameplay\gp_app.obj \
gameplay\loadsave.obj gameplay\tcreques.obj gfx\gfx.obj gfx\gfxmcga.obj gfx\gfxnch4.obj inphdl\inphdl.obj \
landscap\access.obj landscap\hardware.obj landscap\init.obj landscap\landscap.obj landscap\raster.obj \
landscap\scroll.obj landscap\spot.obj list\list.obj living\bob.obj living\living.obj memory\memory.obj \
organisa\organisa.obj planing\graphics.obj planing\guards.obj planing\io.obj planing\main.obj \
planing\planer.obj planing\player.obj planing\prepare.obj planing\support.obj planing\sync.obj planing\system.obj \
present\interac.obj present\present.obj random\random.obj scenes\cars.obj scenes\dealer.obj scenes\done.obj \
scenes\evidence.obj scenes\inside.obj scenes\invest.obj scenes\scenes.obj scenes\tools.obj \
story\story.obj text\text.obj \
port\port.obj intro\intro.obj intro\iffanim.obj gfx\filter.obj cdrom\cdrom.obj SDL_win32_main.obj

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

#========================================
# 'Make clean' rule
#========================================
clean:
	$(RM) $(OBJS) $(PRG) errout.log

#========================================
# Dependencies
#========================================
