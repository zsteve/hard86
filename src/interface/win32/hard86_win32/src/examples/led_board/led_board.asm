org 100h

; LED board data port is 0x40

	xor ax, ax
@@cont:
	inc al
	out 0x40, al
	cmp ax, 0xff
	jne @@cont
	
	int 20h	; return to system