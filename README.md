# apisetparse
Uses the PEB to obtain an apisetmap in order to translate umbrella DLL's such as "api-ms-win-http-time-l1-1-0.dll" to their origin forward DLL e.g. kernelbase.dll


Thanks for snippets and theory :)
- https://xz.aliyun.com/t/7019   (Main functionality)
- https://gist.github.com/lucasg/9aa464b95b4b7344cb0cddbdb4214b25
- https://www.yumpu.com/en/document/read/55059569/hooking-nirvana
- https://lucasg.github.io/2017/10/15/Api-set-resolution/
- https://github.com/zodiacon/WindowsInternals/blob/master/APISetMap/APISetMap.cpp
- https://blog.quarkslab.com/runtime-dll-name-resolution-apisetschema-part-i.html
- https://blog.quarkslab.com/runtime-dll-name-resolution-apisetschema-part-ii.html

example output
```
Z:\>apisetparse.exe | findstr /i kernel32
api-ms-win-core-appinit-l1-1-0.dll,kernel32.dll,kernelbase.dll[kernel32.dll]
api-ms-win-core-atoms-l1-1-0.dll,kernel32.dll
api-ms-win-core-calendar-l1-1-0.dll,kernel32.dll
api-ms-win-core-console-ansi-l2-1-0.dll,kernel32.dll
api-ms-win-core-file-ansi-l1-1-0.dll,kernel32.dll
api-ms-win-core-file-ansi-l2-1-0.dll,kernel32.dll
api-ms-win-core-firmware-l1-1-0.dll,kernel32.dll
api-ms-win-core-heap-obsolete-l1-1-0.dll,kernel32.dll
api-ms-win-core-io-l1-1-1.dll,kernel32.dll,kernelbase.dll[kernel32.dll]
api-ms-win-core-job-l2-1-1.dll,kernel32.dll
api-ms-win-core-kernel32-legacy-ansi-l1-1-0.dll,kernel32.dll
api-ms-win-core-kernel32-legacy-l1-1-6.dll,kernel32.dll
api-ms-win-core-kernel32-private-l1-1-2.dll,kernel32.dll
api-ms-win-core-localization-ansi-l1-1-0.dll,kernel32.dll
api-ms-win-core-namedpipe-ansi-l1-1-1.dll,kernel32.dll
api-ms-win-core-namespace-ansi-l1-1-0.dll,kernel32.dll
api-ms-win-core-privateprofile-l1-1-1.dll,kernel32.dll
api-ms-win-core-processenvironment-ansi-l1-1-0.dll,kernel32.dll
api-ms-win-core-processsecurity-l1-1-0.dll,kernel32.dll,kernelbase.dll[kernel32.dll]
api-ms-win-core-processthreads-l1-1-4.dll,kernel32.dll,kernelbase.dll[kernel32.dll]
[...SNIP...]
```
