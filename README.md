[![HitCount](http://hits.dwyl.com/vaxerski/ZoomWebcamFix.svg)](http://hits.dwyl.com/vaxerski/ZoomWebcamFix)

# ZoomWebcamFix

A simple program to unblock webcams blocked by Zoom. <br/>
***Go to "releases" tab for compiled downloads.***

## Usage

Download the latest release from the releases tab.

- Launch the program **before** zoom.
- Start zoom.
- Leave the process in the background, it patches zoom in real-time.

some anti-viruses *may* flag the launcher as unsafe, but it's just because it's an injector. <br/>
It does not do any harm to your pc, it just needs access to Zoom's process to patch certain functions.

## Screenshots

![Before](https://i.ibb.co/878DXXt/before.png) ![After](https://i.ibb.co/ckYmyxc/after.png)

## How it works

- Hook the DLLMain of the DllSafeCheck.dll.
In onAttach(), this dll registers an NTDLL callback to check for all dlls loaded by zoom.exe and scan them.
We hook DLLMain and call onDetach() to unregister the callback and make zoom blind for any dlls, effectively disabling the scan.

- Assembly patch the HackCheck() call in WinMain of Zoom.exe, but idk if it does anything anymore tbh.
