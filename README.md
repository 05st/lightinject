LightInject
===========
LightInject is a simple command line tool to inject dlls into a process. It supports different injection methods.
## Methods ##
- CreateRemoteThread (1) *[dllmain_32.dll | dllmain_64.dll]*
- NxCreateThreadEx (2) *[dllmain_32.dll | dllmain_64.dll]*
- QueueUserAPC (3) *[dllmain_32.dll | dllmain_64.dll]*
- SetWindowsHookEx (4) *[dllpoc_32.dll | dllpoc_64.dll]*

Supported OS: Win7 - Win10 (x86)
## Usage ##
You should download the binary from the [releases](https://github.com/05st/lightinject/releases) tab (although it might be slightly outdated compared to building it yourself). I recommend you add it to your PATH environment variable.
If you want to build this project, open up the solution in Visual Studio and build it from there.

```
lightinject.exe processName.exe "C:\directory\example.dll" 1
(Inject example.dll to processName.exe using CreateRemoteThread.)
```
