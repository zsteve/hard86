org 100h

jmp start

; computes n!
factorial:
	cmp ax, 1
	jz @@r
	push ax
	dec ax
	call factorial
	mov bx, ax
	pop ax
	mul bx
	@@r:
	ret
  
a db 5 dup(0xff)
b db 5 dup(0)
  
start:

	mov ah, 3h
	int 10h
	
	inc dl
	
	mov ah, 2h
	int 10h
	
	
@@cont:
	
	mov ah, 0
	int 16h
	mov ah, 9h
	mov cx, 1
	mov bl, 0dh
	mov bh, 1h
	int 10h
	
	mov ah, 3h
	int 10h
	
	inc dl
	
	mov ah, 2h
	int 10h
	
	jmp @@cont
	

	mov al, 1
	mov bh, 0
	mov bl, 0eh
	mov cx, msg1end - msg1 ; calculate message size. 
	mov dl, 10
	mov dh, 7
	push cs
	pop es
	mov bp, msg1
	mov ah, 13h
	int 10h
	jmp msg1end
	msg1 db " hello, world! my name is Hard86 8086 emulator with support for virtual devices! "
	msg1end:

	; interrupt test
	mov al, 'a'
	mov bl, 2
	mov ah, 9
	mov cx, 81
	int 10h

	; screen test
	mov     ax, 0b800h
	mov     es, ax
	mov si, 1
	mov al, 'A'
	mov [es:si], al


	; test of xchg
	mov ax, 5
	mov bx, 6
	xchg ax, bx

	; test of string ops

	lea si, [a]
	lea di, [b]

	mov cx, 5

	rep movsb
	
	mov ax, 5
	mov bx, ds
	mov es, bx
	lea di, [b]
	mov cx, 5
	rep stosb

	; in/out test
	mov cx, 0xffff
@@start:
	xor ax, ax
	mov al, cl
	shl ax, 4
	out 5, ax

	dec cx
	cmp cx, 0
	jne @@start
	
	ret
	
	; parameter : ax = n
	; calculate 5!
	mov cx, 1
	mov ah, 9h
	mov al, 'a'
	int 10h
	mov ax, 100
	call factorial


  ret