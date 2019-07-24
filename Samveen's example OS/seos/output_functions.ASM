; Location and explaination of the Output Functions And Variables
; Copyright (c) 2003 onwards, Samveen S. Gulati

; All output is being "cooked" before it is to be sent on to the screen.
; "Cooking" entails that all the special characters that are to be put
; on screen should be converted to their proper values eg. ^M to \n etc.
; The cooking is done on the kernel's kernel_video_buffer area
; therefore all the output functions will work through this area for
; outputting. At the end of all system calls that change the screen the
; kernel_video_buffer area will be transferred to the video buffer of the
; machine.
; The functions that are used for outputting are
;    putchar  : put a char on screen
;    putbyte  : put a byte on screen
;    putword  : put a word on screen
;    putdword : put a dword on screen
;    puts     : put a string on screen
;    clear_screen : clear the screen (fill it with 0x0000 as of now)
;    flush_kernel_video_buffer: puts the KVB contents into the video memory
;    show_kernel_history_buffer: puts the KVBH contents into the video memory


; Data that may/will have to be private for the
; output routines
kernel_video_buffer_history: ; a screenful history of the
     times 2000 dw 0000h     ; kernel video buffer
kernel_video_buffer:   ; The kernels private video buffer
    times 2000 dw 0000h 	      ; this is sent to screen at the end
    kernel_video_buffer_end db 0x0    ; of every output system call

    cursor_location_custom dw 0 ; address of cursor in the video buffer
    character_color db 07h	; color to which output chars will be set (Def : grey on black)


;----------------
; put a char on screen
; This function is the corner stone of the output setup. It is called by all the
; other outputting functions to put the chars on screen. This strategy may not
; be very efficient in the case of strings which may cause some slowdown as
; compared to direct access of the kernel video buffer by the puts procedure but
; then the cooking code will not have to be inserted into the puts procedure

; parameters
;  al: char to put
putchar:
	pusha

	mov di, [cursor_location_custom]
	cmp di, 0xfa0 ;4000 (bytes)
	jb putchar_no_buffer_overflow

	; move all the data from 2nd line onwards to first
	; line onwards (includes kernel video buffer history)
	mov di,0
	add di, kernel_video_buffer_history

	mov si, 0xa0 ; 160 (bytes)
	add si, kernel_video_buffer_history


	mov cx , 0xf50 ;4000 - 80 (words)
	rep movsw

	; blank the last line
	push ax
	xor ax,ax
	mov cx, 0x50
	rep stosw

	pop ax
	mov di,0xf00 ; start of last line: 4000 - 160

putchar_no_buffer_overflow:

	add di, kernel_video_buffer
	cld

	cmp al, 0x20 ;check for unprintable chars
	jb putchar_control

	mov ah,[character_color] ; al is char and ah is color
	stosw
	jmp putchar_cleanup

putchar_control:
	cmp al, 0xd
	je putchar_control_newline
	cmp al, 0xa
	je putchar_control_newline

	cmp al, 8
	je putchar_control_backspace

	push ax
	mov al,'^'
	stosw
	pop ax
	add al,0x40
	stosw
	jmp putchar_cleanup

putchar_control_backspace:
	dec di
	dec di
	xor ax,ax
	stosw
	dec di
	dec di
	jmp putchar_cleanup

putchar_control_newline:
	mov ax,[cursor_location_custom]
	mov bl,0xa0
	div bl
	mov al,0xa0
	sub al,ah
	xor ah,ah
	add di,ax
	jmp putchar_cleanup

putchar_cleanup:
	sub di,kernel_video_buffer
	mov [cursor_location_custom],di

	popa
	ret
;----------------


;----------------
    ;put a byte on screen as hex.
    ;range of byte is 0x0 - 0xff
    ;params
    ;	value to display - dl

putbyte:
	pusha
	mov cx, 2

	xor ax,ax
putbyte_loop:
	mov al,dl
	shr al,4
	cmp al,0xa
	jb putbyte_num ;if less than 0xa->1-9

	add al,'a'-10
	jmp putbyte_put
putbyte_num:
	add al, '0'
putbyte_put:
	call putchar

	shl dl,4
	loop putbyte_loop

	popa
	ret
;----------------


;----------------
    ;put a word on screen as hex.
    ;range of word is 0x0 - 0xffff
    ;params
    ;	value to display - dx

putword:
	pusha
	mov cx, 2

	xor ax,ax
putword_loop:
	push dx
	mov dl,dh
	call putbyte
	pop dx
	shl dx,8
	loop putword_loop

	popa
	ret
;----------------


;----------------
    ;put a dword on screen as hex.
    ;range of dword is 0x0 - 0xffffffff
    ;params
    ;	value to display - edx

putdword:
	pusha
	mov cx, 2

putdword_loop:
	push edx
	shr edx,16
	call putword
	pop edx
	shl edx,16
	loop putdword_loop

	popa
	ret
;----------------


;----------------
    ; print char string on screen
    ; params:
    ;	 address of string (dx)
puts:
	pusha

	mov cx,1
	mov esi,edx

puts_loop:
	;cld
	;lodsb
	mov al , [gs:esi]
	inc esi

	cmp al,0
	jz continue_puts_loop

	call putchar

	inc cx

continue_puts_loop:
	loop puts_loop

	popa
	ret
;----------------


;----------------
;subroutine to clear the screen
clear_screen:
	pusha

	mov edi,0
	add edi, kernel_video_buffer

	xor ax,ax
	cld

	mov cx,2000
	rep stosw

	mov [cursor_location_custom] ,ax
	popa

	ret
;----------------


;----------------
; subroutine to put the kernel history video buffer
; into the machine's video buffer
show_kernel_history_buffer:
	pusha

	; setup source ds:si = data_seg:kernel_video_buffer
	mov esi,kernel_video_buffer_history

	; setup destination es:di= b800:0000
	push es
	mov cx , 0xb800
	mov es , cx
	mov edi,0

	; 2000 words to move
	mov cx, 2000

	cld
	rep movsw

	pop es
	popa
	ret
;----------------


;----------------
; subroutine to move the kernel custom video buffer
; into the machine's video buffer
flush_kernel_video_buffer:
	pusha

	; setup source ds:si = data_seg:kernel_video_buffer
	mov si,kernel_video_buffer

	; setup destination es:di= b800:0000
	push es
	mov cx , 0xb800
	mov es , cx
	mov di,0

	; 2000 words to move
	mov cx, 2000

	cld
	rep movsw

	pop es
	popa
	ret
;----------------
