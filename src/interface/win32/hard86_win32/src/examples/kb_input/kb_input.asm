org 100h

jmp start

in_buf db 40 dup(0)

start:
lea di, [in_buf]
mov bx, 0
@@cont:
xor ah, ah
int 16h
cmp al, 0dh
je @@stop
mov [di+bx], al
mov ah, 09h
xor bh, bh
mov bl, 0fh
mov cx, 1
int 10h

mov ah, 3h
int 10h

inc dl

mov ah, 2h
int 10h

inc bx
cmp bx, 40
jne @@cont
@@stop:

int 20h