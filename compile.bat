@ECHO OFF
CD C:\bc31\bin
bcc.exe -mh -v -em-kernel.exe -IC:\bc31\include;C:\project\h -LC:\bc31\lib;C:\project\lib C:\project\src\*.cpp C:\project\lib\*.lib
DEL *.obj
MKDIR C:\project\output
COPY m-kernel.exe C:\project\output\m-kernel.exe
DEL m-kernel.exe
PAUSE