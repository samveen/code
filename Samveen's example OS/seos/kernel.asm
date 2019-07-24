; The kernel
; Copyright (c) 2003 onwards, Samveen S. Gulati

	org 0000h
	jmp start_of_kernel

    ; Message Strings
    start_message db "Starting SEOS", 0xa
		  db "Copyright Samveen S. Gulati", 0xa, 0xa
		  db "Thanks to :",0xa
		  db 20h,20h,20h,20h, "Dr. V. S. Dixit: Head of Department, CS, ARSD",0xa
		  db 20h,20h,20h,20h, "Mr. V. B. Singh: Head of Department, CS, DCAC",0xa
		  db 20h,20h,20h,20h, "friends and all the others who helped.", 0xa
		  db 0

    halt_command_message db "System halted",0xa,0
    interpreter_not_found_message db "Cannot find command interpreter",0xa,0
    interpreter_not_exec_message  db "Cannot exec command interpreter",0xa,0


    ; Location of the disk driver buffer and its offset to store the data before
    ; passing it to the calling procedure
    disk_buffer_seg dw 0h
    disk_buffer dd 0h



    ; name of the interpreter on the floppy
    interpreter_name db "interpret.com",0


;----------------
;include the output functions file
include "output_functions.ASM"
;----------------

;----------------
;include the input functions file
include "input_functions.ASM"
;----------------

;----------------
;include the output functions file
include "file_functions.ASM"
;----------------

;----------------
;include the interrupt handler file
include "interrupt_handler.ASM"
;----------------



start_of_kernel:

	;setting ds to cs
	push cs
	pop ds

	push cs
	pop es

	; setting up interrupt 40h,41h and 42h
	; The location of the interrupt vector is the starting of memory. Each
	; vector address is made up of two parts :-
	;      1) segment number of the handler
	;      2) address of the handler inside the segment
	; These two values are stored in a dword with the low word containing the
	; address and the high word containing the segment number. As the interrupt
	; vector location is calculated from the interrupt number therefore each
	; vector location is 4 * interrupt number. Therefore interrupt 40h (64d)
	; will be stored at memory location 100h (256d). In memory it goes like
	; lo_add:hi_add:lo_seg:hi_seg . When moved into an extended register like
	; edx it becomes of the form segment:address in hiword:loword.
interrupt_handlers_setup:
	push 0
	pop gs				 ; segment of interrupt vector 40h
	mov cx,1000h			 ; segment of interrupt handler routine

	; interrupt handler 40h
	mov si, 100h			 ; address of interrupt vector 40h (40h * 4 = 100h)
	mov dx, interrupt_handler_output ; address of interrupt handler routine
	mov [gs:si+2],cx
	mov [gs:si],dx

	; interrupt handler 41h
	mov si, 104h			 ; address of interrupt vector 41h (41h * 4 = 104h)
	mov dx, interrupt_handler_input  ; address of interrupt handler routine
	mov [gs:si+2],cx
	mov [gs:si],dx

	; interrupt handler 42h
	mov si, 108h			 ; address of interrupt vector 42h (42h * 4 = 108h)
	mov dx, interrupt_handler_FS	 ; address of interrupt handler routine
	mov [gs:si+2],cx
	mov [gs:si],dx


	; setting up the disk buffer area and its seg
	mov eax,0
	mov [disk_buffer],eax
	mov ax,8000h
	mov [disk_buffer_seg],ax

	;starting message
	mov edx, start_message
	mov ah,5
	int 40h

	; load interpreter
	mov edx, interpreter_name
	mov ebx, 0h  ; ebx = offset
	mov cx,5000h ; buffer segment ; 16 bit
	mov ah,2     ; load file
	int 42h

	; was interpreter found and loaded successfully
	cmp cx, 0
	jz problem_in_interpreter

	; interpreter found. execute interpreter
	push ds
	call far 5000h:0000h
	pop ds

	jmp system_halt

problem_in_interpreter:

	cmp al,0
	jz interpreter_not_found
	mov edx, interpreter_not_exec_message
	mov ah,5
	int 40h

	jmp system_halt

interpreter_not_found: ; interpreter not found give error messege


	mov edx, interpreter_not_found_message
	mov ah,5
	int 40h

system_halt: ; halt system
	mov edx, halt_command_message
	mov ah,5
	int 40h

	hlt

;last data
end_byte db 0xff
