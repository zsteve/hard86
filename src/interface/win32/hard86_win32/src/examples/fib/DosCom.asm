;  A 16-bit DOS HelloWorld program originally by RedOx.  Produces a tiny model .com executable.

; To assemble and link from within WinAsm Studio, you must have a special 16-bit linker, 
; such as the one in the archive at this URL- http://win32assembly.online.fr/files/Lnk563.exe
; Run the archive to unpack Link.exe, rename the Link.exe file to Link16.exe and copy it
; into the \masm32\bin folder.
;
; To produce a .COM file, .model must be tiny, also you must add /tiny to the linker command line

.model tiny, stdcall
.data
.code

	rec_wrapper proto :WORD

	.startup
	
	invoke rec_wrapper, 20
	out 40h, ax
	
	ret
	
rec_fib proc a:WORD, b:WORD, count:WORD
	.if(count==0)
		mov ax, a
		add ax, b
		ret
	.endif
	xor ax, ax
	mov ax, a
	add ax, b
	mov bx, count
	dec bx
	invoke rec_fib, ax, a, bx
	Ret
rec_fib endp

rec_wrapper proc n:WORD
	invoke rec_fib, 1, 0, n
	Ret
rec_wrapper endp
end