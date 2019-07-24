; Interrupt handlers of the operating system
; Copyright (c) 2003 onwards, Samveen S. Gulati

; Interrupt 40h   : Output functions
; values for functions
;   0: clear screen
;   1: putbyte (byte->dl)
;   2: putword (word->dx)
;   3: putdword (dword->edx)
;   4: putchar (char->al)
;   5: puts (dword address->edx)
;   6: refresh the screen
;   7: put history buffer on screen



; Interrupt 41h   : Input functions
; values for functions
;   0: getchar returns value in ax
;   1: gets (dest_address->edx, maxlen->cl)



; Interrupt 42h   : filesystem functions
; values for functions
;   0: load_entry (index->dl)
;   1: fs_list_files ()
;   2: fs_load_file (name_address->edx) (retval->al)


;data
    interrupt_name_message db "INT",0
    function_not_implemented_message db "function not implemented:",0

;code
interrupt_handler_output:
	push ds  ; save incoming data segment
	push gs  ; save gs

	; copy ds to gs
	push ds
	pop gs

	; copy cs to ds
	push cs   ; // setting up ds to point to kernel data area
	pop ds

	cmp ah,1
	jb int_40_function_0
	je int_40_function_1
	cmp ah,3
	jb int_40_function_2
	je int_40_function_3
	cmp ah,5
	jb int_40_function_4
	je int_40_function_5
	cmp ah,7
	jb int_40_function_6
	je int_40_function_7

	push edx
	mov edx,interrupt_name_message
	call puts
	mov dl,40h
	call putbyte
	mov edx,function_not_implemented_message
	call puts
	mov dl,ah
	call putbyte
	pop edx
	jmp interrupt_handler_output_video_buffer_flush

int_40_function_0:  ;clear
	call clear_screen
	jmp interrupt_handler_output_video_buffer_flush

int_40_function_1:  ;putbyte
	call putbyte
	jmp interrupt_handler_output_video_buffer_flush

int_40_function_2:  ;putword
	call putword
	jmp interrupt_handler_output_video_buffer_flush

int_40_function_3:  ;putdword
	call putdword
	jmp interrupt_handler_output_video_buffer_flush

int_40_function_4:  ;putchar
	call putchar
	jmp interrupt_handler_output_video_buffer_flush

int_40_function_5:  ;puts
	call puts
	jmp interrupt_handler_output_video_buffer_flush

int_40_function_6:  ;refresh the screen
	jmp interrupt_handler_output_video_buffer_flush

int_40_function_7:  ;put history buffer on screen
	call show_kernel_history_buffer
	jmp interrupt_handler_output_cleanup

interrupt_handler_output_video_buffer_flush:

	; display any changes in video buffer
	; on to the screen
	call flush_kernel_video_buffer


interrupt_handler_output_cleanup:
	pop gs
	pop ds
	iret
;----------------


;----------------
interrupt_handler_input:
	push ds  ; save incoming data segment
	push gs  ; save gs

	; copy ds to gs
	push ds
	pop gs

	; copy cs to ds
	push cs   ; // setting up ds to point to kernel data area
	pop ds

	cmp ah,1
	jb int_41_function_0
	je int_41_function_1


	mov edx,interrupt_name_message
	call puts
	mov dl,41h
	call putbyte
	mov edx,function_not_implemented_message
	call puts
	mov dl,ah
	call putbyte
	jmp interrupt_handler_input_cleanup

int_41_function_0:  ;getchar
	call getchar
	jmp interrupt_handler_input_cleanup
int_41_function_1:  ;gets
	call gets
	jmp interrupt_handler_input_cleanup


interrupt_handler_input_cleanup:

	pop gs
	pop ds
	iret
;----------------


interrupt_handler_FS:
	push ds  ; save incoming data segment
	push gs  ; save gs

	; copy ds to gs
	push ds
	pop gs

	; copy cs to ds
	push cs   ; // setting up ds to point to kernel data area
	pop ds

	cmp ah,1
	jb int_42_function_0
	je int_42_function_1
	cmp ah,3
	jb int_42_function_2
	je int_42_function_3


	mov edx,interrupt_name_message
	call puts
	mov dl,42h
	call putbyte
	mov edx,function_not_implemented_message
	call puts
	mov dl,ah
	call putbyte
	jmp interrupt_handler_FS_cleanup

int_42_function_0:  ;load_entry
	call load_entry
	jmp interrupt_handler_FS_cleanup

int_42_function_1: ;fs_list_files
	call fs_list_files
	jmp interrupt_handler_FS_cleanup

int_42_function_2:  ;fs_exec_file

	call fs_exec_file
	jmp interrupt_handler_FS_cleanup

int_42_function_3:  ;fs_load_file

	call fs_load_file
	jmp interrupt_handler_FS_cleanup

interrupt_handler_FS_cleanup:
	pop gs
	pop ds
	iret
;----------------
