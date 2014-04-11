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
  mov ax, 100
  call factorial
  ret