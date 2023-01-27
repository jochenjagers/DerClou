/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_CDROM
#define MODULE_CDROM

#include "theclou.h"

#ifndef MODULE_ERROR
#include "error\error.h"
#endif

extern unsigned int  CDRomInstalled;

extern int CDROM_Install(void);
extern void CDROM_UnInstall(void);
extern int CDROM_WaitForMedia(void);
extern void CDROM_PlayAudioTrack(unsigned char TrackNum);
//extern void CDROM_PlayAudioFrames(unsigned long StartFrame, unsigned long NumOfFrames);
extern void CDROM_PlayAudioSequence(unsigned char TrackNum,unsigned long StartOffset,unsigned long EndOffset);
extern void CDROM_StopAudioTrack(void);
extern int CDROM_SetGameCDDrive(void);

#endif
