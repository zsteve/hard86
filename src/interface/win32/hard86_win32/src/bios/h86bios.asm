; Hard86 emulator BIOS source
; Licensed under the GNU GPL v2 License
; Stephen Zhang 2014

offset equ
bios_seg equ 0x40

_data:
db "Hard86 Emulator BIOS", 0h

; data

cur_x db 19h
cur_y db 19h

kb_buf_size equ 64

kb_buf db kb_buf_size dup(0) ; keyboard buffer
kb_buf_begin dw 0
kb_buf_end dw 0

; interrupts


int_00:
int_01:
int_02:
int_03:
int_04:
int_05:
int_06:
int_07:
  ; hard86 specific interrupt
  ; key pressed
  ; data in port 0x60
  push ax
  push bx
  push ds
  
  mov bx, bios_seg
  mov ds, bx
  
  in al, 0x60
  
  mov bx, offset kb_buf
  add bx, word [kb_buf_end]
  mov byte [bx], al
  mov bx, offset kb_buf_end
  
  cmp [kb_buf_end], kb_buf_size-1
  je int7_out_of_space
  
  inc [kb_buf_end]
  
  pop ds
  pop bx
  pop ax
  iret
  
int7_out_of_space:
  cmp kb_buf_begin, 0
  je int7_buffer_full
  ; otherwise, wrap around to the beginning
  mov [kb_buf_end], 0
  
  pop ds
  pop bx
  pop ax
  iret
  
int7_buffer_full:
  ; keyboard buffer is truly full.
  
  
  pop ds
  pop bx
  pop ax
  
  iret
  
int_08:
int_09:
int_0a:
int_0b:
int_0c:
int_0d:
int_0e:
int_0f:
int_10:
	cmp ah, 02h
	je int10_ah2
  cmp ah, 08h
  je int10_ah8
	cmp ah, 09h
	je int10_ah9
	
int10_ah2:
	; set cursor position
	push ds
	push bx
	
	mov bx, bios_seg
	mov ds, bx
	mov [cur_y+_data], dh
	mov [cur_x+_data], dl
	
	pop bx
	pop ds
	iret
  
int10_ah8:
  ; read character and attribute at cursor position
  iret

int10_ah9:
	; write character and attribute at cursor position
	; video memory is at B800:0000
	push ds
	push es
	push bx
	push ax
	
	mov bx, bios_seg
	mov es, bx
	
	mov bx, 0xb800
	mov ds, bx
	
	xor bx, bx
	mov bl, [es:cur_y+_data]
	mov ax, 80d
	mul bx
	xor ax, ax
	mov al, [es:cur_x+_data]
	add bx, ax
	
	pop ax
	
int10_ah9_cont1:
	mov byte [bx], al
	inc bx
	mov byte [bx], ah
	dec cx
	cmp cx, 0
	jne int10_ah9_cont1

	pop bx
	pop es
	pop ds
	iret


int_11:
int_12:
int_13:
int_14:
int_15:
int_16:
  cmp ah, 0x00
  je int16_ah0
  cmp ah, 0x01
  je int16_ah1
  iret
int16_ah0:
  push ax
  push bx
  push ds
  
  mov bx, bios_seg
  mov ds, bx
  
  mov bx, [kb_buf_end]
  mov ax, [kb_buf_begin]
  cmp bx, ax
  jne int16_ah0_hasdata
  pop ds
  pop bx
  pop ax
  xor ax, ax
  iret
  
int16_ah0_hasdata:
  mov bx, offset kb_buf
  add bx, [kb_buf_begin]
  mov al, byte [bx]
  mov ah, al
  inc [kb_buf_begin]
  
  pop ds
  pop bx
  sub sp, 2
  iret
  
int16_ah1:
  push ax
  push bx
  push ds
  
  mov bx, bios_seg
  mov ds, bx
  
  mov bx, [kb_buf_end]
  mov ax, [kb_buf_begin]
  cmp bx, ax
  jne int16_ah1_hasdata
  
int16_ah1_hasdata:
  
  iret
int_17:
int_18:
int_19:
int_1a:
int_1b:
int_1c:
int_1d:
int_1e:
int_1f:
int_20:
int_21:
int_22:
int_23:
int_24:
int_25:
int_26:
int_27:
int_28:
int_29:
int_2a:
int_2b:
int_2c:
int_2d:
int_2e:
int_2f:
int_30:
int_31:
int_32:
int_33:
int_34:
int_35:
int_36:
int_37:
int_38:
int_39:
int_3a:
int_3b:
int_3c:
int_3d:
int_3e:
int_3f:
int_40:
int_41:
int_42:
int_43:
int_44:
int_45:
int_46:
int_47:
int_48:
int_49:
int_4a:
int_4b:
int_4c:
int_4d:
int_4e:
int_4f:
int_50:
int_51:
int_52:
int_53:
int_54:
int_55:
int_56:
int_57:
int_58:
int_59:
int_5a:
int_5b:
int_5c:
int_5d:
int_5e:
int_5f:
int_60:
int_61:
int_62:
int_63:
int_64:
int_65:
int_66:
int_67:
int_68:
int_69:
int_6a:
int_6b:
int_6c:
int_6d:
int_6e:
int_6f:
int_70:
int_71:
int_72:
int_73:
int_74:
int_75:
int_76:
int_77:
int_78:
int_79:
int_7a:
int_7b:
int_7c:
int_7d:
int_7e:
int_7f:
int_80:
int_81:
int_82:
int_83:
int_84:
int_85:
int_86:
int_87:
int_88:
int_89:
int_8a:
int_8b:
int_8c:
int_8d:
int_8e:
int_8f:
int_90:
int_91:
int_92:
int_93:
int_94:
int_95:
int_96:
int_97:
int_98:
int_99:
int_9a:
int_9b:
int_9c:
int_9d:
int_9e:
int_9f:
int_a0:
int_a1:
int_a2:
int_a3:
int_a4:
int_a5:
int_a6:
int_a7:
int_a8:
int_a9:
int_aa:
int_ab:
int_ac:
int_ad:
int_ae:
int_af:
int_b0:
int_b1:
int_b2:
int_b3:
int_b4:
int_b5:
int_b6:
int_b7:
int_b8:
int_b9:
int_ba:
int_bb:
int_bc:
int_bd:
int_be:
int_bf:
int_c0:
int_c1:
int_c2:
int_c3:
int_c4:
int_c5:
int_c6:
int_c7:
int_c8:
int_c9:
int_ca:
int_cb:
int_cc:
int_cd:
int_ce:
int_cf:
int_d0:
int_d1:
int_d2:
int_d3:
int_d4:
int_d5:
int_d6:
int_d7:
int_d8:
int_d9:
int_da:
int_db:
int_dc:
int_dd:
int_de:
int_df:
int_e0:
int_e1:
int_e2:
int_e3:
int_e4:
int_e5:
int_e6:
int_e7:
int_e8:
int_e9:
int_ea:
int_eb:
int_ec:
int_ed:
int_ee:
int_ef:
int_f0:
int_f1:
int_f2:
int_f3:
int_f4:
int_f5:
int_f6:
int_f7:
int_f8:
int_f9:
int_fa:
int_fb:
int_fc:
int_fd:
int_fe:
int_ff:
	
; interrupt offset table
; entire BIOS must fit in one 64k code segment,
; so only IP values are listed

dw offset int_00
dw offset int_01
dw offset int_02
dw offset int_03
dw offset int_04
dw offset int_05
dw offset int_06
dw offset int_07
dw offset int_08
dw offset int_09
dw offset int_0a
dw offset int_0b
dw offset int_0c
dw offset int_0d
dw offset int_0e
dw offset int_0f
dw offset int_10
dw offset int_11
dw offset int_12
dw offset int_13
dw offset int_14
dw offset int_15
dw offset int_16
dw offset int_17
dw offset int_18
dw offset int_19
dw offset int_1a
dw offset int_1b
dw offset int_1c
dw offset int_1d
dw offset int_1e
dw offset int_1f
dw offset int_20
dw offset int_21
dw offset int_22
dw offset int_23
dw offset int_24
dw offset int_25
dw offset int_26
dw offset int_27
dw offset int_28
dw offset int_29
dw offset int_2a
dw offset int_2b
dw offset int_2c
dw offset int_2d
dw offset int_2e
dw offset int_2f
dw offset int_30
dw offset int_31
dw offset int_32
dw offset int_33
dw offset int_34
dw offset int_35
dw offset int_36
dw offset int_37
dw offset int_38
dw offset int_39
dw offset int_3a
dw offset int_3b
dw offset int_3c
dw offset int_3d
dw offset int_3e
dw offset int_3f
dw offset int_40
dw offset int_41
dw offset int_42
dw offset int_43
dw offset int_44
dw offset int_45
dw offset int_46
dw offset int_47
dw offset int_48
dw offset int_49
dw offset int_4a
dw offset int_4b
dw offset int_4c
dw offset int_4d
dw offset int_4e
dw offset int_4f
dw offset int_50
dw offset int_51
dw offset int_52
dw offset int_53
dw offset int_54
dw offset int_55
dw offset int_56
dw offset int_57
dw offset int_58
dw offset int_59
dw offset int_5a
dw offset int_5b
dw offset int_5c
dw offset int_5d
dw offset int_5e
dw offset int_5f
dw offset int_60
dw offset int_61
dw offset int_62
dw offset int_63
dw offset int_64
dw offset int_65
dw offset int_66
dw offset int_67
dw offset int_68
dw offset int_69
dw offset int_6a
dw offset int_6b
dw offset int_6c
dw offset int_6d
dw offset int_6e
dw offset int_6f
dw offset int_70
dw offset int_71
dw offset int_72
dw offset int_73
dw offset int_74
dw offset int_75
dw offset int_76
dw offset int_77
dw offset int_78
dw offset int_79
dw offset int_7a
dw offset int_7b
dw offset int_7c
dw offset int_7d
dw offset int_7e
dw offset int_7f
dw offset int_80
dw offset int_81
dw offset int_82
dw offset int_83
dw offset int_84
dw offset int_85
dw offset int_86
dw offset int_87
dw offset int_88
dw offset int_89
dw offset int_8a
dw offset int_8b
dw offset int_8c
dw offset int_8d
dw offset int_8e
dw offset int_8f
dw offset int_90
dw offset int_91
dw offset int_92
dw offset int_93
dw offset int_94
dw offset int_95
dw offset int_96
dw offset int_97
dw offset int_98
dw offset int_99
dw offset int_9a
dw offset int_9b
dw offset int_9c
dw offset int_9d
dw offset int_9e
dw offset int_9f
dw offset int_a0
dw offset int_a1
dw offset int_a2
dw offset int_a3
dw offset int_a4
dw offset int_a5
dw offset int_a6
dw offset int_a7
dw offset int_a8
dw offset int_a9
dw offset int_aa
dw offset int_ab
dw offset int_ac
dw offset int_ad
dw offset int_ae
dw offset int_af
dw offset int_b0
dw offset int_b1
dw offset int_b2
dw offset int_b3
dw offset int_b4
dw offset int_b5
dw offset int_b6
dw offset int_b7
dw offset int_b8
dw offset int_b9
dw offset int_ba
dw offset int_bb
dw offset int_bc
dw offset int_bd
dw offset int_be
dw offset int_bf
dw offset int_c0
dw offset int_c1
dw offset int_c2
dw offset int_c3
dw offset int_c4
dw offset int_c5
dw offset int_c6
dw offset int_c7
dw offset int_c8
dw offset int_c9
dw offset int_ca
dw offset int_cb
dw offset int_cc
dw offset int_cd
dw offset int_ce
dw offset int_cf
dw offset int_d0
dw offset int_d1
dw offset int_d2
dw offset int_d3
dw offset int_d4
dw offset int_d5
dw offset int_d6
dw offset int_d7
dw offset int_d8
dw offset int_d9
dw offset int_da
dw offset int_db
dw offset int_dc
dw offset int_dd
dw offset int_de
dw offset int_df
dw offset int_e0
dw offset int_e1
dw offset int_e2
dw offset int_e3
dw offset int_e4
dw offset int_e5
dw offset int_e6
dw offset int_e7
dw offset int_e8
dw offset int_e9
dw offset int_ea
dw offset int_eb
dw offset int_ec
dw offset int_ed
dw offset int_ee
dw offset int_ef
dw offset int_f0
dw offset int_f1
dw offset int_f2
dw offset int_f3
dw offset int_f4
dw offset int_f5
dw offset int_f6
dw offset int_f7
dw offset int_f8
dw offset int_f9
dw offset int_fa
dw offset int_fb
dw offset int_fc
dw offset int_fd
dw offset int_fe
dw offset int_ff