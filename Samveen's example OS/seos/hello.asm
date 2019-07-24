; Code for the hello world example executable
; Copyright (c) 2003 onwards, Samveen S. Gulati

; The SEOS executable format lacks a signature
; This is purely machine instructions and data
	jmp _main


	message db "hello, world",0
_main:
	push cs
	pop ds

	mov ah,5  ; print message
	mov edx,message
	int 40h

	mov ah,0  ; get char
	int 41h

	retf
