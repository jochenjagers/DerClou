/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_CDROM
#define MODULE_CDROM

#ifdef THECLOU_CDROM_VERSION

#include "theclou.h"

extern unsigned int  CDRomInstalled;

int CDROM_Install(void);
void CDROM_UnInstall(void);
void CDROM_WaitForMedia(void);
void CDROM_PlayAudioSequence(unsigned char TrackNum,unsigned long StartOffset,unsigned long EndOffset);
void CDROM_StopAudioTrack(void);

#endif

#endif
