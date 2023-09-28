#include "ddraw.h"

char *ExplainDDError(DWORD c) {
    static char *eb;
    switch(c) {
    case DD_OK:
        eb = "DD_OK";
        break;
    case DDERR_ALREADYINITIALIZED:
        eb = "DDERR_ALREADYINITIALIZED";
        break;
    case DDERR_BLTFASTCANTCLIP:
        eb = "DDERR_BLTFASTCANTCLIP";
        break;
    case DDERR_CANNOTATTACHSURFACE:
        eb = "DDERR_CANNOTATTACHSURFACE";
        break;
    case DDERR_CANNOTDETACHSURFACE:
        eb = "DDERR_CANNOTDETACHSURFACE";
        break;
    case DDERR_CANTCREATEDC:
        eb = "DDERR_CANTCREATEDC";
        break;
    case DDERR_CANTDUPLICATE:
        eb = "DDERR_CANTDUPLICATE";
        break;
    case DDERR_CANTLOCKSURFACE:
        eb = "DDERR_CANTLOCKSURFACE";
        break;
    case DDERR_CANTPAGELOCK:
        eb = "DDERR_CANTPAGELOCK";
        break;
    case DDERR_CANTPAGEUNLOCK:
        eb = "DDERR_CANTPAGEUNLOCK";
        break;
    case DDERR_CLIPPERISUSINGHWND:
        eb = "DDERR_CLIPPERISUSINGHWND";
        break;
    case DDERR_COLORKEYNOTSET:
        eb = "DDERR_COLORKEYNOTSET";
        break;
    case DDERR_CURRENTLYNOTAVAIL:
        eb = "DDERR_CURRENTLYNOTAVAIL";
        break;
    case DDERR_DCALREADYCREATED:
        eb = "DDERR_DCALREADYCREATED";
        break;
    case DDERR_DEVICEDOESNTOWNSURFACE:
        eb = "DDERR_DEVICEDOESNTOWNSURFACE";
        break;
    case DDERR_DIRECTDRAWALREADYCREATED:
        eb = "DDERR_DIRECTDRAWALREADYCREATED";
        break;
    case DDERR_EXCEPTION:
        eb = "DDERR_EXCEPTION";
        break;
    case DDERR_EXCLUSIVEMODEALREADYSET:
        eb = "DDERR_EXCLUSIVEMODEALREADYSET";
        break;
    case DDERR_EXPIRED:
        eb = "DDERR_EXPIRED";
        break;
    case DDERR_GENERIC:
        eb = "DDERR_GENERIC";
        break;
    case DDERR_HEIGHTALIGN:
        eb = "DDERR_HEIGHTALIGN";
        break;
    case DDERR_HWNDALREADYSET:
        eb = "DDERR_HWNDALREADYSET";
        break;
    case DDERR_HWNDSUBCLASSED:
        eb = "DDERR_HWNDSUBCLASSED";
        break;
    case DDERR_IMPLICITLYCREATED:
        eb = "DDERR_IMPLICITLYCREATED";
        break;
    case DDERR_INCOMPATIBLEPRIMARY:
        eb = "DDERR_INCOMPATIBLEPRIMARY";
        break;
    case DDERR_INVALIDCAPS:
        eb = "DDERR_INVALIDCAPS";
        break;
    case DDERR_INVALIDCLIPLIST:
        eb = "DDERR_INVALIDCLIPLIST";
        break;
    case DDERR_INVALIDDIRECTDRAWGUID:
        eb = "DDERR_INVALIDDIRECTDRAWGUID";
        break;
    case DDERR_INVALIDMODE:
        eb = "DDERR_INVALIDMODE";
        break;
    case DDERR_INVALIDOBJECT:
        eb = "DDERR_INVALIDOBJECT";
        break;
    case DDERR_INVALIDPARAMS:
        eb = "DDERR_INVALIDPARAMS";
        break;
    case DDERR_INVALIDPIXELFORMAT:
        eb = "DDERR_INVALIDPIXELFORMAT";
        break;
    case DDERR_INVALIDPOSITION:
        eb = "DDERR_INVALIDPOSITION";
        break;
    case DDERR_INVALIDRECT:
        eb = "DDERR_INVALIDRECT";
        break;
    case DDERR_INVALIDSTREAM:
        eb = "DDERR_INVALIDSTREAM";
        break;
    case DDERR_INVALIDSURFACETYPE:
        eb = "DDERR_INVALIDSURFACETYPE";
        break;
    case DDERR_LOCKEDSURFACES:
        eb = "DDERR_LOCKEDSURFACES";
        break;
    case DDERR_MOREDATA:
        eb = "DDERR_MOREDATA";
        break;
    case DDERR_NO3D:
        eb = "DDERR_NO3D";
        break;
    case DDERR_NOALPHAHW:
        eb = "DDERR_NOALPHAHW";
        break;
    case DDERR_NOBLTHW:
        eb = "DDERR_NOBLTHW";
        break;
    case DDERR_NOCLIPLIST:
        eb = "DDERR_NOCLIPLIST";
        break;
    case DDERR_NOCLIPPERATTACHED:
        eb = "DDERR_NOCLIPPERATTACHED";
        break;
    case DDERR_NOCOLORCONVHW:
        eb = "DDERR_NOCOLORCONVHW";
        break;
    case DDERR_NOCOLORKEY:
        eb = "DDERR_NOCOLORKEY";
        break;
    case DDERR_NOCOLORKEYHW:
        eb = "DDERR_NOCOLORKEYHW";
        break;
    case DDERR_NOCOOPERATIVELEVELSET:
        eb = "DDERR_NOCOOPERATIVELEVELSET";
        break;
    case DDERR_NODC:
        eb = "DDERR_NODC";
        break;
    case DDERR_NODDROPSHW:
        eb = "DDERR_NODDROPSHW";
        break;
    case DDERR_NODIRECTDRAWHW:
        eb = "DDERR_NODIRECTDRAWHW";
        break;
    case DDERR_NODIRECTDRAWSUPPORT:
        eb = "DDERR_NODIRECTDRAWSUPPORT";
        break;
    case DDERR_NOEMULATION:
        eb = "DDERR_NOEMULATION";
        break;
    case DDERR_NOEXCLUSIVEMODE:
        eb = "DDERR_NOEXCLUSIVEMODE";
        break;
    case DDERR_NOFLIPHW:
        eb = "DDERR_NOFLIPHW";
        break;
    case DDERR_NOFOCUSWINDOW:
        eb = "DDERR_NOFOCUSWINDOW";
        break;
    case DDERR_NOGDI:
        eb = "DDERR_NOGDI";
        break;
    case DDERR_NOHWND:
        eb = "DDERR_NOHWND";
        break;
    case DDERR_NOMIPMAPHW:
        eb = "DDERR_NOMIPMAPHW";
        break;
    case DDERR_NOMIRRORHW:
        eb = "DDERR_NOMIRRORHW";
        break;
    case DDERR_NONONLOCALVIDMEM:
        eb = "DDERR_NONONLOCALVIDMEM";
        break;
    case DDERR_NOOPTIMIZEHW:
        eb = "DDERR_NOOPTIMIZEHW";
        break;
    case DDERR_NOOVERLAYDEST:
        eb = "DDERR_NOOVERLAYDEST";
        break;
    case DDERR_NOOVERLAYHW:
        eb = "DDERR_NOOVERLAYHW";
        break;
    case DDERR_NOPALETTEATTACHED:
        eb = "DDERR_NOPALETTEATTACHED";
        break;
    case DDERR_NOPALETTEHW:
        eb = "DDERR_NOPALETTEHW";
        break;
    case DDERR_NORASTEROPHW:
        eb = "DDERR_NORASTEROPHW";
        break;
    case DDERR_NOROTATIONHW:
        eb = "DDERR_NOROTATIONHW";
        break;
    case DDERR_NOSTRETCHHW:
        eb = "DDERR_NOSTRETCHHW";
        break;
    case DDERR_NOT4BITCOLOR:
        eb = "DDERR_NOT4BITCOLOR";
        break;
    case DDERR_NOT4BITCOLORINDEX:
        eb = "DDERR_NOT4BITCOLORINDEX";
        break;
    case DDERR_NOT8BITCOLOR:
        eb = "DDERR_NOT8BITCOLOR";
        break;
    case DDERR_NOTAOVERLAYSURFACE:
        eb = "DDERR_NOTAOVERLAYSURFACE";
        break;
    case DDERR_NOTEXTUREHW:
        eb = "DDERR_NOTEXTUREHW";
        break;
    case DDERR_NOTFLIPPABLE:
        eb = "DDERR_NOTFLIPPABLE";
        break;
    case DDERR_NOTFOUND:
        eb = "DDERR_NOTFOUND";
        break;
    case DDERR_NOTINITIALIZED:
        eb = "DDERR_NOTINITIALIZED";
        break;
    case DDERR_NOTLOADED:
        eb = "DDERR_NOTLOADED";
        break;
    case DDERR_NOTLOCKED:
        eb = "DDERR_NOTLOCKED";
        break;
    case DDERR_NOTPAGELOCKED:
        eb = "DDERR_NOTPAGELOCKED";
        break;
    case DDERR_NOTPALETTIZED:
        eb = "DDERR_NOTPALETTIZED";
        break;
    case DDERR_NOVSYNCHW:
        eb = "DDERR_NOVSYNCHW";
        break;
    case DDERR_NOZBUFFERHW:
        eb = "DDERR_NOZBUFFERHW";
        break;
    case DDERR_NOZOVERLAYHW:
        eb = "DDERR_NOZOVERLAYHW";
        break;
    case DDERR_OUTOFCAPS:
        eb = "DDERR_OUTOFCAPS";
        break;
    case DDERR_OUTOFMEMORY:
        eb = "DDERR_OUTOFMEMORY";
        break;
    case DDERR_OUTOFVIDEOMEMORY:
        eb = "DDERR_OUTOFVIDEOMEMORY";
        break;
    case DDERR_OVERLAPPINGRECTS:
        eb = "DDERR_OVERLAPPINGRECTS";
        break;
    case DDERR_OVERLAYCANTCLIP:
        eb = "DDERR_OVERLAYCANTCLIP";
        break;
    case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
        eb = "DDERR_OVERLAYCOLORKEYONLYONEACTIVE";
        break;
    case DDERR_OVERLAYNOTVISIBLE:
        eb = "DDERR_OVERLAYNOTVISIBLE";
        break;
    case DDERR_PALETTEBUSY:
        eb = "DDERR_PALETTEBUSY";
        break;
    case DDERR_PRIMARYSURFACEALREADYEXISTS:
        eb = "DDERR_PRIMARYSURFACEALREADYEXISTS";
        break;
    case DDERR_REGIONTOOSMALL:
        eb = "DDERR_REGIONTOOSMALL";
        break;
    case DDERR_SURFACEALREADYATTACHED:
        eb = "DDERR_SURFACEALREADYATTACHED";
        break;
    case DDERR_SURFACEALREADYDEPENDENT:
        eb = "DDERR_SURFACEALREADYDEPENDENT";
        break;
    case DDERR_SURFACEBUSY:
        eb = "DDERR_SURFACEBUSY";
        break;
    case DDERR_SURFACEISOBSCURED:
        eb = "DDERR_SURFACEISOBSCURED";
        break;
    case DDERR_SURFACELOST:
        eb = "DDERR_SURFACELOST";
        break;
    case DDERR_SURFACENOTATTACHED:
        eb = "DDERR_SURFACENOTATTACHED";
        break;
    case DDERR_TOOBIGHEIGHT:
        eb = "DDERR_TOOBIGHEIGHT";
        break;
    case DDERR_TOOBIGSIZE:
        eb = "DDERR_TOOBIGSIZE";
        break;
    case DDERR_TOOBIGWIDTH:
        eb = "DDERR_TOOBIGWIDTH";
        break;
    case DDERR_UNSUPPORTED:
        eb = "DDERR_UNSUPPORTED";
        break;
    case DDERR_UNSUPPORTEDFORMAT:
        eb = "DDERR_UNSUPPORTEDFORMAT";
        break;
    case DDERR_UNSUPPORTEDMASK:
        eb = "DDERR_UNSUPPORTEDMASK";
        break;
    case DDERR_UNSUPPORTEDMODE:
        eb = "DDERR_UNSUPPORTEDMODE";
        break;
    case DDERR_VERTICALBLANKINPROGRESS:
        eb = "DDERR_VERTICALBLANKINPROGRESS";
        break;
    case DDERR_VIDEONOTACTIVE:
        eb = "DDERR_VIDEONOTACTIVE";
        break;
    case DDERR_WASSTILLDRAWING:
        eb = "DDERR_WASSTILLDRAWING";
        break;
    case DDERR_WRONGMODE:
        eb = "DDERR_WRONGMODE";
        break;
    case DDERR_XALIGN:
        eb = "DDERR_XALIGN";
        break;
    default:
        eb = "unknown";
        break;
    }
    return eb;
}
