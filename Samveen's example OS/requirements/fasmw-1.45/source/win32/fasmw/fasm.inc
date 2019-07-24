
; flat assembler interface for Win32 IDE
; Copyright (c) 1999-2003, Tomasz Grysztar.
; All rights reserved.

flat_assembler:

	mov	edx,[esp+4]
	mov	[input_file],edx
	call	convert_path
	invoke	GetFullPathName,buffer,1000h,full_path_buffer,param_buffer
	mov	edi,[param_buffer]
	xor	al,al
	stosb
	invoke	SetCurrentDirectory,full_path_buffer
	mov	[hfile],0
	invoke	GlobalAlloc,GMEM_MOVEABLE,1
	mov	[hmem_display],eax
	invoke	GlobalLock,[hmem_display]
	mov	byte [eax],0
	invoke	GlobalUnlock,[hmem_display]
	mov	[display_size],1
	mov	[error_data_size],0
	mov	[memory_start],0
	mov	eax,[compiler_memory]
	shl	eax,10
	jz	out_of_memory
      allocate_memory:
	mov	edx,eax
	shr	edx,3
	mov	ecx,eax
	sub	ecx,edx
	mov	[memory_end],ecx
	mov	[additional_memory_end],edx
	invoke	VirtualAlloc,0,eax,MEM_COMMIT,PAGE_READWRITE
	or	eax,eax
	jnz	memory_allocated
	mov	eax,[additional_memory_end]
	shl	eax,2
	cmp	eax,4000h
	jb	out_of_memory
	jmp	allocate_memory
      memory_allocated:
	mov	[memory_start],eax
	mov	[code_start],eax
	add	eax,[memory_end]
	mov	[memory_end],eax
	mov	[additional_memory],eax
	add	[additional_memory_end],eax

	invoke	SendMessage,[hwnd_progress],PBM_SETPOS,0,0
	invoke	SetThreadPriority,[hthread],[compiler_priority]
	invoke	GetTickCount
	mov	[start_time],eax
	call	preprocessor
	invoke	SendMessage,[hwnd_progress],PBM_SETPOS,1,0
	call	parser
	invoke	SendMessage,[hwnd_progress],PBM_SETPOS,2,0
	call	assembler
	invoke	SendMessage,[hwnd_progress],PBM_SETPOS,3,0

	mov	edx,[input_file]
	call	convert_path
	push	edi
	lea	edi,[eax-1]
      find_extension:
	dec	eax
	cmp	eax,buffer
	jb	extension_found
	cmp	byte [eax],'\'
	je	extension_found
	cmp	byte [eax],'/'
	je	extension_found
	cmp	byte [eax],'.'
	jne	find_extension
	mov	edi,eax
      extension_found:
	cmp	[output_format],2
	je	exe_extension
	jb	com_extension
	cmp	[output_format],4
	je	obj_extension
	cmp	[output_format],5
	je	o_extension
	cmp	[output_format],3
	jne	no_extension
	bt	[format_flags],8
	jnc	exe_extension
	mov	eax,'.dll'
	jmp	make_extension
      com_extension:
	mov	eax,'.com'
	jmp	make_extension
      obj_extension:
	mov	eax,'.obj'
	jmp	make_extension
      o_extension:
	mov	eax,'.o'
	bt	[format_flags],0
	jnc	make_extension
      no_extension:
	xor	eax,eax
	jmp	make_extension
      exe_extension:
	mov	eax,'.exe'
      make_extension:
	xchg	eax,[edi]
	scasd
	mov	byte [edi],0
	scasb
	mov	esi,edi
	stosd
	sub	edi,9
	xor	eax,eax
      adapt_case:
	mov	al,[esi]
	or	al,al
	jz	adapt_next
	invoke	CharLower,eax
	cmp	al,[esi]
	je	adapt_ok
	sub	byte [edi],20h
      adapt_ok:
	inc	esi
      adapt_next:
	inc	edi
	cmp	byte [edi],0
	jne	adapt_case
	pop	edi
	mov	[output_file],buffer

	call	formatter
	invoke	SendMessage,[hwnd_progress],PBM_SETPOS,4,0
	invoke	GetTickCount
	sub	eax,[start_time]
	mov	[total_time],eax
	xor	al,al

exit_program:
	movzx	eax,al
	push	eax
	mov	eax,[memory_start]
	or	eax,eax
	jz	memory_ok
	invoke	VirtualFree,eax,0,MEM_RELEASE
	mov	[memory_start],0
      memory_ok:
	mov	eax,[hfile]
	or	eax,eax
	jz	handle_ok
	invoke	CloseHandle,eax
      handle_ok:
	invoke	PostMessage,[hwnd_compiler],WM_COMMAND,IDOK,0
	invoke	ExitThread

open:
	call	convert_path
	invoke	CreateFile,buffer,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0
	cmp	eax,-1
	je	file_error
	mov	[hfile],eax
	mov	ebx,eax
	clc
	ret
    file_error:
	stc
	ret
    convert_path:
	push	ebx esi edi
	mov	esi,edx
	mov	edi,buffer
      copy_path:
	lods	byte [esi]
	cmp	al,'%'
	je	environment_variable
	stos	byte [edi]
	or	al,al
	jnz	copy_path
	cmp	edi,buffer+1000h
	ja	out_of_memory
	mov	eax,edi
	pop	edi esi ebx
	ret
      environment_variable:
	mov	ebx,esi
      find_variable_end:
	lods	byte [esi]
	or	al,al
	jz	not_environment_variable
	cmp	al,'%'
	jne	find_variable_end
	mov	byte [esi-1],0
	invoke	GetEnvironmentVariable,ebx,full_path_buffer,1000h
	mov	eax,buffer+1000h
	sub	eax,edi
	invoke	GetPrivateProfileString,_section_environment,ebx,full_path_buffer,edi,eax,ini_path
	add	edi,eax
	mov	byte [esi-1],'%'
	jmp	copy_path
      not_environment_variable:
	mov	al,'%'
	stos	byte [edi]
	mov	esi,ebx
	jmp	copy_path
create:
	invoke	CreateFile,edx,GENERIC_WRITE,0,0,CREATE_ALWAYS,0,0
	cmp	eax,-1
	je	file_error
	mov	ebx,eax
	clc
	ret
write:
	invoke	WriteFile,ebx,edx,ecx,bytes_count,0
	or	eax,eax
	jz	file_error
	clc
	ret
read:
	mov	ebp,ecx
	invoke	ReadFile,ebx,edx,ecx,bytes_count,0
	or	eax,eax
	jz	file_error
	cmp	ebp,[bytes_count]
	jne	file_error
	clc
	ret
close:
	cmp	ebx,[hfile]
	jne	close_handle
	mov	[hfile],0
      close_handle:
	invoke	CloseHandle,ebx
	ret
lseek:
	movzx	eax,al
	invoke	SetFilePointer,ebx,edx,0,eax
	cmp	eax,-1
	je	file_error
	ret

display_block:
	push	edi
	push	ecx
	add	ecx,[display_size]
	invoke	GlobalReAlloc,[hmem_display],ecx,GMEM_MOVEABLE
	or	eax,eax
	jz	out_of_memory
	mov	[hmem_display],eax
	invoke	GlobalLock,[hmem_display]
	add	eax,[display_size]
	lea	edi,[eax-1]
	pop	ecx
	add	[display_size],ecx
	rep	movsb
	xor	al,al
	stosb
	invoke	GlobalUnlock,[hmem_display]
	pop	edi
	ret
fatal_error:
	pop	[error_message]
	mov	al,0FFh
	jmp	exit_program
assembler_error:
	call	flush_display_buffer
	pop	[error_message]
	mov	ebx,[current_line]
	xor	ecx,ecx
	xor	edx,edx
      get_error_lines:
	test	byte [ebx+7],80h
	jz	error_lines_ok
	cmp	dword [ebx],edx
	je	get_next_error_line
	inc	ecx
	mov	edx,[ebx]
	mov	edx,[edx-5]
	mov	eax,[edx+4]
	add	eax,[ebx+4]
	and	eax,7FFFFFFFh
	push	eax
	mov	edx,[edx]
	push	edx
      get_next_error_line:
	mov	ebx,[ebx+8]
	jmp	get_error_lines
      error_lines_ok:
	inc	ecx
	mov	eax,[ebx+4]
	and	eax,7FFFFFFFh
	push	eax
	mov	edx,[ebx]
	push	edx
	mov	ebx,ecx
	inc	ecx
	shl	ecx,3
	mov	[error_data_size],ecx
	invoke	GlobalAlloc,GMEM_MOVEABLE,ecx
	mov	[hmem_error_data],eax
	invoke	GlobalLock,[hmem_error_data]
	mov	[eax],ebx
	invoke	GlobalUnlock,[hmem_error_data]
	xor	ebx,ebx
      store_error_lines:
	pop	edx
	call	convert_path
	invoke	GetFullPathName,buffer,1000h,full_path_buffer,param_buffer
	inc	eax
	mov	esi,eax
	add	eax,[error_data_size]
	invoke	GlobalReAlloc,[hmem_error_data],eax,GMEM_MOVEABLE
	invoke	GlobalLock,[hmem_error_data]
	mov	edi,eax
	add	edi,[error_data_size]
	mov	ecx,esi
	mov	esi,full_path_buffer
	rep	movsb
	pop	edx
	mov	[eax+8+ebx*8+4],edx
	sub	edi,eax
	xchg	[error_data_size],edi
	mov	[eax+8+ebx*8],edi
	mov	esi,[eax]
	invoke	GlobalUnlock,[hmem_error_data]
	inc	ebx
	cmp	ebx,esi
	jb	store_error_lines
	mov	edi,[additional_memory]
	mov	eax,[code_start]
	cmp	eax,[memory_start]
	jne	error_in_preprocessed
	xor	al,al
	stosb
	jmp	instruction_converted
      error_in_preprocessed:
	mov	esi,[current_line]
	add	esi,12
	xor	dl,dl
      convert_instruction:
	lodsb
	cmp	al,1Ah
	je	copy_symbol
	cmp	al,22h
	je	copy_symbol
	cmp	al,3Bh
	je	ignore_preprocessor_symbols
	stosb
	or	al,al
	jz	instruction_converted
	xor	dl,dl
	jmp	convert_instruction
      copy_symbol:
	or	dl,dl
	jz	space_ok
	mov	byte [edi],20h
	inc	edi
      space_ok:
	cmp	al,22h
	je	quoted
	lodsb
	movzx	ecx,al
	rep	movsb
	or	dl,-1
	jmp	convert_instruction
      quoted:
	mov	al,27h
	stosb
	lodsd
	mov	ecx,eax
      copy_quoted:
	lodsb
	stosb
	cmp	al,27h
	jne	quote_ok
	stosb
      quote_ok:
	loop	copy_quoted
	mov	al,27h
	stosb
	or	dl,-1
	jmp	convert_instruction
      ignore_preprocessor_symbols:
	xor	al,al
	stosb
      instruction_converted:
	sub	edi,[additional_memory]
	mov	ebx,[error_data_size]
	lea	eax,[ebx+edi]
	invoke	GlobalReAlloc,[hmem_error_data],eax,GMEM_MOVEABLE
	invoke	GlobalLock,[hmem_error_data]
	mov	ecx,edi
	mov	[eax+4],ebx
	lea	edi,[eax+ebx]
	mov	esi,[additional_memory]
	rep	movsb
	invoke	GlobalUnlock,[hmem_error_data]
	mov	al,1
	jmp	exit_program

make_timestamp:
	invoke	GetSystemTime,systime
	movzx	ecx,[systime.wYear]
	mov	eax,ecx
	sub	eax,1970
	mov	ebx,365
	mul	ebx
	mov	ebp,eax
	mov	eax,ecx
	sub	eax,1969
	shr	eax,2
	add	ebp,eax
	mov	eax,ecx
	sub	eax,1901
	mov	ebx,100
	div	ebx
	sub	ebp,eax
	mov	eax,ecx
	xor	edx,edx
	sub	eax,1601
	mov	ebx,400
	div	ebx
	add	ebp,eax
	movzx	ecx,[systime.wMonth]
	mov	eax,ecx
	dec	eax
	mov	ebx,30
	mul	ebx
	add	ebp,eax
	cmp	ecx,8
	jbe	months_correction
	mov	eax,ecx
	sub	eax,7
	shr	eax,1
	add	ebp,eax
	mov	ecx,8
      months_correction:
	mov	eax,ecx
	shr	eax,1
	add	ebp,eax
	sub	ebp,2
	cmp	ecx,2
	jbe	day_correction_ok
	movzx	ecx,word [systime.wYear]
	test	ecx,11b
	jnz	day_correction_ok
	xor	edx,edx
	mov	eax,ecx
	mov	ebx,100
	div	ebx
	or	edx,edx
	jnz	day_correction
	mov	eax,ecx
	mov	ebx,400
	div	ebx
	or	edx,edx
	jnz	day_correction_ok
      day_correction:
	inc	ebp
      day_correction_ok:
	movzx	eax,[systime.wDay]
	dec	eax
	add	eax,ebp
	mov	ebx,24
	mul	ebx
	movzx	ecx,[systime.wHour]
	add	eax,ecx
	mov	ebx,60
	mul	ebx
	movzx	ecx,[systime.wMinute]
	add	eax,ecx
	mov	ebx,60
	mul	ebx
	movzx	ecx,[systime.wSecond]
	add	eax,ecx
	ret

macro dm string { db string,0 }

include '..\..\errors.inc'
include '..\..\expressi.inc'
include '..\..\preproce.inc'
include '..\..\parser.inc'
include '..\..\assemble.inc'
include '..\..\formats.inc'
include '..\..\x86.inc'

_counter db 8,'00000000'

align 4

memory_start dd ?
memory_end dd ?
additional_memory dd ?
additional_memory_end dd ?
free_additional_memory dd ?
input_file dd ?
output_file dd ?
source_start dd ?
code_start dd ?
code_size dd ?
real_code_size dd ?
start_time dd ?
total_time dd ?
written_size dd ?
display_size dd ?
error_message dd ?
error_data_size dd ?

current_line dd ?
macros_list dd ?
macro_constants dd ?
macro_block dd ?
macro_block_line_number dd ?
macro_embed_level dd ?
struc_name dd ?
anonymous_reverse dd ?
anonymous_forward dd ?
current_locals_prefix dd ?
labels_list dd ?
label_hash dd ?
org_origin dd ?
org_sib dd ?
org_start dd ?
undefined_data_start dd ?
undefined_data_end dd ?
counter dd ?
counter_limit dd ?
error_line dd ?
error dd ?
display_buffer dd ?
structures_buffer dd ?
number_start dd ?
current_offset dd ?
value dq ?
fp_value rd 8
symbol_identifier dd ?
address_symbol dd ?
format_flags dd ?
symbols_stream dd ?
number_of_relocations dd ?
number_of_sections dd ?
stub_size dd ?
stub_file dd ?
sections_data dd ?
current_section dd ?
machine dw ?
subsystem dw ?
subsystem_version dd ?
image_base dd ?

macro_status db ?
parenthesis_stack db ?
output_format db ?
code_type db ?
current_pass db ?
next_pass_needed db ?
reloc_labels db ?
times_working db ?
virtual_data db ?
fp_sign db ?
fp_format db ?
value_size db ?
forced_size db ?
value_undefined db ?
value_type db ?
address_size db ?
compare_type db ?
base_code db ?
extended_code db ?
postbyte_register db ?
segment_register db ?
operand_size db ?
imm_sized db ?
jump_type db ?
mmx_size db ?
mmx_prefix db ?
nextbyte db ?

characters rb 100h
converted rb 100h
buffer rb 1000h
