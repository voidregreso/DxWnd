// CDAudio.h: interface for the CCDAudio class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CDAUDIO1_H__FDE356E6_CB80_41FE_82BE_9F051620A4A0__INCLUDED_)
#define AFX_CDAUDIO1_H__FDE356E6_CB80_41FE_82BE_9F051620A4A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mmsystem.h"

#ifdef _UNICODE
#define	ATOI	_wtoi
#define	ITOA	_itow
#define	STRNCPY	wcsncpy
#else
#define ATOI	atoi
#define	ITOA	_itoa
#define	STRNCPY	strncpy
#endif


//=============================================================================
// A class wrapper of MCI API functions
//=============================================================================
class CCDAudio {
public:
    // constructor
    CCDAudio();
    // destructor
    virtual ~CCDAudio();

    // Start playing CD Audio
    MCIERROR Play();
    // Start playing CD Audio on given position
    MCIERROR Play(const int nPos);
    // Stop playing CD Audio
    MCIERROR Stop();
    // Pause playing CD Audio
    MCIERROR Pause();
    // Move to the next track
    MCIERROR Forward();
    // Move to the previous track
    MCIERROR Backward();
    // Eject the CDROM
    void EjectCDROM();

    // Return the current position in seconds
    int GetCurrentPos();
    // Return the current track number
    int GetCurrentTrack();
    // Return length of all track in seconds
    int GetLenghtAllTracks();
    // Return total tracks count
    int GetTracksCount();
    // Return length of given track
    int GetTrackLength(const int nTrack);
    // Return begin time of given track
    int GetTrackBeginTime( const int nTrack );
    // set volume
    bool SetVolume(unsigned int);

    // check wheter CD media is inserted
    bool IsMediaInsert();
    // is paused mode
    bool IsPaused();
    // is stopped mode
    bool IsStopped();
    // the device is ready
    bool IsReady();
    // is playing mode
    bool IsPlaying();

protected:
    // MCI error code
    MCIERROR m_nErrorCode;

protected:
    // handle MCI errors
    inline void MCIError( MCIERROR MCIError );
};

#endif // !defined(AFX_CDAUDIO1_H__FDE356E6_CB80_41FE_82BE_9F051620A4A0__INCLUDED_)
