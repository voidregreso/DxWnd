

// main.cpp

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

#define LOG(format, ...) wprintf(format L"\n", __VA_ARGS__)

int main(int argc, char *argv[]) {
    UINT devs = waveInGetNumDevs();
    LOG(L"waveIn devices: %u", devs);
    for (UINT dev = 0; dev < devs; dev++) {
        WAVEINCAPS caps = {};
        MMRESULT mmr = waveInGetDevCaps(dev, &caps, sizeof(caps));
        if (MMSYSERR_NOERROR != mmr) {
            LOG(L"waveInGetDevCaps failed: mmr = 0x%08x", mmr);
            return mmr;
        }
        LOG(
            L"-- waveIn device #%u --\n"
            L"Manufacturer ID: %u\n"
            L"Product ID: %u\n"
            L"Version: %u.%u\n"
            L"Product Name: %s\n"
            L"Formats: 0x%x\n"
            L"Channels: %u\n"
            L"Reserved: %u\n"
            ,
            dev,
            caps.wMid,
            caps.wPid,
            caps.vDriverVersion / 256, caps.vDriverVersion % 256,
            caps.szPname,
            caps.dwFormats,
            caps.wChannels,
            caps.wReserved1
        );
    }
    devs = waveOutGetNumDevs();
    LOG(L"waveOut devices: %u", devs);
    for (UINT dev = 0; dev < devs; dev++) {
        WAVEOUTCAPS caps = {};
        MMRESULT mmr = waveOutGetDevCaps(dev, &caps, sizeof(caps));
        if (MMSYSERR_NOERROR != mmr) {
            LOG(L"waveOutGetDevCaps failed: mmr = 0x%08x", mmr);
            return mmr;
        }
        LOG(
            L"-- waveOut device #%u --\n"
            L"Manufacturer ID: %u\n"
            L"Product ID: %u\n"
            L"Version: %u.%u\n"
            L"Product Name: %s\n"
            L"Formats: 0x%x\n"
            L"Channels: %u\n"
            L"Reserved: %u\n"
            L"Support: 0x%x\n"
            L"%s%s%s%s%s"
            ,
            dev,
            caps.wMid,
            caps.wPid,
            caps.vDriverVersion / 256, caps.vDriverVersion % 256,
            caps.szPname,
            caps.dwFormats,
            caps.wChannels,
            caps.wReserved1,
            caps.dwSupport,
            ((caps.dwSupport & WAVECAPS_LRVOLUME) ?       L"\tWAVECAPS_LRVOLUME\n" :       L""),
            ((caps.dwSupport & WAVECAPS_PITCH) ?          L"\tWAVECAPS_PITCH\n" :          L""),
            ((caps.dwSupport & WAVECAPS_PLAYBACKRATE) ?   L"\tWAVECAPS_PLAYBACKRATE\n" :   L""),
            ((caps.dwSupport & WAVECAPS_VOLUME) ?         L"\tWAVECAPS_VOLUME\n" :         L""),
            ((caps.dwSupport & WAVECAPS_SAMPLEACCURATE) ? L"\tWAVECAPS_SAMPLEACCURATE\n" : L"")
        );
    }
    devs = midiInGetNumDevs();
    LOG(L"midiIn devices: %u", devs);
    for (UINT dev = 0; dev < devs; dev++) {
        MIDIINCAPS caps = {};
        MMRESULT mmr = midiInGetDevCaps(dev, &caps, sizeof(caps));
        if (MMSYSERR_NOERROR != mmr) {
            LOG(L"midiInGetDevCaps failed: mmr = 0x%08x", mmr);
            return mmr;
        }
        LOG(
            L"-- midiIn device #%u --\n"
            L"Manufacturer ID: %u\n"
            L"Product ID: %u\n"
            L"Version: %u.%u\n"
            L"Product Name: %s\n"
            L"Support: 0x%x\n"
            ,
            dev,
            caps.wMid,
            caps.wPid,
            caps.vDriverVersion / 256, caps.vDriverVersion % 256,
            caps.szPname,
            caps.dwSupport
        );
    }
    devs = midiOutGetNumDevs();
    LOG(L"midiOut devices: %u", devs);
    for (UINT dev = 0; dev < devs; dev++) {
        MIDIOUTCAPS caps = {};
        MMRESULT mmr = midiOutGetDevCaps(dev, &caps, sizeof(caps));
        if (MMSYSERR_NOERROR != mmr) {
            LOG(L"midiOutGetDevCaps failed: mmr = 0x%08x", mmr);
            return mmr;
        }
        LOG(
            L"-- midiOut device #%u --\n"
            L"Manufacturer ID: %u\n"
            L"Product ID: %u\n"
            L"Version: %u.%u\n"
            L"Product Name: %s\n"
            L"Technology: 0x%x\n"
            L"Voices: %u\n"
            L"Notes: %u\n"
            L"ChannelMask: 0x%x\n"
            L"Support: 0x%x\n"
            ,
            dev,
            caps.wMid,
            caps.wPid,
            caps.vDriverVersion / 256, caps.vDriverVersion % 256,
            caps.szPname,
            caps.wTechnology,
            caps.wVoices,
            caps.wNotes,
            caps.wChannelMask,
            caps.dwSupport
        );
    }
    getchar();
    return 0;
}
