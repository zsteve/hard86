.model tiny
.data

STAR struct
	x	dw ?	; xpos
	y	dw ?	; ypos
	s	dw ?	; speed
STAR ends

NUM_STARS equ 100

.data

	stars STAR NUM_STARS dup(<0>)
	
	SEED DW 3749h
	RNDNUM DW 0

.code
	.startup

	call init_stars
	.while 1
		xor ax, ax
		out 41h, ax	; lock video buffer
		call clrscr
		call move_stars
		call draw_stars
		mov ax, 1
		out 41h, ax	; unlock video buffer
	.endw
	ret


; random number generator from http://programmersheaven.com/discussion/219366/a-random-number-generator-routine 
random proc
	; Call with dx = range
	LOCAL range:WORD
	
	mov range, dx
	
	; Returns, AL = random number between 0-255,
	; AX may be a random number too ?
	; DW RNDNUM holds AX=random_number_in AX
	PUSH DX
	MOV AX,[SEED] ;; AX = seed
	MOV DX,8405h ;; DX = 8405h
	MUL DX ;; MUL (8405h * SEED) into dword DX:AX
	CMP AX,[SEED]
	JNZ GOTSEED ;; if new SEED = old SEED, alter SEED
	MOV AH,DL
	INC AX
	GOTSEED:
	MOV word ptr [SEED], AX ;; We have a new seed, so store it
	MOV AX,DX ;; AL = random number
	
	mov dx, range
	div dx	; dx = modulus
	mov ax, dx
	
	MOV word ptr [RNDNUM], AX
	
	POP DX
	RET
random endp

clrscr proc
	push es
	mov bx, 0c800h
	xor di, di
	mov es, bx
	mov al, 0
	mov cx, 320*200
	rep stosb
	pop es
	Ret
clrscr endp

init_stars proc
	
	xor ax, ax
	xor cx, cx
	xor di, di
	lea di, stars
	
@@cont:
	mov dx, 320
	call random
	;xor ah, ah
	mov [di].STAR.x, ax
	mov dx, 200
	call random
	;xor ah, ah
	mov [di].STAR.y, ax
	
	mov dx, 5
	call random
  inc ax
	mov [di].STAR.s, ax
	add di, sizeof(STAR)
	inc cx
	cmp cx, NUM_STARS
	jne @@cont
	
	Ret
init_stars endp

move_stars proc
	
	xor ax, ax
	xor cx, cx
	xor di, di
	lea di, stars
	
	.while cx<NUM_STARS
		mov ax, [di].STAR.s
		add [di].STAR.x, ax
		.if [di].STAR.x > 320
			xor ax, ax
			mov [di].STAR.x, ax
			mov dx, 200
			call random
			mov [di].STAR.y, ax
			mov dx, 5
			call random
      inc ax
			mov [di].STAR.s, ax
		.endif
		add di, sizeof(STAR)
		inc cx
	.endw

	Ret
move_stars endp

draw_stars proc
	
	push es
	
	mov bx, 0c800h
	mov es, bx
	
	xor ax, ax
	xor cx, cx
	xor di, di
	xor si, si
	lea di, stars

	.while cx < NUM_STARS
		mov ax, [di].STAR.y
		mov bx, 320
		mul bx
		add ax, [di].STAR.x
		mov bx, ax
		
		mov al, 255
		
		mov byte ptr es:[bx], al
		
		add di, sizeof(STAR)
		inc cx
		
	.endw	
	
	pop es

	Ret
draw_stars endp
end