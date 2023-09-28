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
    Copyright (c) 2017-2018 gho

    Yeah, same for me too!
*/

#define  _CRT_SECURE_NO_WARNINGS

// drflac switches
#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_CRC

// drwav switches
#define DR_WAV_IMPLEMENTATION
#define DR_MP3_IMPLEMENTATION
#define DR_FLAC_NO_CRC

#define wav_buffers 44100 // 44100 = (44100/2) * 2 that is 5000 mSec of stereo sound data
#define flac_buffers 44100 // 44100 = (44100/2) * 2 that is 5000 mSec of stereo sound data
#define mp3_frames2read 11025

#include <vorbis/vorbisfile.h>
#include <flac/dr_flac.h>
#include <wav/dr_wav.h>
#include <mp3/dr_mp3.h>
#include <stdio.h>
#include <windows.h>

#define MCI_DATA_TRACK 1
#define MCI_AUDIO_TRACK 2

#define MCI_CODEC_DATA 0
#define MCI_CODEC_OGGVORBIS 1
#define MCI_CODEC_MP3 2
#define MCI_CODEC_WAV 3
#define MCI_CODEC_FLAC 4

#define MCI_CODEC_FIRST 0
#define MCI_CODEC_LAST 4

//#define DXWASYNCPLAY
//#define PLAYDEBUG
#define CACHETRACKINFO

char sRootPath[MAX_PATH + 1];

#ifdef PLAYDEBUG
static void soundlog(const char *format, ...) {
    static FILE *ftrace = NULL;
    va_list al;
    if(!ftrace) {
        // beware: don't call log soundlog befores RootPath initialization!
        char sTracePath[MAX_PATH];
        sprintf(sTracePath, "%s\\dxwplay.log", sRootPath);
        ftrace = fopen(sTracePath, "w");
        if(!ftrace) return;
    }
    va_start(al, format);
    __try {
        vfprintf(ftrace, format, al);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        fprintf(ftrace, "***\n");
    }
    fflush(ftrace);
}
#define log(f, ...) soundlog(f, __VA_ARGS__)
#else
#define log(f, ...)
#endif

LPCSTR plr_version() {
    return "v1.01";
}

drflac *pFlac = NULL;
drwav *pWav = NULL;
drmp3 Mp3;

int nTrackFrames; // frames read so far
char *folderSuffix = "";

struct track_info {
    unsigned int type;		// either MCI_DATA_TRACK or MCI_AUDIO_TRACK
    unsigned int length;    // seconds
    unsigned int position;  // seconds
};

int *codecs = NULL;
int gPlayer = 0;
BOOL gMP3Playing = FALSE;
DWORD mp3_begin_timer = 0;
DWORD mp3_end_timer = 0;
HANDLE mp3Handler = NULL;
track_info *gTracks;

#define MAX_COMMAND 256

WAVEFORMATEX    plr_fmt;
HWAVEOUT        plr_hwo         = NULL;
OggVorbis_File  plr_vf;
HANDLE          plr_ev          = NULL;
int             plr_cnt         = 0;
int             plr_vol         = 100;
WAVEHDR         *plr_buffers[3] = { NULL, NULL, NULL };

drmp3_uint64 drmp3_get_frames_and_close(drmp3 *pMP3) {
    drmp3_assert(pMP3 != NULL);
    drmp3_uint64 totalFramesRead = 0;
    drmp3_uint64 framesCapacity = 0;
    float *pFrames = NULL;
    float temp[4096];
    for (;;) {
        drmp3_uint64 framesToReadRightNow = drmp3_countof(temp) / pMP3->channels;
        drmp3_uint64 framesJustRead = drmp3_read_f32(pMP3, framesToReadRightNow, temp);
        if (framesJustRead == 0) break;
        totalFramesRead += framesJustRead;
        // If the number of frames we asked for is less that what we actually read it means we've reached the end.
        if (framesJustRead != framesToReadRightNow) break;
    }
    drmp3_uninit(pMP3);
    return totalFramesRead;
}

static void waveOutStop() {
    plr_cnt = 0;
    if (plr_vf.datasource) ov_clear(&plr_vf);
    if (plr_ev) {
        CloseHandle(plr_ev);
        plr_ev = NULL;
    }
    if (plr_hwo) {
        waveOutReset(plr_hwo);
        int i;
        for (i = 0; i < 3; i++) {
            if (plr_buffers[i] && plr_buffers[i]->dwFlags & WHDR_DONE) {
                waveOutUnprepareHeader(plr_hwo, plr_buffers[i], sizeof(WAVEHDR));
                free(plr_buffers[i]->lpData);
                free(plr_buffers[i]);
                plr_buffers[i] = NULL;
            }
        }
        waveOutClose(plr_hwo);
        plr_hwo = NULL;
    }
}

static void ogg_stop() {
    log("%08.8d: ogg_stop\n", GetTickCount());
    waveOutStop();
}

//static void mci_stop()
//{
//	log("%08.8d: mci_stop\n", GetTickCount());
//
//	char retstr[MAX_COMMAND+1];
//	mciSendString("stop mp3", retstr, MAX_COMMAND, NULL);
//	mciSendString("close mp3", retstr, MAX_COMMAND, NULL);
//	gMP3Playing = FALSE;
//	mp3_begin_timer = 0;
//}

static void flc_stop() {
    log("%08.8d: flc_stop\n", GetTickCount());
    if(pFlac) {
        drflac_close(pFlac);
        pFlac = NULL;
    }
    nTrackFrames = 0;
    waveOutStop();
}

static void wav_stop() {
    log("%08.8d: wav_stop\n", GetTickCount());
    if(pWav) {
        drwav_close(pWav);
        pWav = NULL;
    }
    nTrackFrames = 0;
    waveOutStop();
}

static void mp3_stop() {
    log("%08.8d: mp3_stop\n", GetTickCount());
    nTrackFrames = 0;
    waveOutStop();
}

void plr_stop() {
    switch(gPlayer) {
    case MCI_CODEC_OGGVORBIS:
        ogg_stop();
        break;
    case MCI_CODEC_MP3:
        mp3_stop();
        break;
    case MCI_CODEC_FLAC:
        flc_stop();
        break;
    case MCI_CODEC_WAV:
        wav_stop();
        break;
    }
}

void plr_setvolume(int vol) {
    log("%08.8d: plr_setvol(%d)\n", GetTickCount(), vol);
    if (vol < 0) vol = 0;
    if (vol > 100) vol = 100;
    plr_vol = vol;
    switch(gPlayer) {
    case MCI_CODEC_MP3:
    case MCI_CODEC_WAV: {
        char command[MAX_COMMAND + 1];
        char retstr[20 + 1];
        // beware: mci volume ranges 0 to 1000
        sprintf(command, "setaudio mp3 volume to %d", 10 * plr_vol);
        mciSendString(command, retstr, 20, NULL); // set mci volume
    }
    break;
    }
}

int plr_getvolume(void) {
    log("%08.8d: plr_getvol\n", GetTickCount());
    return plr_vol;
}

static int dat_length(const char *path) {
    int len;
    FILE *fp = fopen(path, "r");
    if(fp == NULL) return 4; // 4 seconds, as before ...
    fscanf(fp, "%d", &len);
    log("%08.8d: dat_length path=%s len=%d\n", GetTickCount(), path, len);
    return len;
}

static int ogg_length(const char *path) {
    OggVorbis_File  vf;
    if (ov_fopen(path, &vf) != 0) return 0;
    int ret = (int)ov_time_total(&vf, -1);
    ov_clear(&vf);
    log("%08.8d: ogg_length path=%s len=%d\n", GetTickCount(), path, ret);
    return ret;
}

static int mci_length(const char *path) {
    char command[MAX_COMMAND + 1];
    char retstr[MAX_COMMAND + 1];
    int ret = 0;
    sprintf(command, "status %s length", path);
    if(!mciSendString(command, retstr, MAX_COMMAND, NULL)) {
        sscanf(retstr, "%d", &ret);
        ret /= 1000; // mSec to Sec
    }
    log("%08.8d: mci_length path=%s len=%d\n", GetTickCount(), path, ret);
    return ret;
}

static int flc_length(const char *path) {
    int ret;
    pFlac = drflac_open_file(path);
    // Failed to open FLAC file
    if (pFlac == NULL) return 0;
    drflac_uint64 length = pFlac->totalSampleCount / (pFlac->channels * pFlac->sampleRate);
    ret = (int)length;
    log("%08.8d: flc_length path=%s len=%d\n", GetTickCount(), path, ret);
    log("> samplecount=%d\n", pFlac->totalSampleCount);
    log("> length[sec]=%d\n", length);
    drflac_close(pFlac);
    pFlac = NULL;
    return ret;
}

static int wav_length(const char *path) {
    int ret;
    pWav = drwav_open_file(path);
    // Failed to open WAV file
    if (pWav == NULL) return 0;
    log("> totalsamplecount=%d\n", pWav->totalSampleCount);
    log("> channels=%d\n", pWav->channels);
    log("> samplerate=%d\n", pWav->sampleRate);
    log("> samplecount=%d\n", pWav->totalSampleCount);
    drwav_uint64 length = pWav->totalSampleCount / (pWav->channels * pWav->sampleRate);
    ret = (int)length;
    log("> length[sec]=%d\n", length);
    log("%08.8d: wav_length path=%s len=%d\n", GetTickCount(), path, ret);
    drwav_close(pWav);
    pWav = NULL;
    return ret;
}

static int mp3_length(const char *path) {
    int ret;
    drmp3_uint64 TotalFrameCount;
    if(!drmp3_init_file(&Mp3, path, NULL)) return 0;
    TotalFrameCount = drmp3_get_frames_and_close(&Mp3);
    log("> channels=%d\n", Mp3.channels);
    log("> samplerate=%d\n", Mp3.sampleRate);
    log("> totalframescount=%d\n", TotalFrameCount);
    //ret = (int)(TotalFrameCount / (Mp3.channels * Mp3.sampleRate));
    ret = (int)(TotalFrameCount / Mp3.sampleRate);
    log("%08.8d: mp3_length path=%s len=%d\n", GetTickCount(), path, ret);
    return ret;
}

int plr_length(const char *path, int codec) {
    int len;
    switch(codec) {
    case MCI_CODEC_DATA:
        len = dat_length(path);
        break;
    case MCI_CODEC_OGGVORBIS:
        len = ogg_length(path);
        break;
    case MCI_CODEC_MP3:
        len = mp3_length(path);
        break;
    case MCI_CODEC_FLAC:
        len = flc_length(path);
        break;
    case MCI_CODEC_WAV:
        len = wav_length(path);
        break;
    }
    return len;
}

static int ogg_play(const char *path) {
    log("%08.8d: ogg_play(%s)\n", GetTickCount(), path);
    if (ov_fopen(path, &plr_vf) != 0)
        return 0;
    vorbis_info *vi = ov_info(&plr_vf, -1);
    if (!vi) {
        ov_clear(&plr_vf);
        return 0;
    }
    plr_fmt.wFormatTag      = WAVE_FORMAT_PCM;
    plr_fmt.nChannels       = vi->channels;
    plr_fmt.nSamplesPerSec  = vi->rate;
    plr_fmt.wBitsPerSample  = 16;
    plr_fmt.nBlockAlign     = plr_fmt.nChannels * (plr_fmt.wBitsPerSample / 8);
    plr_fmt.nAvgBytesPerSec = plr_fmt.nBlockAlign * plr_fmt.nSamplesPerSec;
    plr_fmt.cbSize          = 0;
    log("> wFormatTag=%d\n", plr_fmt.wFormatTag);
    log("> nChannels=%d\n", plr_fmt.nChannels);
    log("> nSamplesPerSec=%d\n", plr_fmt.nSamplesPerSec);
    log("> wBitsPerSample=%d\n", plr_fmt.wBitsPerSample);
    log("> nBlockAlign=%d\n", plr_fmt.nBlockAlign);
    log("> nAvgBytesPerSec=%d\n", plr_fmt.nAvgBytesPerSec);
    log("> cbSize=%d\n", plr_fmt.cbSize);
    plr_ev = CreateEvent(NULL, 0, 1, NULL);
    if (waveOutOpen(&plr_hwo, WAVE_MAPPER, &plr_fmt, (DWORD_PTR)plr_ev, 0, CALLBACK_EVENT) != MMSYSERR_NOERROR)
        return 0;
    return 1;
}

static int flc_play(const char *path) {
    MMRESULT res;
    log("%08.8d: flc_play(%s)\n", GetTickCount(), path);
    nTrackFrames = 0;
    pFlac = drflac_open_file(path);
    if (pFlac == NULL) {
        // Failed to open FLAC file
        return 0;
    }
    // beware: dr_flac always produces 32 bit samples no matter what the original encoding might be!
    plr_fmt.wFormatTag      = WAVE_FORMAT_PCM;
    plr_fmt.nChannels       = pFlac->channels;
    plr_fmt.nSamplesPerSec  = pFlac->sampleRate;
    //plr_fmt.wBitsPerSample  = 32; // always 32!
    plr_fmt.wBitsPerSample  = pFlac->bitsPerSample;
    //plr_fmt.nBlockAlign     = pFlac->channels * (32 / 8); // always 32!
    plr_fmt.nBlockAlign     = pFlac->channels * (pFlac->bitsPerSample / 8);
    plr_fmt.nAvgBytesPerSec = (pFlac->bitsPerSample / 8) * pFlac->sampleRate;
    plr_fmt.cbSize          = 0;
    drflac_uint64 length = pFlac->totalSampleCount / (pFlac->channels * pFlac->sampleRate);
    log("> samplecount=%d\n", pFlac->totalSampleCount);
    log("> length[sec]=%d\n", length);
    log("> wFormatTag=%d\n", plr_fmt.wFormatTag);
    log("> nChannels=%d\n", plr_fmt.nChannels);
    log("> nSamplesPerSec=%d\n", plr_fmt.nSamplesPerSec);
    log("> wBitsPerSample=%d\n", plr_fmt.wBitsPerSample);
    log("> nBlockAlign=%d\n", plr_fmt.nBlockAlign);
    log("> nAvgBytesPerSec=%d\n", plr_fmt.nAvgBytesPerSec);
    log("> cbSize=%d\n", plr_fmt.cbSize);
    plr_ev = CreateEvent(NULL, 0, 1, NULL);
    if ((res = waveOutOpen(&plr_hwo, WAVE_MAPPER, &plr_fmt, (DWORD_PTR)plr_ev, 0, CALLBACK_EVENT)) != MMSYSERR_NOERROR) {
        log("%08.8d: flc_play waveOutOpen error %d\n", GetTickCount(), res);
        return 0;
    }
    return 1;
}

static int wav_play(const char *path) {
    MMRESULT res;
    log("%08.8d: wav_play(%s)\n", GetTickCount(), path);
    nTrackFrames = 0;
    pWav = drwav_open_file(path);
    // Failed to open FLAC file
    if (pWav == NULL) return 0;
    // beware: dr_flac always produces 32 bit samples no matter what the original encoding might be!
    plr_fmt.wFormatTag      = WAVE_FORMAT_PCM;
    plr_fmt.nChannels       = pWav->channels;
    plr_fmt.nSamplesPerSec  = pWav->sampleRate;
    //plr_fmt.wBitsPerSample  = 32; // always 32!
    plr_fmt.wBitsPerSample  = pWav->bitsPerSample;
    //plr_fmt.nBlockAlign     = pWav->channels * (32 / 8); // always 32!
    plr_fmt.nBlockAlign     = pWav->channels * (pWav->bitsPerSample / 8);
    plr_fmt.nAvgBytesPerSec = (pWav->bitsPerSample / 8) * pWav->sampleRate;
    plr_fmt.cbSize          = 0;
    log("> samplecount=%d\n", pWav->totalSampleCount);
    log("> length[sec]=%d\n", pWav->totalSampleCount / (pWav->channels * pWav->sampleRate));
    log("> wFormatTag=%d\n", plr_fmt.wFormatTag);
    log("> nChannels=%d\n", plr_fmt.nChannels);
    log("> nSamplesPerSec=%d\n", plr_fmt.nSamplesPerSec);
    log("> wBitsPerSample=%d\n", plr_fmt.wBitsPerSample);
    log("> nBlockAlign=%d\n", plr_fmt.nBlockAlign);
    log("> nAvgBytesPerSec=%d\n", plr_fmt.nAvgBytesPerSec);
    log("> cbSize=%d\n", plr_fmt.cbSize);
    plr_ev = CreateEvent(NULL, 0, 1, NULL);
    if ((res = waveOutOpen(&plr_hwo, WAVE_MAPPER, &plr_fmt, (DWORD_PTR)plr_ev, 0, CALLBACK_EVENT)) != MMSYSERR_NOERROR) {
        log("%08.8d: wav_play waveOutOpen error %d\n", GetTickCount(), res);
        return 0;
    }
    return 1;
}

#ifdef DXWASYNCPLAY
static DWORD WINAPI mp3_play_thread(LPVOID lpParameter) {
    char command[MAX_COMMAND + 1];
    char retstr[MAX_COMMAND + 1];
    log("%08.8d: mp3_play_thread\n", GetTickCount());
    mciSendString("close mp3", retstr, MAX_COMMAND, NULL); // may fail, we don't care ...
    sprintf(command, "open %s type mpegvideo alias mp3", (const char *)lpParameter);
    if(mciSendString(command, retstr, MAX_COMMAND, NULL)) return 1;
    sprintf(command, "setaudio mp3 volume to %d", 10 * plr_vol);
    if(mciSendString(command, retstr, MAX_COMMAND, NULL)) return 1; // set volume to current value
    // wait until track finished or killed by a mp3_stop
    mciSendString("play mp3 wait", retstr, MAX_COMMAND, NULL);
    gMP3Playing = 0;
    return 0;
}
#endif // DXWASYNCPLAY

static int mp3_play(const char *path) {
    MMRESULT res;
    log("%08.8d: mp3_play(%s)\n", GetTickCount(), path);
    nTrackFrames = 0;
    if(!drmp3_init_file(&Mp3, path, NULL)) return 0;
    log("> channels=%d\n", Mp3.channels);
    log("> samplerate=%d\n", Mp3.sampleRate);
    log("> framesConsumed=%d\n", Mp3.framesConsumed);
    log("> framesRemaining=%d\n", Mp3.framesRemaining);
    log("> dataCapacity=%d\n", Mp3.dataCapacity);
    log("> sizeof(float)=%d\n", sizeof(float));
    // beware: dr_flac always produces 16 bit (SHORT) mp3 PCM samples
    plr_fmt.wFormatTag      = WAVE_FORMAT_PCM;
    plr_fmt.nChannels       = Mp3.channels;
    plr_fmt.nSamplesPerSec  = Mp3.sampleRate;
    plr_fmt.wBitsPerSample  = 8 * sizeof(SHORT);
    plr_fmt.nBlockAlign     = sizeof(SHORT) * Mp3.channels;
    plr_fmt.nAvgBytesPerSec = plr_fmt.nBlockAlign * plr_fmt.nSamplesPerSec;
    plr_fmt.cbSize          = 0;
    log("> wFormatTag=%d\n", plr_fmt.wFormatTag);
    log("> nChannels=%d\n", plr_fmt.nChannels);
    log("> nSamplesPerSec=%d\n", plr_fmt.nSamplesPerSec);
    log("> wBitsPerSample=%d\n", plr_fmt.wBitsPerSample);
    log("> nBlockAlign=%d\n", plr_fmt.nBlockAlign);
    log("> nAvgBytesPerSec=%d\n", plr_fmt.nAvgBytesPerSec);
    log("> cbSize=%d\n", plr_fmt.cbSize);
    plr_ev = CreateEvent(NULL, 0, 1, NULL);
    if ((res = waveOutOpen(&plr_hwo, WAVE_MAPPER, &plr_fmt, (DWORD_PTR)plr_ev, 0, CALLBACK_EVENT)) != MMSYSERR_NOERROR) {
        log("%08.8d: mp3_play waveOutOpen error %d\n", GetTickCount(), res);
        return 0;
    }
    return 1;
}

//static int mci_play(int codec, const char *path, int len)
//{
//	int ret;
//	log("%08.8d: mci_play(%s)\n", GetTickCount(), path);
//#ifdef DXWASYNCPLAY
//	if(mp3Handler) TerminateThread(mp3Handler, 0);
//	mp3Handler = CreateThread(NULL, 0, mp3_play_thread, (LPVOID)path, 0, NULL);
//#else
//	char command[MAX_COMMAND+1];
//	char retstr[MAX_COMMAND+1];
//	mciSendString("stop mp3", retstr, MAX_COMMAND, NULL); // may fail, we don't care ...
//	mciSendString("close mp3", retstr, MAX_COMMAND, NULL); // may fail, we don't care ...
//	switch(codec) {
//		case MCI_CODEC_WAV:
//			// huh's trick: type mpegvideo allow for volume changes on .wav files
//			sprintf(command, "open %s type mpegvideo alias mp3", path);
//			break;
//		default:
//			sprintf(command, "open %s alias mp3", path);
//			break;
//	}
//	if(ret = mciSendString(command, retstr, MAX_COMMAND, NULL)) { // open track
//		log("%08.8d: mci_play - open error %d\n", GetTickCount(), ret);
//		return 0;
//	}
//	sprintf(command, "setaudio mp3 volume to %d", 10 * plr_vol);
//	if(ret = mciSendString(command, retstr, MAX_COMMAND, NULL)) { // set volume to current value
//		log("%08.8d: mci_play - setaudio volume error %d\n", GetTickCount(), ret); // play track
//		// do not abort operation on error here ...
//	}
//	if(mciSendString("play mp3", retstr, MAX_COMMAND, NULL)) {
//		log("%08.8d: mci_play - play error %d\n", GetTickCount(), ret); // play track
//		return 0;
//	}
//#endif // DXWASYNCPLAY
//	gMP3Playing = 1;
//	mp3_begin_timer = GetTickCount();
//	mp3_end_timer = mp3_begin_timer + (1000 * len);
//    return 1;
//}

int plr_play(int track) {
    int ret = 1;
    char path[MAX_PATH];
    plr_stop();
    gPlayer = codecs[track];
    switch(gPlayer) {
    case MCI_CODEC_OGGVORBIS:
        sprintf(path, "%s\\Music%s\\Track%02d.ogg", sRootPath, folderSuffix, track);
        ret = ogg_play(path);
        break;
    case MCI_CODEC_MP3:
        sprintf(path, "%s\\Music%s\\Track%02d.mp3", sRootPath, folderSuffix, track);
        ret = mp3_play(path);
        break;
    case MCI_CODEC_WAV:
        sprintf(path, "%s\\Music%s\\Track%02d.wav", sRootPath, folderSuffix, track);
        ret = wav_play(path);
        break;
    case MCI_CODEC_FLAC:
        sprintf(path, "%s\\Music%s\\Track%02d.flac", sRootPath, folderSuffix, track);
        ret = flc_play(path);
        break;
    }
    return ret;
}

int ogg_pump() {
    log("%08.8d: ogg_pump\n", GetTickCount());
    if (!plr_vf.datasource)
        return 0;
    int pos = 0;
    int bufsize = plr_fmt.nAvgBytesPerSec / 4; // 250ms (avg at 500ms) should be enough for everyone
    char *buf = (char *)malloc(bufsize);
    while (pos < bufsize) {
        long bytes = ov_read(&plr_vf, buf + pos, bufsize - pos, 0, 2, 1, NULL);
        if (bytes == OV_HOLE) {
            free(buf);
            continue;
        }
        if (bytes == OV_EBADLINK) {
            free(buf);
            return 0;
        }
        if (bytes == OV_EINVAL) {
            free(buf);
            //free(buf);
            return 0;
        }
        if (bytes == 0) {
            free(buf);
            int i, in_queue = 0;
            for (i = 0; i < 3; i++) {
                if (plr_buffers[i] && plr_buffers[i]->dwFlags & WHDR_DONE) {
                    waveOutUnprepareHeader(plr_hwo, plr_buffers[i], sizeof(WAVEHDR));
                    free(plr_buffers[i]->lpData);
                    free(plr_buffers[i]);
                    plr_buffers[i] = NULL;
                }
                if (plr_buffers[i])
                    in_queue++;
            }
            Sleep(100);
            log("%08.8d: ogg in_queue=%d\n", GetTickCount(), in_queue);
            return !(in_queue == 0);
        }
        pos += bytes;
    }
    // volume control, kinda nasty
    if(plr_vol != 100) { // v2.04.98: skip for 100% volume
        int x, end = pos / 2;
        short *sbuf = (short *)buf;
        for (x = 0; x < end; x++)
            sbuf[x] = (short)(sbuf[x] * (plr_vol / 100.0f));
    }
    WAVEHDR *header = (WAVEHDR *)malloc(sizeof(WAVEHDR));
    header->dwBufferLength   = pos;
    header->lpData           = buf;
    header->dwUser           = 0;
    header->dwFlags          = plr_cnt == 0 ? WHDR_BEGINLOOP : 0;
    header->dwLoops          = 0;
    header->lpNext           = NULL;
    header->reserved         = 0;
    log("> dwBufferLength=%d\n", header->dwBufferLength );
    log("> lpData=%x\n", header->lpData );
    log("> dwUser=%x\n", header->dwUser );
    log("> dwFlags=%x\n", header->dwFlags );
    log("> dwLoops=%d\n", header->dwLoops );
    log("> lpNext=%x\n", header->lpNext );
    log("> reserved=%d\n", header->reserved );
    waveOutPrepareHeader(plr_hwo, header, sizeof(WAVEHDR));
    if (plr_cnt > 1)
        WaitForSingleObject(plr_ev, INFINITE);
    int i, queued = 0;
    for (i = 0; i < 3; i++) {
        if (plr_buffers[i] && plr_buffers[i]->dwFlags & WHDR_DONE) {
            waveOutUnprepareHeader(plr_hwo, plr_buffers[i], sizeof(WAVEHDR));
            free(plr_buffers[i]->lpData);
            free(plr_buffers[i]);
            plr_buffers[i] = NULL;
        }
        if (!queued && plr_buffers[i] == NULL) {
            waveOutWrite(plr_hwo, header, sizeof(WAVEHDR));
            plr_buffers[i] = header;
            queued = 1;
        }
    }
    log("%08.8d: ogg_pump queued=%d plr_cnt=%d\n", GetTickCount(), queued, plr_cnt);
    if (!queued) {
        free(header);
        free(buf);
    }
    plr_cnt++;
    return 1;
}

int flc_pump() {
    MMRESULT res;
    drflac_uint64 bytes;
    log("%08.8d: flc_pump\n", GetTickCount());
    void *buf = NULL;
    int pos;
    if(!pFlac) return 0; // safeguard condition
    switch(pFlac->bitsPerSample) {
    case 16:
        log("%08.8d: flc_pump\n", GetTickCount());
        pos = (size_t)flac_buffers * sizeof(drflac_int16);
        buf = malloc(pos);
        bytes = drflac_read_s16(pFlac, flac_buffers, (drwav_int16 *)buf);
        break;
    case 32:
        pos = (size_t)flac_buffers * sizeof(drflac_int32);
        buf = malloc(pos);
        bytes = drflac_read_s32(pFlac, flac_buffers, (drwav_int32 *)buf);
        break;
    default:
        break;
    }
    nTrackFrames += flac_buffers;
    log("%08.8d: drflac_read_s32 read bytes=%d\n", GetTickCount(), bytes);
    if (bytes == 0) {
        if(buf) {
            log("%08.8d: flc_pump free last buffer\n", GetTickCount());
            free(buf);
        }
        int i, in_queue = 0;
        for (i = 0; i < 3; i++) {
            if (plr_buffers[i] && plr_buffers[i]->dwFlags & WHDR_DONE) {
                waveOutUnprepareHeader(plr_hwo, plr_buffers[i], sizeof(WAVEHDR));
                free(plr_buffers[i]->lpData);
                free(plr_buffers[i]);
                plr_buffers[i] = NULL;
            }
            if (plr_buffers[i])
                in_queue++;
        }
        // v2.05.20: fixed flac behavior at end of track causing crash
        log("%08.8d: flac in_queue=%d\n", GetTickCount(), in_queue);
        if(in_queue == 0) {
            drflac_close(pFlac);
            pFlac = NULL;
        } else Sleep(100);
        return !(in_queue == 0);
    }
    // volume control, kinda nasty
    if(plr_vol != 100) { // v2.04.98: skip for 100% volume
        int x, end = pos / 2;
        short *sbuf = (short *)buf;
        for (x = 0; x < end; x++)
            sbuf[x] = (short)(sbuf[x] * (plr_vol / 100.0f));
    }
    WAVEHDR *header = (WAVEHDR *)malloc(sizeof(WAVEHDR));
    header->dwBufferLength   = pos;
    header->lpData           = (LPSTR)buf;
    header->dwUser           = 0;
    header->dwFlags          = plr_cnt == 0 ? WHDR_BEGINLOOP : 0;
    header->dwLoops          = 0;
    header->lpNext           = NULL;
    header->reserved         = 0;
    log("> dwBufferLength=%d\n", header->dwBufferLength );
    log("> lpData=%x\n", header->lpData );
    log("> dwUser=%x\n", header->dwUser );
    log("> dwFlags=%x\n", header->dwFlags );
    log("> dwLoops=%d\n", header->dwLoops );
    log("> lpNext=%x\n", header->lpNext );
    log("> reserved=%d\n", header->reserved );
    if(res = waveOutPrepareHeader(plr_hwo, header, sizeof(WAVEHDR))) {
        log("%08.8d: flc_pump waveOutPrepareHeader error %d\n", GetTickCount(), res);
        return 0;
    }
    if (plr_cnt > 1)
        WaitForSingleObject(plr_ev, INFINITE);
    int i, queued = 0;
    for (i = 0; i < 3; i++) {
        if (plr_buffers[i] && plr_buffers[i]->dwFlags & WHDR_DONE) {
            if(res = waveOutUnprepareHeader(plr_hwo, plr_buffers[i], sizeof(WAVEHDR))) {
                log("%08.8d: flc_pump waveOutUnprepareHeader error %d\n", GetTickCount(), res);
                return 0;
            }
            free(plr_buffers[i]->lpData);
            free(plr_buffers[i]);
            plr_buffers[i] = NULL;
        }
        if (!queued && plr_buffers[i] == NULL) {
#ifdef PLAYDEBUG
            if(res = waveOutWrite(plr_hwo, header, sizeof(WAVEHDR)))
                log("%08.8d: flc_pump waveOutWrite error %d\n", GetTickCount(), res);
            else
                log("%08.8d: flc_pump waveOutWrite buffer=%d ok\n", GetTickCount(), i);
#else
            waveOutWrite(plr_hwo, header, sizeof(WAVEHDR));
#endif // PLAYDEBUG
            plr_buffers[i] = header;
            queued = 1;
        }
    }
    log("%08.8d: flc_pump queued=%d plr_cnt=%d\n", GetTickCount(), queued, plr_cnt);
    if (!queued) {
        free(header);
        free(buf);
    }
    plr_cnt++;
    return 1;
}

int wav_pump() {
    MMRESULT res;
    drwav_uint64 bytes;
    log("%08.8d: wav_pump\n", GetTickCount());
    void *buf;
    int pos;
    bytes = 0;
    buf = NULL;
    if(pWav) { // safeguard condition
        switch(pWav->bitsPerSample) {
        case 16:
            pos = (size_t)wav_buffers * sizeof(drwav_int16);
            buf = malloc(pos);
            bytes = drwav_read_s16(pWav, wav_buffers, (drwav_int16 *)buf);
            break;
        case 32:
            pos = (size_t)wav_buffers * sizeof(drwav_int32);
            buf = malloc(pos);
            bytes = drwav_read_s32(pWav, wav_buffers, (drwav_int32 *)buf);
            break;
        default:
            break;
        }
        //nTrackFrames += flac_buffers;
        nTrackFrames += (int)bytes;
    }
    log("%08.8d: drwav_read_s32 read bytes=%d trackframes=%d\n", GetTickCount(), bytes, nTrackFrames);
    if (bytes == 0) {
        if(buf) free(buf);
        int i, in_queue = 0;
        for (i = 0; i < 3; i++) {
            if (plr_buffers[i] && plr_buffers[i]->dwFlags & WHDR_DONE) {
                waveOutUnprepareHeader(plr_hwo, plr_buffers[i], sizeof(WAVEHDR));
                free(plr_buffers[i]->lpData);
                free(plr_buffers[i]);
                plr_buffers[i] = NULL;
            }
            if (plr_buffers[i])
                in_queue++;
        }
        log("%08.8d: wav in_queue=%d\n", GetTickCount(), in_queue);
        // v1.01: replicated fix for flac format here
        if(in_queue == 0) {
            drwav_close(pWav);
            pWav = NULL;
        } else Sleep(200);
        return !(in_queue == 0);
    }
    // volume control, kinda nasty
    if(plr_vol != 100) { // v2.04.98: skip for 100% volume
        int x, end = pos / 2;
        short *sbuf = (short *)buf;
        for (x = 0; x < end; x++)
            sbuf[x] = (short)(sbuf[x] * (plr_vol / 100.0f));
    }
    WAVEHDR *header = (WAVEHDR *)malloc(sizeof(WAVEHDR));
    header->dwBufferLength   = pos;
    header->lpData           = (LPSTR)buf;
    header->dwUser           = 0;
    header->dwFlags          = plr_cnt == 0 ? WHDR_BEGINLOOP : 0;
    header->dwLoops          = 0;
    header->lpNext           = NULL;
    header->reserved         = 0;
    log("> dwBufferLength=%d\n", header->dwBufferLength );
    log("> lpData=%x\n", header->lpData );
    log("> dwUser=%x\n", header->dwUser );
    log("> dwFlags=%x\n", header->dwFlags );
    log("> dwLoops=%d\n", header->dwLoops );
    log("> lpNext=%x\n", header->lpNext );
    log("> reserved=%d\n", header->reserved );
    if(res = waveOutPrepareHeader(plr_hwo, header, sizeof(WAVEHDR))) {
        log("%08.8d: wav_pump waveOutPrepareHeader error %d\n", GetTickCount(), res);
        return 0;
    }
    if (plr_cnt > 1)
        WaitForSingleObject(plr_ev, INFINITE);
    int i, queued = 0;
    for (i = 0; i < 3; i++) {
        if (plr_buffers[i] && plr_buffers[i]->dwFlags & WHDR_DONE) {
            if(res = waveOutUnprepareHeader(plr_hwo, plr_buffers[i], sizeof(WAVEHDR))) {
                log("%08.8d: wav_pump waveOutUnprepareHeader error %d\n", GetTickCount(), res);
                return 0;
            }
            free(plr_buffers[i]->lpData);
            free(plr_buffers[i]);
            plr_buffers[i] = NULL;
        }
        if (!queued && plr_buffers[i] == NULL) {
#ifdef PLAYDEBUG
            if(res = waveOutWrite(plr_hwo, header, sizeof(WAVEHDR)))
                log("%08.8d: wav_pump waveOutWrite error %d\n", GetTickCount(), res);
            else
                log("%08.8d: wav_pump waveOutWrite buffer=%d ok\n", GetTickCount(), i);
#else
            waveOutWrite(plr_hwo, header, sizeof(WAVEHDR));
#endif // PLAYDEBUG
            plr_buffers[i] = header;
            queued = 1;
        }
    }
    log("%08.8d: wav_pump queued=%d plr_cnt=%d\n", GetTickCount(), queued, plr_cnt);
    if (!queued) {
        free(header);
        free(buf);
    }
    plr_cnt++;
    return 1;
}

int mp3_pump() {
    MMRESULT res;
    drmp3_uint64 nFramesRead;
    log("%08.8d: mp3_pump\n", GetTickCount());
    float *buf = (float *)malloc(mp3_frames2read * Mp3.channels * sizeof(float));
    nFramesRead = drmp3_read_f32(&Mp3, mp3_frames2read, buf);
    nTrackFrames += (int)nFramesRead;
    log("%08.8d: drmp3_read_s32 read frames=%d\n", GetTickCount(), nFramesRead);
    if (nFramesRead == 0) {
        int i, in_queue = 0;
        for (i = 0; i < 3; i++) {
            if (plr_buffers[i] && plr_buffers[i]->dwFlags & WHDR_DONE) {
                waveOutUnprepareHeader(plr_hwo, plr_buffers[i], sizeof(WAVEHDR));
                free(plr_buffers[i]->lpData);
                free(plr_buffers[i]);
                plr_buffers[i] = NULL;
            }
            if (plr_buffers[i])
                in_queue++;
        }
        drmp3_free(&Mp3);
        Sleep(100);
        log("%08.8d: mp3 in_queue=%d\n", GetTickCount(), in_queue);
        return !(in_queue == 0);
    }
    drmp3_int16 *bufs = (drmp3_int16 *)malloc(mp3_frames2read * Mp3.channels * sizeof(SHORT));
    drmp3dec_f32_to_s16(buf, bufs, (int)nFramesRead * Mp3.channels);
    free(buf);
    // volume control, kinda nasty
    if(plr_vol < 100) { // v2.04.98: skip for 99-100% volume
        int x;
        drmp3_uint64 end = nFramesRead * Mp3.channels;
        for (x = 0; x < end; x++)
            bufs[x] = (short)(bufs[x] * (plr_vol / 100.0f));
    }
    WAVEHDR *header = (WAVEHDR *)malloc(sizeof(WAVEHDR));
    header->dwBufferLength   = mp3_frames2read * Mp3.channels * sizeof(SHORT);
    header->lpData           = (LPSTR)bufs;
    header->dwUser           = 0;
    header->dwFlags          = plr_cnt == 0 ? WHDR_BEGINLOOP : 0;
    header->dwLoops          = 0;
    header->lpNext           = NULL;
    header->reserved         = 0;
    log("> dwBufferLength=%d\n", header->dwBufferLength );
    log("> lpData=%x\n", header->lpData );
    log("> dwUser=%x\n", header->dwUser );
    log("> dwFlags=%x\n", header->dwFlags );
    log("> dwLoops=%d\n", header->dwLoops );
    log("> lpNext=%x\n", header->lpNext );
    log("> reserved=%d\n", header->reserved );
    if(res = waveOutPrepareHeader(plr_hwo, header, sizeof(WAVEHDR))) {
        log("%08.8d: mp3_pump waveOutPrepareHeader error %d\n", GetTickCount(), res);
        free(bufs);
        return 0;
    }
    if (plr_cnt > 1)
        WaitForSingleObject(plr_ev, INFINITE);
    int i, queued = 0;
    for (i = 0; i < 3; i++) {
        if (plr_buffers[i] && plr_buffers[i]->dwFlags & WHDR_DONE) {
            if(res = waveOutUnprepareHeader(plr_hwo, plr_buffers[i], sizeof(WAVEHDR))) {
                log("%08.8d: mp3_pump waveOutUnprepareHeader error %d\n", GetTickCount(), res);
                return 0;
            }
            free(plr_buffers[i]->lpData);
            free(plr_buffers[i]);
            plr_buffers[i] = NULL;
        }
        if (!queued && plr_buffers[i] == NULL) {
#ifdef PLAYDEBUG
            if(res = waveOutWrite(plr_hwo, header, sizeof(WAVEHDR)))
                log("%08.8d: mp3_pump waveOutWrite error %d\n", GetTickCount(), res);
            else
                log("%08.8d: mp3_pump waveOutWrite buffer=%d ok\n", GetTickCount(), i);
#else
            waveOutWrite(plr_hwo, header, sizeof(WAVEHDR));
#endif // PLAYDEBUG
            plr_buffers[i] = header;
            queued = 1;
        }
    }
    log("%08.8d: mp3_pump queued=%d plr_cnt=%d\n", GetTickCount(), queued, plr_cnt);
    if (!queued)
        free(header);
    plr_cnt++;
    return 1;
}

int plr_pump() {
    int ret = 1;
    switch(gPlayer) {
    case MCI_CODEC_OGGVORBIS:
        ret = ogg_pump();
        break;
    case MCI_CODEC_MP3:
        ret = mp3_pump();
        break;
    case MCI_CODEC_FLAC:
        ret = flc_pump();
        break;
    case MCI_CODEC_WAV:
        ret = wav_pump();
        break;
    }
    return ret;
}

int ogg_seek(int sec) {
    log("%08.8d: ogg_seek(%d)\n", GetTickCount(), sec);
    //if(!plr_vf) return 0;
    int len = (int)ov_time_total(&plr_vf, -1);
    if(sec < 0) sec = 0;
    if(sec > len) sec = len;
    return ov_time_seek(&plr_vf, (double)sec);
}

int mp3_seek(int sec) {
    log("%08.8d: mp3_seek(%d)\n", GetTickCount(), sec);
    // to do ...
    return 0;
}

int flc_seek(int sec) {
    int ret = 0;
    log("%08.8d: flc_seek(%d)\n", GetTickCount(), sec);
    if(pFlac) {
        drflac_uint64 sampleIndex = sec * pFlac->sampleRate;
        drflac_seek_to_sample(pFlac, sampleIndex);
        ret = sec;
    }
    nTrackFrames = ret * (pFlac->sampleRate * pFlac->channels);
    return ret;
}

int wav_seek(int sec) {
    int ret = 0;
    log("%08.8d: wav_seek(%d)\n", GetTickCount(), sec);
    if(pWav) {
        drwav_uint64 sampleIndex = sec * pWav->sampleRate;
        drwav_seek_to_sample(pWav, sampleIndex);
        ret = sec;
    }
    nTrackFrames = ret * (pWav->sampleRate * pWav->channels);
    return ret;
}

int plr_seek(int sec) {
    int ret = 0;
    switch(gPlayer) {
    case MCI_CODEC_OGGVORBIS:
        ret = ogg_seek(sec);
        break;
    case MCI_CODEC_MP3:
        ret = mp3_seek(sec);
        break;
    case MCI_CODEC_FLAC:
        ret = flc_seek(sec);
        break;
    case MCI_CODEC_WAV:
        ret = wav_seek(sec);
        break;
    }
    return ret;
}

static int ogg_tell() {
    log("%08.8d: ogg_tell\n", GetTickCount());
    //if(!plr_vf) return 0;
    int tpos = (int)ov_time_tell(&plr_vf);
    return tpos;
}

static int mp3_tell() {
    int ret = 0;
    ret = nTrackFrames / Mp3.sampleRate;
    log("%08.8d: mp3_tell -> %d\n", GetTickCount(), ret);
    return ret;
}

static int flc_tell() {
    int ret = 0;
    if(pFlac) ret = nTrackFrames / (pFlac->sampleRate * pFlac->channels);
    log("%08.8d: flc_tell -> %d\n", GetTickCount(), ret);
    return ret;
}

static int wav_tell() {
    int ret = 0;
    if(pWav) ret = nTrackFrames / (pWav->sampleRate * pWav->channels);
    log("%08.8d: wav_tell -> %d\n", GetTickCount(), ret);
    return ret;
}

int plr_tell() {
    int ret = 0;
    switch(gPlayer) {
    case MCI_CODEC_OGGVORBIS:
        ret = ogg_tell();
        break;
    case MCI_CODEC_MP3:
        ret = mp3_tell();
        break;
    case MCI_CODEC_FLAC:
        ret = flc_tell();
        break;
    case MCI_CODEC_WAV:
        ret = wav_tell();
        break;
    }
    return ret;
}

int plr_load(track_info *tracks, int max) {
    int last_track = 0;
    int position = 0;
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule != NULL) {
        char *p;
        char *pLastSlash;
        // Use GetModuleFileName() with module handle to get the path
        GetModuleFileName(hModule, sRootPath, MAX_PATH);
        p = sRootPath;
        while (p = strchr(p, '\\')) {
            pLastSlash = p;
            p = p + 1;
        }
        *pLastSlash = (char)0;
    }
    log("%08.8d: plr_load\n", GetTickCount());
    // initialize
    if(codecs) free(codecs);
    codecs = (int *)malloc((max + 1) * sizeof(int));
    memset(codecs, 0, (max + 1)*sizeof(int));
    for (int i = 0; i < max; i++) memset(&tracks[i], 0, sizeof(track_info));
#ifdef CACHETRACKINFO
    char sCachePath[MAX_PATH];
    sprintf(sCachePath, "%s\\Music%s\\tracklen.nfo", sRootPath, folderSuffix);
    FILE *fpTrackInfo = fopen(sCachePath, "r");
    if(fpTrackInfo) {
        log("%08.8d: cache found, loading ...\n", GetTickCount());
        while (!feof(fpTrackInfo)) {
            int iTrackNo, iTrackLen, iTrackPos;
            char cTrackType;
            track_info *pTrack;
            if(fscanf(fpTrackInfo, "track=%d type=%c pos=%d len=%d\n", &iTrackNo, &cTrackType, &iTrackPos, &iTrackLen) != 4) break;
            log("%08.8d: cache get track=%d type=%c pos=%d len=%d\n", GetTickCount(), iTrackNo, cTrackType, iTrackPos, iTrackLen);
            if((iTrackNo < 1) || (iTrackNo >= max)) break;
            pTrack = &tracks[iTrackNo];
            pTrack->length = iTrackLen;
            pTrack->position = iTrackPos;
            switch(cTrackType) {
            case 'w':
                pTrack->type = MCI_AUDIO_TRACK;
                codecs[iTrackNo] = MCI_CODEC_WAV;
                break;
            case 'o':
                pTrack->type = MCI_AUDIO_TRACK;
                codecs[iTrackNo] = MCI_CODEC_OGGVORBIS;
                break;
            case 'm':
                pTrack->type = MCI_AUDIO_TRACK;
                codecs[iTrackNo] = MCI_CODEC_MP3;
                break;
            case 'f':
                pTrack->type = MCI_AUDIO_TRACK;
                codecs[iTrackNo] = MCI_CODEC_FLAC;
                break;
            case 'd':
                pTrack->type = MCI_DATA_TRACK;
                codecs[iTrackNo] = MCI_CODEC_DATA;
                break;
            }
            if(iTrackNo > last_track) last_track = iTrackNo;
        }
        fclose(fpTrackInfo);
        gTracks = tracks; // save the pointer
        log("%08.8d: plr_load returns last_track=%d\n", GetTickCount(), last_track);
        return last_track;
    }
#endif
    // skip track 0, it is a placeholder
    for (int i = 1; i < max; i++) {
        char path[MAX_PATH];
        FILE *fp;
        tracks[i].length = 0;
        for(int codec = MCI_CODEC_FIRST; codec <= MCI_CODEC_LAST; codec++) {
            char *ext;
            switch(codec) {
            case MCI_CODEC_DATA:
                ext = "dat";
                break;
            case MCI_CODEC_OGGVORBIS:
                ext = "ogg";
                break;
            case MCI_CODEC_MP3:
                ext = "mp3";
                break;
            case MCI_CODEC_WAV:
                ext = "wav";
                break;
            case MCI_CODEC_FLAC:
                ext = "flac";
                break;
            }
            _snprintf_s(path, sizeof path, 80, "%s\\Music%s\\Track%02d.%s", sRootPath, folderSuffix, i, ext);
            fp = fopen(path, "r");
            if(fp) {
                log("%08.8d: found codec=%d path=%s \n", GetTickCount(), codec, path);
                fclose(fp);
                codecs[i] = codec;
                tracks[i].length = plr_length(path, codec);
                break;
            }
        }
        if (tracks[i].length < 4) {
            tracks[i].type = MCI_DATA_TRACK;
            tracks[i].length = 4; // missing tracks are 4 second data tracks for us
        } else {
            last_track = i;
            tracks[i].type = codecs[i] == MCI_CODEC_DATA ? MCI_DATA_TRACK : MCI_AUDIO_TRACK;
        }
        tracks[i].position = position;
        position += tracks[i].length;
    }
#ifdef CACHETRACKINFO
    fpTrackInfo = fopen(sCachePath, "w");
    if(fpTrackInfo) {
        log("%08.8d: building cache ...\n", GetTickCount());
        for(int iTrackNo = 1; iTrackNo <= last_track; iTrackNo++) {
            char cTrackType;
            track_info *pTrack;
            pTrack = &tracks[iTrackNo];
            if(pTrack->type == MCI_DATA_TRACK) cTrackType = 'd';
            else {
                switch(codecs[iTrackNo]) {
                case MCI_CODEC_WAV:
                    cTrackType = 'w';
                    break;
                case MCI_CODEC_OGGVORBIS:
                    cTrackType = 'o';
                    break;
                case MCI_CODEC_MP3:
                    cTrackType = 'm';
                    break;
                case MCI_CODEC_FLAC:
                    cTrackType = 'f';
                    break;
                case MCI_CODEC_DATA:
                    cTrackType = 'd';
                    break;
                }
            }
            fprintf(fpTrackInfo, "track=%d type=%c pos=%d len=%d\n", iTrackNo, cTrackType, pTrack->position, pTrack->length);
            log("%08.8d: cache put track=%d type=%c pos=%d len=%d\n", GetTickCount(), iTrackNo, cTrackType, pTrack->position, pTrack->length);
        }
        fclose(fpTrackInfo);
    } else
        log("%08.8d: cache creation err=%d path=%s\n", GetTickCount(), GetLastError(), sCachePath);
#endif
    gTracks = tracks; // save the pointer
    log("%08.8d: plr_load returns last_track=%d\n", GetTickCount(), last_track);
    return last_track;
}

void plr_change(int cdindex) {
    static char sSuffix[2 + 1];
    log("%08.8d: plr_change load cd index=%d\n", GetTickCount(), cdindex);
    if(cdindex < 0) cdindex = 0;
    if(cdindex > 8) cdindex = 8;
    if(cdindex) {
        sprintf_s(sSuffix, 3, "%02d", cdindex + 1);
        folderSuffix = &sSuffix[0];
    } else
        folderSuffix = "";
    log("%08.8d: plr_change folder=\"Music%s\"\n", GetTickCount(), folderSuffix);
}