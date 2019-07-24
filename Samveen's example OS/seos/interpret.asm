; Code for the main command interpreter executable
; Copyright (c) 2003 onwards, Samveen S. Gulati

; The SEOS executable format lacks a signature
; This is purely machine instructions and data
	jmp _main

start_interpreter_byte db 0xFF

; this is the commands table
; it is used both for parsing the commands as well as the help string
; to display for the help command
command_table_start:
command_table_clear db	"clear", ": clears the screen"		      , 0xd ; help_string + newline
command_table_exit  db	"exit ", ": exits from the interpreter"       , 0xd ; help_string + newline
command_table_help  db	"help ", ": help for commands"		      , 0xd ; help_string + newline
command_table_pgup  db	"pgup ", ": show previous screenful of output", 0xd ; help_string + newline
command_table_reset db	"reset", ": reset the system(soft)"	      , 0xd ; help_string + newline
command_table_exec  db	"exec ", ": execute a file"		      , 0xd ; help_string + newline
command_table_list  db	"list ", ": list files on fs"		      , 0xd ; help_string + newline
command_table_type  db	"type ", ": load a file and display it"       , 0xd ; help_string + newline
command_table_end db 0


;data
   command_prompt db "SEOS :",0
   command_line:
	times 255 db 0x00

   file_name_prompt db "File Name :",0
   file_name:
	times 64  db 0x00

   unknown_command_message db "unknown command",0xd,0
   not_found_string db " not found",0xd,0
   not_exec_string  db " not executable",0xd,0


; the main command interpreter
_main:

	push cs
	pop ds

loop_main: ; The main loop
	mov ah,5
	mov edx,command_prompt
	int 40h

	mov edx, command_line
	mov cl, 254
	mov ah,1
	int 41h

	call parse_command_line

	call exec_command_line

	cmp cl,0
	jnz loop_main

	retf


;---------------------------------
; function to parse the command line that is convert the string
; in command_line to a number that is then passed to the
; exec_command_line function
; leaves the returned value in dx

parse_command_line:

	cmp byte [command_line], 0
	jnz parse_command_line_not_null
	mov dx, 0
	jmp parse_command_line_cleanup

parse_command_line_not_null:
	mov si, command_line
	lodsd

	;test for commands
	cmp dword [command_table_clear], eax
	jz parse_command_line_clear_command

	cmp dword [command_table_exit], eax
	jz parse_command_line_exit_command

	cmp dword [command_table_help], eax
	jz parse_command_line_help_command

	cmp dword [command_table_pgup], eax
	jz parse_command_line_pgup_command

	cmp dword [command_table_reset], eax
	jz parse_command_line_reset_command

	cmp dword [command_table_exec], eax
	jz parse_command_line_exec_command

	cmp dword [command_table_list],eax
	jz parse_command_line_list_command

	cmp dword [command_table_type],eax
	jz parse_command_line_type_command

	mov dx, -1
	jmp parse_command_line_cleanup



parse_command_line_clear_command:
	mov dx, 1
	jmp parse_command_line_cleanup

parse_command_line_exit_command:
	mov dx, 2
	jmp parse_command_line_cleanup

parse_command_line_help_command:
	mov dx, 3
	jmp parse_command_line_cleanup

parse_command_line_pgup_command:
	mov dx ,4
	jmp parse_command_line_cleanup

parse_command_line_reset_command:
	mov dx ,5
	jmp parse_command_line_cleanup

parse_command_line_exec_command:
	mov dx ,6
	jmp parse_command_line_cleanup

parse_command_line_list_command:
	mov dx ,7
	jmp parse_command_line_cleanup

parse_command_line_type_command:
	mov dx ,8
	jmp parse_command_line_cleanup

parse_command_line_cleanup:

	ret
;---------------------------------


;---------------------------------
; function to exec the command line that is has been
; parsed by the parse_command_line function into a number
retval db 0h
exec_command_line:

	mov [retval],1
	cmp dx,1
	jb exec_command_line_cleanup
	je exec_command_line_clear_command
	cmp dx,3
	jb exec_command_line_exit_command
	je exec_command_line_help_command
	cmp dx,5
	jb exec_command_line_pgup_command
	je exec_command_line_reset_command
	cmp dx,7
	jb exec_command_line_exec_command
	je exec_command_line_list_command
	cmp dx,9
	jb exec_command_line_type_command

	mov edx, unknown_command_message
	mov ah,5
	int 40h
	jmp exec_command_line_cleanup

exec_command_line_clear_command:
	xor ah, ah
	int 40h
	jmp exec_command_line_cleanup

exec_command_line_exit_command:
	mov [retval],0

	jmp exec_command_line_cleanup

exec_command_line_help_command:
	mov edx,command_table_start
	mov ah, 5
	int 40h

	jmp exec_command_line_cleanup

exec_command_line_pgup_command:
	mov ah,7 ;call show_command_history
	int 40h

	mov ah,0
	int 41h

	mov ah,6
	int 40h
	jmp exec_command_line_cleanup

exec_command_line_reset_command:
	int 19h
	jmp exec_command_line_cleanup

exec_command_line_exec_command:
	mov ah,5
	mov edx,file_name_prompt
	int 40h

	mov edx, file_name
	mov cl, 63
	mov ah,1
	int 41h

	cmp byte [file_name] , 0
	jz exec_command_line_exec_command_end

	mov ebx, 0   ; ebx = offset
	mov cx,7000h ; buffer segment ; 16 bit
	mov ah,2     ; load file
	int 42h


	cmp cx, 0
	jnz exec_command_line_exec_command_found


	mov edx, file_name
	mov ah,5
	int 40h

	cmp al, 0
	jnz exec_command_line_exec_command_not_exec

	mov ah,5
	mov edx, not_found_string
	int 40h

	jmp exec_command_line_exec_command_end
exec_command_line_exec_command_not_exec:
	mov ah,5
	mov edx, not_exec_string
	int 40h

	jmp exec_command_line_exec_command_end

exec_command_line_exec_command_found:
	push ds
	call far 7000h:0000
	pop ds
exec_command_line_exec_command_end:

	jmp exec_command_line_cleanup

exec_command_line_list_command:
	mov ah,1
	int 42h

	jmp exec_command_line_cleanup

exec_command_line_type_command:
	mov ah,5
	mov edx,file_name_prompt
	int 40h

	mov edx, file_name
	mov cl, 63
	mov ah,1
	int 41h

	cmp byte [file_name] , 0
	jz exec_command_line_type_command_end

	mov ebx, 0   ; ebx = offset
	mov cx,7000h ; buffer segment ; 16 bit
	mov ah,3     ; load file
	int 42h

	cmp cx, 0
	jnz exec_command_line_type_command_found

	mov edx, file_name
	mov ah,5
	int 40h

	mov ah,5
	mov edx, not_found_string
	int 40h

	jmp exec_command_line_type_command_end

exec_command_line_type_command_found:
	push ds
	; set source
	push 7000h
	pop ds
	mov esi, 0000

	;count is in cx

	cld
display_loop:
	lodsb
	mov ah,4
	int 40h
	loop display_loop

	pop ds
exec_command_line_type_command_end:

	jmp exec_command_line_cleanup

exec_command_line_cleanup:
	mov cl, [retval]

	ret
;---------------------------------


;last data
end_interpreter_byte db 0xFF
