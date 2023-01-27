/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/

/*CDRom handling , 2014-07-01 by templer */

#include "SDL.h"

#include "cdrom\cdrom.h"

SDL_CD *CDROM_Struct = NULL;

unsigned int	CDROM_DriveNr;
unsigned int    CDRomInstalled = 0;
char CDDrive[256]="X:\\";

//*********************************************************
//*** CDROM_Install										***
//*********************************************************
// return code:	 0 there was no "The Clou!" CDRom found in drive
//				-1 no CD drive found / could not init CD drive
//               1 CD drive installed and "The Clou!" CDRom found
//
// CDRomInstalled is only set if CD drive and "The Clou!" CDRom was found

int CDROM_Install(void)
{
	CDRomInstalled = 0;

	if (SDL_InitSubSystem (SDL_INIT_CDROM) == 0)	// 2015-01-10 LucyG : changed from SDL_Init to SDL_InitSubSystem
	{
		if (SDL_CDNumDrives() > 0)
		{
			if (CDROM_SetGameCDDrive() == -1)
			{
				return 0;
			}
			else
			{
				CDROM_Struct = SDL_CDOpen(CDROM_DriveNr);
				if (CDROM_Struct)
				{
					if (SDL_CDStatus(CDROM_Struct) != CD_ERROR) {	// 2014-07-12 LucyG : use SDL_CDStatus instead of CDROM_Struct->status
						CDRomInstalled = 1;
						return 1;
					}
					SDL_CDClose(CDROM_Struct);	// 2014-07-13 LucyG : cleanup on error
					CDROM_Struct = NULL;
				}
			}
		}
		return -1;
	}
	return -1;
}


//**********************************************************
//*** CDROM_UnInstall									***
//**********************************************************

void CDROM_UnInstall(void)
{
	if (CDROM_Struct && CDRomInstalled)
	{
		SDL_CDClose(CDROM_Struct);
		CDRomInstalled = 0;
	}
}



//**********************************************************
//*** CDROM_GetDeviceStatus								***
//**********************************************************
/* Get the current CD-Rom status and update the SDL_CD Structure */
int	CDROM_GetDeviceStatus(void)
{
	if (CDROM_Struct)
		return SDL_CDStatus(CDROM_Struct);
	
	return -1;
}




//**********************************************************
//*** CDROM_CheckMediaChanged							***
//**********************************************************

int	CDROM_CheckMediaChanged(void)
{
	if (CDROM_GetDeviceStatus() != CD_TRAYEMPTY)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


//**********************************************************
//*** CDROM_WaitForMedia								***
//**********************************************************
// returns	-1 if user wants to continue without CD
//			0 if the CD/DVD was found
//          
int	CDROM_WaitForMedia(void)
{
	int count = 0;

	// BAD: Windows specific code... sorry templer
	/*
	wchar_t message[256];
	swprintf(message,255,(LPCWSTR)"could not find or read CD/DVD in drive: %s\n press 'Cancel' to continue without CD",CDDrive);
	*/

		while (CDROM_CheckMediaChanged() != 0)
		{
			count += 1;
			SDL_Delay(10);
			if (count == 10000)
			{
				Log("%s|%s: CD/DVD Error", __FILE__, __func__);
				/*
				mID = MessageBox(NULL,message,(LPCWSTR)"CD/DVD Error!", MB_ICONWARNING | MB_RETRYCANCEL | MB_DEFBUTTON2);
				if (mID == IDRETRY)
				{
					count = 0;
				}
				else
				*/
				{
					return -1;
				}
				
			}
		}
		return 0;
}


//**********************************************************
//*** CDROM_PlayAudioTrack				***
//**********************************************************

void CDROM_PlayAudioTrack(unsigned char TrackNum)
{
	if (CDRomInstalled && (CDROM_CheckMediaChanged() == 0))	// 2014-07-13 LucyG : check CDRomInstalled
	{
		if (TrackNum > 0 && CDROM_Struct->numtracks >= TrackNum)
		{
			SDL_CDPlayTracks(CDROM_Struct, TrackNum - 1, 0, 1, 0);
			CDROM_GetDeviceStatus(); //Update CDROM_Struct
		}
		else
		{
			Log("Error: %s|%s|Track number <%d> not found!", __FILE__, __func__,TrackNum);
		}

	}
	
}


/*



//**********************************************************
//*** CDROM_PlayAudioFrames				***
//**********************************************************

void CDROM_PlayAudioFrames(unsigned long StartFrame, unsigned long NumOfFrames)
{
	if (CDRomInstalled && (CDROM_CheckMediaChanged() == 0))	// 2014-07-13 LucyG : check CDRomInstalled
	{
			SDL_CDPlay(CDROM_Struct, StartFrame, NumOfFrames);
			CDROM_GetDeviceStatus(); //Update CDROM_Struct
	}

}
*/




//**********************************************************
//*** CDROM_PlayAudioSequence				***
//**********************************************************

void CDROM_PlayAudioSequence(unsigned char TrackNum,unsigned long StartOffset,unsigned long EndOffset)
{
	Log("%s|%s: TrackNum=%d, StartOffset=%d, EndOffset=%d", __FILE__, __func__, TrackNum, StartOffset, EndOffset);

	if (CDRomInstalled && (CDROM_CheckMediaChanged() == 0))	// 2014-07-13 LucyG : check CDRomInstalled
	{
		if (TrackNum > 0 && CDROM_Struct->numtracks >= TrackNum)
		{
			SDL_CDPlayTracks(CDROM_Struct, TrackNum - 1, StartOffset, 0, EndOffset - StartOffset);
			CDROM_GetDeviceStatus(); //Update CDROM_Struct
		}
	}
	
}


//**********************************************************
//*** CDROM_StopAudioTrack				***
//**********************************************************

void CDROM_StopAudioTrack(void)
{
	if (CDROM_Struct && CDROM_Struct->status != CD_STOPPED)
	{
		SDL_CDStop(CDROM_Struct);
		CDROM_GetDeviceStatus(); //Update CDROM_Struct
	}
}

/* Get the drive number which contains "The Clou!" CD
  * returns -1 if no CD found */

int CDROM_SetGameCDDrive(void)
{
	int num = SDL_CDNumDrives();
	int i;
	SDL_CD *cdrom;

	CDROM_DriveNr=-1;

	if (num > 0)
	{
		for (i=0; i < num; i++)
		{
			cdrom = SDL_CDOpen(i);
			//if (cdrom->status > 0)
			if (CD_INDRIVE(SDL_CDStatus(cdrom)))	// 2014-07-12 LucyG : now it works :)
			{
				if ((cdrom->numtracks == 24) && \
					(cdrom->track[0].type == SDL_DATA_TRACK) && \
					((cdrom->track[1].length > 146000) &&	// templer : 146374  LucyG : 146222
					 (cdrom->track[1].length < 147000))) {
						strcpy(CDDrive,SDL_CDName(i));
						CDROM_DriveNr = i;
						SDL_CDClose(cdrom);		// 2014-07-12 LucyG : close, otherwise re-opening fails
						return i;
				}
			}
			SDL_CDClose(cdrom);
		}
	}
	return -1;
}
