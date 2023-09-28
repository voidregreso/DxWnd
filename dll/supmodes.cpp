#include "dxwnd.h"

SupportedRes_Type SupportedSVGARes[16] = {
    {320, 200},
    {320, 240},
    {400, 300},		// v2.04.77 addition
    {512, 384},		// needed by "Outcast" loading screen
    {640, 400},
    {640, 480},
    {720, 480},
    {800, 600},
    {1024, 768},		// XGA
    {1152, 864},		// v2.04.77 addition
    {1280, 800},		// WXGA
    {1280, 960},		// v2.04.77 addition
    {1280, 1024},
    {1400, 1050},	// v2.04.77 addition
    {1600, 1200},	// UXGA, needed by "LEGO Star Wars" in high res mode
    {0, 0}
};

SupportedRes_Type SupportedHDTVRes[12] = {
    {640, 360},		// nHD
    {720, 480},		// DVD
    {720, 576},		// DV-PAL
    {960, 540},		// qHD
    {1176, 1000},
    {1280, 720},		// HD
    {1440, 900},		// Swat 3 hack
    {1440, 960},
    {1600, 900},		// HD+
    {1920, 1080},	// FHD
    {0, 0}
};

// holds space for possible VGA + HDTV + Custom resolution + terminator
SupportedRes_Type SupportedRes[15 + 10 + 1 + 1];

int SupportedDepths[5] = {8, 16, 24, 32, 0};
