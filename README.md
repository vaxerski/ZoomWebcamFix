[![HitCount](http://hits.dwyl.com/vaxerski/ZoomWebcamFix.svg)](http://hits.dwyl.com/vaxerski/ZoomWebcamFix)

# ZoomWebcamFix

A simple program to unblock webcams blocked by Zoom. <br/>
***Go to "releases" tab for compiled downloads.***

## Usage

Download the latest release from the releases tab.

Launch the program **before** zoom. Start zoom.<br/>
For file patch (secondary, recommended runtime patch), click HOME before launching Zoom, and then launch Zoom.

This software has two modes of patching.

**Default** - runtime code patching. Requires the software to be open in the background all the time while zoom is running,
but is more effective when it comes to Zoom's updates.

**File patch** - activates upon pressing *HOME* while waiting for Zoom. Will patch the dll locally, making a copy.
This method *does not* require any more opening of the program. Once you patch the file, its done and you don't 
need to do anything else. It *will* reset though on Zoom's updates.

## Screenshots

![Before](https://i.ibb.co/878DXXt/before.png) ![After](https://i.ibb.co/ckYmyxc/after.png)

## How it works

**Default**:

- Hooks the DLLMain of the DllSafeCheck.dll.
In onAttach(), this dll registers an NTDLL callback to check for all dlls loaded by zoom.exe and scan them.
We hook DLLMain and call onDetach() to unregister the callback and make zoom blind for any dlls, effectively disabling the scan.

- Assembly patches the HackCheck() call in WinMain of Zoom.exe, but idk if it does anything anymore tbh.

**File patch**:

- Patches the HackCheck() function to be nop'd, effectively removing it and making zoom call nothing and return true :)
