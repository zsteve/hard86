: This batch file is an example of how to assemble and link at the dos prompt.
: It is not required.  You can assemble and link from within WinAsm Studio.
: To link, you must use a 16-bit linker, such as the one in the archive at
: this URL- http://win32assembly.online.fr/files/Lnk563.exe
: Rename the Link.exe file to Link16.exe and copy it into the \masm32\bin folder.

\masm32\bin\ml /c DosCom.asm
\masm32\bin\link16 /tiny DosCom.obj;

pause