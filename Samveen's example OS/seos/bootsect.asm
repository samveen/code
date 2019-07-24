; location and explaination of boot sector
; Copyright (c) 2003 onwards, Samveen S. Gulati

	    ; Goto segment 07C0
	    jmp 07C0h:start

    boot_message db "loading SEOS kernel.......",0

    start:
	    ; Update the segment registers
	    mov ax, cs
	    mov ds, ax
	    mov es, ax


	    mov si, boot_message
    message:		; Print boot_message to screen.
	    lodsb		; load byte at ds:si into al
	    cmp al, 0		; test if character is 0 (end)
	    jz reset		; jump to next code sequence
	    mov ah,0eh		; put character
	    mov bx,0007 	; attribute
	    int 0x10		; call BIOS
	    jmp message

    reset:		; Reset the floppy drive
	    mov ax, 0		;
	    mov dl, 0		; Drive=0 (=A)
	    int 13h		;
	    jc reset		; ERROR => reset again


    read:		; Read in image from the floppy into memory
	    mov ax, 1000h	; Setting destination
	    mov es, ax		; es:bx = 1000:0000
	    mov bx, 0		;

	    mov ah, 2		; Load disk data to ES:BX
	    mov al, 72		; Load 72 sectors(36864)
	    mov ch, 0		; Cylinder=0
	    mov cl, 2		; Sector=2
	    mov dh, 0		; Head=0
	    mov dl, 0		; Drive=0
	    int 13h		; Read!

	    jc read		; ERROR => Try again

	    ; Transferring control to the loaded kernel
	    jmp 1000h:0000

	    times 0x1fe-$ db 00h; Pad the boot sector to correct size

  db 55h,0aah ;boot signature
