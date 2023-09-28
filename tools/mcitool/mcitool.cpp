// mcitool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"

char *mcierr(DWORD err) {
    char *e;
    switch (err) {
    case MCIERR_BAD_CONSTANT:
        e = "The value specified for a parameter is unknown.";
        break;
    case MCIERR_BAD_INTEGER:
        e = "An integer in the command was invalid or missing.";
        break;
    case MCIERR_DUPLICATE_FLAGS:
        e = "A flag or value was specified twice.";
        break;
    case MCIERR_MISSING_COMMAND_STRING:
        e = "No command was specified.";
        break;
    case MCIERR_MISSING_DEVICE_NAME:
        e = "No device name was specified.";
        break;
    case MCIERR_MISSING_STRING_ARGUMENT:
        e = "A string value was missing from the command.";
        break;
    case MCIERR_NEW_REQUIRES_ALIAS:
        e = "An alias must be used with the \"new\" device name.";
        break;
    case MCIERR_NO_CLOSING_QUOTE:
        e = "A closing quotation mark is missing.";
        break;
    case MCIERR_NOTIFY_ON_AUTO_OPEN:
        e = "The \"notify\" flag is illegal with auto-open.";
        break;
    case MCIERR_PARAM_OVERFLOW:
        e = "The output string was not long enough.";
        break;
    case MCIERR_PARSER_INTERNAL:
        e = "An internal parser error occurred.";
        break;
    case MCIERR_UNRECOGNIZED_KEYWORD:
        e = "An unknown command parameter was specified.";
        break;
    case MCIERR_BAD_TIME_FORMAT:
        e = "The specified value for the time format is invalid.";
        break;
    case MCIERR_CANNOT_LOAD_DRIVER:
        e = "The specified device driver will not load properly.";
        break;
    case MCIERR_CANNOT_USE_ALL:
        e = "The device name \"all\" is not allowed for this command.";
        break;
    case MCIERR_CREATEWINDOW:
        e = "Could not create or use window.";
        break;
    case MCIERR_DEVICE_LENGTH:
        e = "The device or driver name is too long. Specify a device or driver name that is less than 79 characters.";
        break;
    case MCIERR_DEVICE_LOCKED:
        e = "The device is now being closed. Wait a few seconds, then try again.";
        break;
    case MCIERR_DEVICE_NOT_INSTALLED:
        e = "The specified device is not installed on the system. Use the Drivers option from the Control Panel to install the device.";
        break;
    case MCIERR_DEVICE_NOT_READY:
        e = "The device driver is not ready.";
        break;
    case MCIERR_DEVICE_OPEN:
        e = "The device name is already used as an alias by this application. Use a unique alias.";
        break;
    case MCIERR_DEVICE_ORD_LENGTH:
        e = "The device or driver name is too long. Specify a device or driver name that is less than 79 characters.";
        break;
    case MCIERR_DEVICE_TYPE_REQUIRED:
        e = "The specified device cannot be found on the system. Check that the device is installed and the device name is spelled correctly.";
        break;
    case MCIERR_DRIVER:
        e = "The device driver exhibits a problem. Check with the device manufacturer about obtaining a new driver.";
        break;
    case MCIERR_DRIVER_INTERNAL:
        e = "The device driver exhibits a problem. Check with the device manufacturer about obtaining a new driver.";
        break;
    case MCIERR_DUPLICATE_ALIAS:
        e = "The specified alias is already used in this application. Use a unique alias.";
        break;
    case MCIERR_EXTENSION_NOT_FOUND:
        e = "The specified extension has no device type associated with it. Specify a device type.";
        break;
    case MCIERR_EXTRA_CHARACTERS:
        e = "You must enclose a string with quotation marks; characters following the closing quotation mark are not valid.";
        break;
    case MCIERR_FILE_NOT_FOUND:
        e = "The requested file was not found. Check that the path and filename are correct.";
        break;
    case MCIERR_FILE_NOT_SAVED:
        e = "The file was not saved. Make sure your system has sufficient disk space or has an intact network connection.";
        break;
    case MCIERR_FILE_READ:
        e = "A read from the file failed. Make sure the file is present on your system or that your system has an intact network connection.";
        break;
    case MCIERR_FILE_WRITE:
        e = "A write to the file failed. Make sure your system has sufficient disk space or has an intact network connection.";
        break;
    case MCIERR_FILENAME_REQUIRED:
        e = "The filename is invalid. Make sure the filename is no longer than eight characters, followed by a period and an extension.";
        break;
    case MCIERR_FLAGS_NOT_COMPATIBLE:
        e = "The specified parameters cannot be used together.";
        break;
    case MCIERR_GET_CD:
        e = "The requested file OR MCI device was not found. Try changing directories or restarting your system.";
        break;
    case MCIERR_HARDWARE:
        e = "The specified device exhibits a problem. Check that the device is working correctly or contact the device manufacturer.";
        break;
    case MCIERR_ILLEGAL_FOR_AUTO_OPEN:
        e = "MCI will not perform the specified command on an automatically opened device. Wait until the device is closed, then try to perform the command.";
        break;
    case MCIERR_INTERNAL:
        e = "A problem occurred in initializing MCI. Try restarting the Windows operating system.";
        break;
    case MCIERR_INVALID_DEVICE_ID:
        e = "Invalid device ID. Use the ID given to the device when the device was opened.";
        break;
    case MCIERR_INVALID_DEVICE_NAME:
        e = "The specified device is not open nor recognized by MCI.";
        break;
    case MCIERR_INVALID_FILE:
        e = "The specified file cannot be played on the specified MCI device. The file may be corrupt or may use an incorrect file format.";
        break;
    case MCIERR_MISSING_PARAMETER:
        e = "The specified command requires a parameter, which you must supply.";
        break;
    case MCIERR_MULTIPLE:
        e = "Errors occurred in more than one device. Specify each command and device separately to identify the devices causing the errors.";
        break;
    case MCIERR_MUST_USE_SHAREABLE:
        e = "The device driver is already in use. You must specify the \"sharable\" parameter with each open command to share the device.";
        break;
    case MCIERR_NO_ELEMENT_ALLOWED:
        e = "The specified device does not use a filename.";
        break;
    case MCIERR_NO_INTEGER:
        e = "The parameter for this MCI command must be an integer value.";
        break;
    case MCIERR_NO_WINDOW:
        e = "There is no display window.";
        break;
    case MCIERR_NONAPPLICABLE_FUNCTION:
        e = "The specified MCI command sequence cannot be performed in the given order. Correct the command sequence; then, try again.";
        break;
    case MCIERR_NULL_PARAMETER_BLOCK:
        e = "A null parameter block (structure) was passed to MCI.";
        break;
    case MCIERR_OUT_OF_MEMORY:
        e = "Your system does not have enough memory for this task. Quit one or more applications to increase the available memory, then, try to perform the task again.";
        break;
    case MCIERR_OUTOFRANGE:
        e = "The specified parameter value is out of range for the specified MCI command.";
        break;
    case MCIERR_SET_CD:
        e = "The specified file or MCI device is inaccessible because the application cannot change directories.";
        break;
    case MCIERR_SET_DRIVE:
        e = "The specified file or MCI device is inaccessible because the application cannot change drives.";
        break;
    case MCIERR_UNNAMED_RESOURCE:
        e = "You cannot store an unnamed file. Specify a filename.";
        break;
    case MCIERR_UNRECOGNIZED_COMMAND:
        e = "The driver cannot recognize the specified command.";
        break;
    case MCIERR_UNSUPPORTED_FUNCTION:
        e = "The MCI device driver the system is using does not support the specified command.";
        break;
    case MCIERR_WAVE_INPUTSINUSE:
        e = "All waveform devices that can record files in the current format are in use. Wait until one of these devices is free; then, try again.";
        break;
    case MCIERR_WAVE_INPUTSUNSUITABLE:
        e = "No installed waveform device can record files in the current format. Use the Drivers option from the Control Panel to install a suitable waveform recording device.";
        break;
    case MCIERR_WAVE_INPUTUNSPECIFIED:
        e = "You can specify any compatible waveform recording device.";
        break;
    case MCIERR_WAVE_OUTPUTSINUSE:
        e = "All waveform devices that can play files in the current format are in use. Wait until one of these devices is free; then, try again.";
        break;
    case MCIERR_WAVE_OUTPUTSUNSUITABLE:
        e = "No installed waveform device can play files in the current format. Use the Drivers option from the Control Panel to install a suitable waveform device.";
        break;
    case MCIERR_WAVE_OUTPUTUNSPECIFIED:
        e = "You can specify any compatible waveform playback device.";
        break;
    case MCIERR_WAVE_SETINPUTINUSE:
        e = "The current waveform device is in use. Wait until the device is free; then, try again to set the device for recording.";
        break;
    case MCIERR_WAVE_SETINPUTUNSUITABLE:
        e = "The device you are using to record a waveform cannot recognize the data format.";
        break;
    case MCIERR_WAVE_SETOUTPUTINUSE:
        e = "The current waveform device is in use. Wait until the device is free; then, try again to set the device for playback.";
        break;
    case MCIERR_WAVE_SETOUTPUTUNSUITABLE:
        e = "The device you are using to playback a waveform cannot recognize the data format.";
        break;
    case MCIERR_SEQ_DIV_INCOMPATIBLE:
        e = "The time formats of the \"song pointer\" and SMPTE are singular. You can't use them together.";
        break;
    case MCIERR_SEQ_NOMIDIPRESENT:
        e = "This system has no installed MIDI devices. Use the Drivers option from the Control Panel to install a MIDI driver.";
        break;
    case MCIERR_SEQ_PORT_INUSE:
        e = "The specified MIDI port is already in use. Wait until it is free; then, try again.";
        break;
    case MCIERR_SEQ_PORT_MAPNODEVICE:
        e = "The current MIDI Mapper setup refers to a MIDI device that is not installed on the system. Use the MIDI Mapper from the Control Panel to edit the setup.";
        break;
    case MCIERR_SEQ_PORT_MISCERROR:
        e = "An error occurred with specified port.";
        break;
    case MCIERR_SEQ_PORT_NONEXISTENT:
        e = "The specified MIDI device is not installed on the system. Use the Drivers option from the Control Panel to install a MIDI device.";
        break;
    case MCIERR_SEQ_PORTUNSPECIFIED:
        e = "The system does not have a current MIDI port specified.";
        break;
    case MCIERR_SEQ_TIMER:
        e = "All multimedia timers are being used by other applications. Quit one of these applications; then, try again.";
        break;
    //case MCIERR_AVI_AUDIOERROR: e="Unknown error while attempting to play audio."; break;
    //case MCIERR_AVI_BADPALETTE: e="Unable to switch to new palette."; break;
    //case MCIERR_AVI_CANTPLAYFULLSCREEN: e="This AVI file cannot be played in full screen mode."; break;
    //case MCIERR_AVI_DISPLAYERROR: e="Unknown error while attempting to display video."; break;
    //case MCIERR_AVI_NOCOMPRESSOR: e="Can't locate installable compressor needed to play this file."; break;
    //case MCIERR_AVI_NODISPDIB: e="256 color VGA mode not available."; break;
    //case MCIERR_AVI_NOTINTERLEAVED: e="This AVI file is not interleaved."; break;
    //case MCIERR_AVI_OLDAVIFORMAT: e="This AVI file is of an obsolete format."; break;
    //case MCIERR_AVI_TOOBIGFORVGA: e="This AVI file is too big to be played in the selected VGA mode."; break;
    default:
        e = "???";
        break;
    }
    return e;
}

void mciparser() {
    char s[1024 + 1];
    char r[1024 + 1];
    MCIERROR ret;
    printf("mcitool: (C)gho 2017\nenter mci string, cd <path> to change working directory or q to quit\n");
    printf("example:\n\topen cdaudio\n\tset cdaudio time format tmsf\n\tplay cdaudio from 6 to 7\n\tclose cdaudio\n\n");
    while(1) {
        printf("> ");
        gets_s(s, 1024);
        if((s[0] == 'q') || (s[0] == 'Q'))return;
        if(!strncmp(s, "cd ", 3)) {
            printf("cd \"%s\"\n", &s[3]);
            if(SetCurrentDirectory(&s[3])) {
                char wdir[MAX_PATH + 1];
                GetCurrentDirectory(MAX_PATH, wdir);
                printf("successfully moved to \"%s\"\n", wdir);
            } else
                printf("ERROR: err=%d\n", GetLastError());
            continue;
        }
        strcpy_s(r, 1024, "****"); // mark return buffer
        ret = mciSendStringA(s, r, 1024, 0);
        if(ret) {
            DWORD err, herr;
            err = ret & 0xFFFF;
            herr = (ret >> 16) & 0xFFFF;
            printf("ERROR: ret=%d %s\n", err, mcierr(err));
            if(herr) printf("SPECIFIC ERROR: %d\n", herr);
        } else
            printf("SUCCESS: \"%s\"\n", r);
    }
}

int _tmain(int argc, _TCHAR *argv[]) {
    mciparser();
    return 0;
}

