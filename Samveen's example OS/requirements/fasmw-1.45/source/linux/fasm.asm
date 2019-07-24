
; flat assembler interface for Linux
; Copyright (c) 1999-2003, Tomasz Grysztar.
; All rights reserved.

	format	ELF executable
	entry	start

	macro	align value { rb (value-1) - ($ + value-1) mod value }

start:

	mov	esi,_logo
	call	display_string

	pop	eax
	cmp	eax,3
	jne	information
	pop	eax
	pop	[input_file]
	pop	[output_file]
	pop	eax
	pop	[environment]

	call	init_memory

	mov	edi,characters
	mov	ecx,100h
	xor	al,al
      make_characters_table:
	stosb
	inc	al
	loop	make_characters_table
	mov	esi,characters+'a'
	mov	edi,characters+'A'
	mov	ecx,26
	rep	movsb
	mov	edi,characters
	mov	esi,symbol_characters+1
	movzx	ecx,byte [esi-1]
	xor	eax,eax
      convert_table:
	lodsb
	mov	byte [edi+eax],0
	loop	convert_table

	mov	eax,78
	mov	ebx,buffer
	xor	ecx,ecx
	int	0x80
	mov	eax,dword [buffer]
	mov	ecx,1000
	mul	ecx
	mov	ebx,eax
	mov	eax,dword [buffer+4]
	div	ecx
	add	eax,ebx
	mov	[start_time],eax

	call	preprocessor
	call	parser
	call	assembler
	call	formatter

	movzx	eax,[current_pass]
	inc	al
	call	display_number
	mov	esi,_passes_suffix
	call	display_string
	mov	eax,78
	mov	ebx,buffer
	xor	ecx,ecx
	int	0x80
	mov	eax,dword [buffer]
	mov	ecx,1000
	mul	ecx
	mov	ebx,eax
	mov	eax,dword [buffer+4]
	div	ecx
	add	eax,ebx
	sub	eax,[start_time]
	jnc	time_ok
	add	eax,3600000
      time_ok:
	xor	edx,edx
	mov	ebx,100
	div	ebx
	or	eax,eax
	jz	display_bytes_count
	xor	edx,edx
	mov	ebx,10
	div	ebx
	push	edx
	call	display_number
	mov	dl,'.'
	call	display_character
	pop	eax
	call	display_number
	mov	esi,_seconds_suffix
	call	display_string
      display_bytes_count:
	mov	eax,[written_size]
	call	display_number
	mov	esi,_bytes_suffix
	call	display_string
	xor	al,al
	jmp	exit_program

information:
	mov	esi,_usage
	call	display_string
	mov	al,1
	jmp	exit_program

include 'system.inc'

include '..\version.inc'
include '..\errors.inc'
include '..\expressi.inc'
include '..\preproce.inc'
include '..\parser.inc'
include '..\assemble.inc'
include '..\formats.inc'
include '..\x86.inc'

_copyright db 'Copyright (c) 1999-2002, Tomasz Grysztar',0xA,0

_logo db 'flat assembler  version ',VERSION_STRING,0xA,0
_usage db 'usage: fasm source output',0xA,0

_passes_suffix db ' passes, ',0
_seconds_suffix db ' seconds, ',0
_bytes_suffix db ' bytes.',0xA,0

_counter db 8,'00000000'

align 4

memory_start dd ?
memory_end dd ?
additional_memory dd ?
additional_memory_end dd ?
free_additional_memory dd ?
input_file dd ?
output_file dd ?
environment dd ?
source_start dd ?
code_start dd ?
code_size dd ?
real_code_size dd ?
start_time dd ?
written_size dd ?

current_line dd ?
macros_list dd ?
macro_constants dd ?
macro_block dd ?
macro_block_line_number dd ?
macro_embed_level dd ?
struc_name dd ?
current_locals_prefix dd ?
anonymous_reverse dd ?
anonymous_forward dd ?
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
