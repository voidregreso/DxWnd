// =========================================================================== //
// ===                          CD audio emulation                         === //
// =========================================================================== //

/*
    Copyright (c) 2012 Toni Spets <toni.spets@iki.fi>

    Permission to use, copy, modify, and distribute this software for any
    purpose with or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/*
    Copyright (c) 2017 gho

    Yeah, same for me too!
*/

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#define MAX_TRACKS 99
#define MCI_DATA_TRACK 1
#define MCI_AUDIO_TRACK 2

#ifdef WIN32
#define snprintf _snprintf
#endif

struct track_info {
    unsigned int type;		// either MCI_DATA_TRACK or MCI_AUDIO_TRACK
    unsigned int length;    // seconds
    unsigned int position;  // seconds
};

extern struct track_info tracks[MAX_TRACKS];

struct play_info {
    DWORD_PTR play_callback;
    DWORD_PTR stop_callback;
    DWORD first;
    DWORD last;
};

//#ifdef _DEBUG
//#define dprintf(...) OutTraceSND(__VA_ARGS__)
//#else
//#define dprintf(...)
//#endif
#define dprintf(...) OutTraceSND(__VA_ARGS__)

extern struct play_info info;

typedef struct {
    CRITICAL_SECTION cs;
    DWORD dwStatus;
    BOOL bUpdateTrack;
    DWORD dwSeekedTrack;
    DWORD dwTimeFormat;
    DWORD dwNumTracks; // total number of tracks
    DWORD dwFirstTrack; // beginning of playable audio data in TTMMSS format, usually 020000
    DWORD dwLastTrack; // end of audio playable audio data in TTMMSS format
    DWORD dwCurrentTrack; // currently played track in TTMMSS format, MMSS incremented while track plays
    DWORD dwDevID;
    int playing;
    int paused;
    int dooropened;
} mciEmuDescriptor;

extern mciEmuDescriptor mciEmu;

extern int (*pplr_pump)(void);
extern void (*pplr_stop)(void);
extern int (*pplr_play)(int);
extern void (*pplr_setvolume)(int);
extern int (*pplr_getvolume)(void);
extern int (*pplr_seek)(int);
extern int (*pplr_tell)(void);

extern int player_init();
extern int player_main();
extern void player_set_status(DWORD);
extern void player_change();

#define TTMMSS_ENCODE(t, m, s) (((t)<<16) | ((m)<<8) | (s))
#define TTMMSS_TRACKNO(x) ((x & 0xFF0000) >> 16)
#define TTMMSS_NEXTTRACK 0x010000
#define TTMMSS_ENDOFTRACK 0x00FFFF
#define TTMMSS_OFFSET(x) (((x & 0xFF00) >> 8) * 60 + (x & 0xFF))
#define TTMMSS_MINUTES(x) ((x & 0x00FF00) >> 8)
#define TTMMSS_SECONDS(x) (x & 0x0000FF)

#define MCI_FRAMES_PER_SECOND 75
#define MCI_FORMAT_UNDEFINED (-1)

#define MCIEMU_DOOR_OPEN	0xFFFFFFF1
#define MCIEMU_DOOR_CLOSED	0xFFFFFFF2
