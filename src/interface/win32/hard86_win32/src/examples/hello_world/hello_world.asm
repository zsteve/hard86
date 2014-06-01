org 100h

	jmp start
msg1 db "Hello World!"
start:
	mov al, 1
	mov bh, 0
	mov bl, 0eh
	mov cx, start - msg1
	mov dl, 0
	mov dh, 0
	push cs
	pop es
	mov bp, msg1
	mov ah, 13h
	int 10h
	
	int 20h