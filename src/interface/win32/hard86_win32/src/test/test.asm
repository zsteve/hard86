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
  

start:
  ; parameter : ax = n
  ; calculate 5!
  mov cx, 1
  mov ah, 9h
  mov al, 'a'
  int 10h
  mov ax, 100
  call factorial
  ret