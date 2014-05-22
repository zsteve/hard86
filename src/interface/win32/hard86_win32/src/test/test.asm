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

  lea si, [a]
  lea di, [b]
  
  mov cx, 5
  
  rep movsb

  ; in/out test
  mov cx, 0x1
@@start:
  xor ax, ax
  mov al, cl
  shl ax, 4
  out 5, ax

  dec cx
  cmp cx, 0
  jne @@start
  ; parameter : ax = n
  ; calculate 5!
  mov cx, 1
  mov ah, 9h
  mov al, 'a'
  int 10h
  mov ax, 100
  call factorial


  ret