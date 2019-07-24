; Input functions
; Copyright (c) 2003 onwards, Samveen S. Gulati

;    getchar : gets a char with echo
;    gets    : gets a string of chars till 255 or \n


;----------------
    ;get a char on screen with echo, the result is
    ; in AX same as the in 16h
getchar:
	mov ah, 0h		 ;bios: get character
	int 16h
	call putchar
	ret
;----------------


;----------------
    ; get a character string as input with echo
    ; char 0xd (enter) terminates input
    ; params:
    ;	 edx: address of string
    ;	 cl: length of string buffer (including terminating 0)
gets:
	pusha

	mov edi,edx
	mov bl,cl
	cld
gets_loop:
	call getchar
	call flush_kernel_video_buffer

	cmp al, 0xd ; check for enter
	jz gets_loop_break

	cmp al, 0x8	       ; check for backspace
	jnz gets_loop_not_bs   ; if not bs, do your stuff

	; Code for Backspace handling

	cmp cl, bl   ; check if bs on start of string
	je gets_loop ; if so go to start of loop and begin again

	dec edi ; if bs and not start of string dec di so as to store
	       ; the next at the previous location

	inc cl ; increment cx by 2 to cover
	inc cl ; up for the loop statement+bs

	jmp gets_loop_continue

gets_loop_not_bs:
	mov [gs:edi], al
	inc edi
gets_loop_continue:
	loop gets_loop

gets_loop_break:    ; terminate string with zero
	mov [gs:edi],byte 0h

    ; cleanup
	popa
	ret
;----------------
