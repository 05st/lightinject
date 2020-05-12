LightInject
===========
LightInject is a simple command line tool to inject dlls into a process. It supports different injection methods.
## Methods ##
- CreateRemoteThread (1)
- NxCreateThreadEx (2)
- QueueUserAPC (3)

Supported OS: Win7-Win10 x64
## Usage ##
```
lightinject.exe processName.exe "C:\directory\example.dll" 1
(Inject example.dll to processName.exe using CreateRemoteThread.)
```
