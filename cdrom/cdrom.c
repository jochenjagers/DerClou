/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include <dos.h>
#include <mem.h>

#include "cdrom\cdrom.h"

#ifdef THECLOU_CDROM_VERSION
#define TRUE -1
#define FALSE 0


struct CDROM_Struct_ReqHeader
{
	unsigned char	Len;
	unsigned char	SubUnit;
	unsigned char	CommandCode;
	unsigned int	Status;
	char		DevName[8];
};


struct	CDROM_Struct_OpenClose
{
	struct CDROM_Struct_ReqHeader	ReqHeader;
} CDROM_OpenClose;


struct	CDROM_Struct_IOCTLO
{
	struct CDROM_Struct_ReqHeader	ReqHeader;
	unsigned char			MediaDesc;
	void				far *Buffer;
	unsigned int			BufferSize;
	unsigned long			StartSector;
	unsigned long			VolumePtr;
} CDROM_IO;


struct	CDROM_Struct_PlayAudio
{
	struct CDROM_Struct_ReqHeader	ReqHeader;
	unsigned char			AdrMode;
	unsigned long			StartSector;
	unsigned long			NumOfSectors;
} CDROM_PlayAudio;


struct	CDROM_Struct_StopAudio
{
	struct CDROM_Struct_ReqHeader	ReqHeader;
} CDROM_StopAudio;


struct	CDROM_Struct_DeviceStatus
{
	unsigned int	Command;
	unsigned long	Status;
} CDROM_DeviceStatus;


struct	CDROM_Struct_AudioDiskInfo
{
	unsigned char	Command;
	unsigned char   FirstTrack;
	unsigned char	LastTrack;
	unsigned long	LeadOutTrack;
} CDROM_AudioDiskInfo;


struct	CDROM_Struct_AudioTrackInfo
{
	unsigned char	Command;
	unsigned char	TrackNum;
	unsigned long	StartSector;
	unsigned char	TrackInfo;
} CDROM_AudioTrackInfo;


struct	CDROM_Struct_MediaChanged
{
	unsigned char	Command;
	unsigned char	Status;
} CDROM_MediaChanged;


struct REGPACK 	r;
unsigned int	CDROM_DriveNr;
unsigned long	CDROM_TrackBegin;
unsigned long	CDROM_TrackEnd;
unsigned long	CDROM_TrackFrames;
unsigned char	CDROM_CurrentTrack;

unsigned int  CDRomInstalled = 0;

//*********************************************************
//*** CDROM_Install					***
//*********************************************************

int	CDROM_Install(void)
{
	int retval;


	r.r_ax = 0x1500;
	r.r_bx = 0x0;
	intr( 0x2F,&r);
	CDROM_DriveNr = r.r_cx;

//	asm mov ax,0x1500
//	asm mov bx,0
//	asm int 0x2f
//	asm mov CDROM_DriveNr,cx

	if ( r.r_bx == 0 )     	retval = FALSE;
	else			retval = TRUE;

	CDROM_OpenClose.ReqHeader.Len = sizeof(CDROM_OpenClose);
	CDROM_OpenClose.ReqHeader.CommandCode = 13;
	CDROM_OpenClose.ReqHeader.Status = 0;

	r.r_ax = 0x1510;
	r.r_cx = CDROM_DriveNr;
	r.r_bx = FP_OFF(&CDROM_OpenClose);
	r.r_es = FP_SEG(&CDROM_OpenClose);
	intr(0x2F,&r);

	return (retval);

}


//**********************************************************
//*** CDROM_UnInstall					***
//**********************************************************

void	CDROM_UnInstall(void)
{

	CDROM_OpenClose.ReqHeader.Len = sizeof(CDROM_OpenClose);
	CDROM_OpenClose.ReqHeader.CommandCode = 14;
	CDROM_OpenClose.ReqHeader.Status = 0;

	r.r_ax = 0x1510;
	r.r_cx = CDROM_DriveNr;
	intr(0x2F,&r);

}


//**********************************************************
//*** CDROM_BinToFrames					***
//**********************************************************
unsigned long	CDROM_BinToFrames(unsigned long sectors)
{
	unsigned long Min,Sec,Frames;

	Frames = sectors & 0xFF;
	Sec    = (sectors >> 8) & 0xFF;
	Min    = (sectors >> 16) & 0xFF;

	return ((((Min*60)+Sec)*75)+Frames);

}

//**********************************************************
//*** CDROM_FramesToBin				***
//**********************************************************
unsigned long	CDROM_FramesToBin(unsigned long FramesToConvert)
{
	unsigned long Min,Sec,Frames;

	Frames = FramesToConvert % 75;
	FramesToConvert = FramesToConvert / 75;
	Sec = FramesToConvert % 60;
	Min = FramesToConvert / 60;


	return ((Min << 16) | (Sec << 8) | Frames);

}



//**********************************************************
//*** CDROM_FramesToMin					***
//**********************************************************
unsigned char	CDROM_FramesToMin(unsigned long FramesToConvert)
{
	return ((unsigned char )(FramesToConvert / (60*75)));
}



//**********************************************************
//*** CDROM_FramesToSec					***
//**********************************************************
unsigned char 	CDROM_FramesToSec(unsigned long FramesToConvert)
{
	return ( (FramesToConvert / 75) % 60);
}



//**********************************************************
//*** CDROM_FramesToFrames				***
//**********************************************************
unsigned char	CDROM_FramesToFrames(unsigned long FramesToConvert)
{
	return ( FramesToConvert % 75);
}






//**********************************************************
//*** CDROM_GetDeviceStatus				***
//**********************************************************

void	CDROM_GetDeviceStatus(void)
{
	memset ( &CDROM_IO, 0, sizeof( CDROM_IO));

	CDROM_IO.ReqHeader.Len= sizeof( CDROM_IO);
	CDROM_IO.ReqHeader.CommandCode= 3;
	CDROM_IO.Buffer = &CDROM_DeviceStatus;
	CDROM_IO.BufferSize = sizeof(CDROM_DeviceStatus);

	CDROM_DeviceStatus.Command = 6;
	CDROM_DeviceStatus.Status = 0;

	r.r_ax = 0x1510;
	r.r_cx = CDROM_DriveNr;
	r.r_bx = FP_OFF(&CDROM_IO);
	r.r_es = FP_SEG(&CDROM_IO);
	intr (0x2F,&r);
}




//**********************************************************
//*** CDROM_CheckMediaChanged				***
//**********************************************************

int	CDROM_CheckMediaChanged(void)
{
	memset ( &CDROM_IO, 0, sizeof( CDROM_IO));

	CDROM_IO.ReqHeader.Len= sizeof( CDROM_IO);
	CDROM_IO.ReqHeader.CommandCode= 3;
	CDROM_IO.Buffer = &CDROM_MediaChanged;
	CDROM_IO.BufferSize = sizeof(CDROM_MediaChanged);

	CDROM_MediaChanged.Command = 9;
	CDROM_MediaChanged.Status = 0;

	r.r_ax = 0x1510;
	r.r_cx = CDROM_DriveNr;
	r.r_bx = FP_OFF(&CDROM_IO);
	r.r_es = FP_SEG(&CDROM_IO);
	intr(0x2F,&r);

	if (CDROM_MediaChanged.Status == 0xFF)
		return (TRUE);
	else
		return (FALSE);
}


//**********************************************************
//*** CDROM_CheckMediaChanged				***
//**********************************************************

void	CDROM_WaitForMedia(void)
{
	while (CDROM_CheckMediaChanged());
}



//**********************************************************
//*** CDROM_GetAudioDiskInfo				***
//**********************************************************

void	CDROM_GetAudioDiskInfo(void)
{
	memset( &CDROM_IO, 0, sizeof( CDROM_IO));

	CDROM_IO.ReqHeader.Len= sizeof( CDROM_IO);
	CDROM_IO.ReqHeader.CommandCode= 3;
	CDROM_IO.Buffer = &CDROM_AudioDiskInfo;
	CDROM_IO.BufferSize = sizeof(CDROM_AudioDiskInfo);

	CDROM_AudioDiskInfo.Command = 10;
	CDROM_AudioDiskInfo.FirstTrack = 0;
	CDROM_AudioDiskInfo.LastTrack = 0;
	CDROM_AudioDiskInfo.LeadOutTrack = 0;

	r.r_ax = 0x1510;
	r.r_cx = CDROM_DriveNr;
	r.r_bx = FP_OFF(&CDROM_IO);
	r.r_es = FP_SEG(&CDROM_IO);
	intr(0x2F,&r);

}


//**********************************************************
//*** CDROM_GetAudioTrackInfo				***
//**********************************************************

void	CDROM_GetAudioTrackInfo(unsigned char TrackNum)
{
	memset( &CDROM_IO, 0,sizeof( CDROM_IO));

	CDROM_IO.ReqHeader.Len= sizeof( CDROM_IO);
	CDROM_IO.ReqHeader.CommandCode= 3;
	CDROM_IO.Buffer = &CDROM_AudioTrackInfo;
	CDROM_IO.BufferSize = sizeof(CDROM_AudioTrackInfo);

	CDROM_AudioTrackInfo.Command = 11;
	CDROM_AudioTrackInfo.TrackNum = TrackNum;
	CDROM_AudioTrackInfo.StartSector = 0;
	CDROM_AudioTrackInfo.TrackInfo = 0;

	r.r_ax = 0x1510;
	r.r_cx = CDROM_DriveNr;
	r.r_bx = FP_OFF(&CDROM_IO);
	r.r_es = FP_SEG(&CDROM_IO);
	intr(0x2F,&r);

}

//**********************************************************
//*** CDROM_PlayAudioTrack				***
//**********************************************************

void	CDROM_PlayAudioTrack(unsigned char TrackNum)
{

	CDROM_GetAudioDiskInfo();

	CDROM_GetAudioTrackInfo(TrackNum);
	CDROM_TrackBegin = CDROM_BinToFrames(CDROM_AudioTrackInfo.StartSector);


	if (TrackNum == CDROM_AudioDiskInfo.LastTrack)
		CDROM_TrackEnd = CDROM_BinToFrames(CDROM_AudioDiskInfo.LeadOutTrack);
	else
	{
		CDROM_GetAudioTrackInfo(TrackNum+1);
		CDROM_TrackEnd = CDROM_BinToFrames(CDROM_AudioTrackInfo.StartSector);
	}

	CDROM_TrackFrames = CDROM_TrackEnd - CDROM_TrackBegin;


	memset(&CDROM_PlayAudio,0,sizeof(CDROM_PlayAudio));

	CDROM_PlayAudio.ReqHeader.Len = sizeof(CDROM_PlayAudio);
	CDROM_PlayAudio.ReqHeader.CommandCode = 132;
	CDROM_PlayAudio.AdrMode = 1;
	CDROM_PlayAudio.StartSector = CDROM_FramesToBin(CDROM_TrackBegin);
	CDROM_PlayAudio.NumOfSectors = CDROM_TrackFrames;

	r.r_ax = 0x1510;
	r.r_cx = CDROM_DriveNr;
	r.r_bx = FP_OFF(&CDROM_PlayAudio);
	r.r_es = FP_SEG(&CDROM_PlayAudio);
	intr(0x2F,&r);
}



//**********************************************************
//*** CDROM_PlayAudioFrames				***
//**********************************************************

void	CDROM_PlayAudioFrames(unsigned long StartFrame, unsigned long NumOfFrames)
{
	memset(&CDROM_PlayAudio,0,sizeof(CDROM_PlayAudio));

	CDROM_PlayAudio.ReqHeader.Len = sizeof(CDROM_PlayAudio);
	CDROM_PlayAudio.ReqHeader.CommandCode = 132;
	CDROM_PlayAudio.AdrMode = 1;
	CDROM_PlayAudio.StartSector = CDROM_FramesToBin(StartFrame);
	CDROM_PlayAudio.NumOfSectors = NumOfFrames;

	r.r_ax = 0x1510;
	r.r_cx = CDROM_DriveNr;
	r.r_bx = FP_OFF(&CDROM_PlayAudio);
	r.r_es = FP_SEG(&CDROM_PlayAudio);
	intr(0x2F,&r);

}




//**********************************************************
//*** CDROM_PlayAudioSequence				***
//**********************************************************

void	CDROM_PlayAudioSequence(unsigned char TrackNum,unsigned long StartOffset,unsigned long EndOffset)
{

	CDROM_GetAudioTrackInfo(TrackNum);
	CDROM_TrackBegin = StartOffset + CDROM_BinToFrames(CDROM_AudioTrackInfo.StartSector);
	CDROM_TrackEnd = EndOffset + CDROM_BinToFrames(CDROM_AudioTrackInfo.StartSector);
	CDROM_TrackFrames = CDROM_TrackEnd - CDROM_TrackBegin;


	memset(&CDROM_PlayAudio,0,sizeof(CDROM_PlayAudio));

	CDROM_PlayAudio.ReqHeader.Len = sizeof(CDROM_PlayAudio);
	CDROM_PlayAudio.ReqHeader.CommandCode = 132;
	CDROM_PlayAudio.AdrMode = 1;
	CDROM_PlayAudio.StartSector = CDROM_FramesToBin(CDROM_TrackBegin);
	CDROM_PlayAudio.NumOfSectors = CDROM_TrackFrames;

	r.r_ax = 0x1510;
	r.r_cx = CDROM_DriveNr;
	r.r_bx = FP_OFF(&CDROM_PlayAudio);
	r.r_es = FP_SEG(&CDROM_PlayAudio);
	intr(0x2F,&r);
}




//**********************************************************
//*** CDROM_StopAudioTrack				***
//**********************************************************

void	CDROM_StopAudioTrack(void)
{
	CDROM_GetDeviceStatus();
	if ((CDROM_IO.ReqHeader.Status & 0x0200) != 0)
		{
		CDROM_StopAudio.ReqHeader.Len = sizeof(CDROM_PlayAudio);
		CDROM_StopAudio.ReqHeader.CommandCode = 133;

		r.r_ax = 0x1510;
		r.r_cx = CDROM_DriveNr;
		r.r_bx = FP_OFF(&CDROM_StopAudio);
		r.r_es = FP_SEG(&CDROM_StopAudio);
		intr(0x2F,&r);
      }
}
#else

#ifndef __COSP__
unsigned int  CDRomInstalled = 1;
#endif

#endif
