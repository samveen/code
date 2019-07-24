; Location and explaination of File functions and related variables
; Copyright (c) 2003 onwards, Samveen S. Gulati

; The file system being used is a fixed file length file system. The first sector
; is reserved for the boot loader. The next 89 sectors (45568 bytes) are reserved
; for the kernel. The rest 2790 sectors are devoted to the filesystem. The first 90
; sectors of this space is used for the file allocation table. Each sector will hold
; information about one file. Therefore the number of files is fixed to 90 (max size:30
; sectors). The file information will contain at least the status(whether that entry
; holds a valid file or not) and the name of the file (64 bytes long). This setup can
; be viewed as a set of 90 file inodes with each file inode pointing to the beginning
; of a set of thirty data inodes. No special filesystem information is being stored as
; of now.

; Each entry in the file table is 512 bytes long. First byte stores the index of the
; entry (also index of the file). Second byte stores value 1 if entry is valid or zero
; if it isn't. tne next two bytes (third and fourth) hold a short value holding the
; size of the file. The fifth byte holds the attributes of the file( bit 1 is system
; bit, bit 2 is executable bit, bit 3 is readonly bit). The last 64 bytes hold the name
; of the file(this may or may not be a zero terminated string). A global memory area
; is used for holding one entry of the fat. All entries will be loaded here for processing,
; processed and then the next entry will be loaded.


;----------------------------------
include "general_functions.ASM"
;----------------------------------


; Bits of the attr field
ATTR_SYSTEM   equ 01h	; bitmask for bit 0
ATTR_EXEC     equ 02h	; bitmask for bit 1
ATTR_READONLY equ 04h	; bitmask for bit 2


; The location of the buffer for the current file entry is as follows
; This is also the buffer of the filesystem driver to store the file entry
; of the current file.
file_table_entry_current:
    file_table_entry_current_index db 0h   ; 1 byte
    file_table_entry_current_valid db 0h   ; 1 byte
    file_table_entry_current_size  dw 0h   ; 2 byte
    file_table_entry_current_attr  db 0h   ; 1 byte
    file_table_entry_current_filler:
		     times 443 db 0h	   ; 443 bytes
file_table_entry_current_name:
		     times 64  db 0h	   ; 64 bytes

file_table_entry_current_end db 0


; Temporary data
dest_add dd 0h
dest_seg dw 0h
name_of_file dd 0
exec_problem db 0


;----------------------------------
; sub to load a disk sector into a buffer
; params
;   ax	LBA address of sector
;   ebx buffer offset
;   dx	buffer segment ; 16 bit
;   cl	count (range 1-30)
; ax is the LBA of the sector to load (range 0 - 2879)
; CHS address is calculated from LBA as follows
;      C = LBA / (2*18)     ; integer division
;      temp = LBA % (2*18)
;      H =  temp / 18	    ; integer division
;      S = temp % 18
temp_sec db 0h

load_sectors:
	pusha

	mov [temp_sec], cl

	; setting es:bx = seg:offset
	push es
	mov es,  dx	    ; es = seg
;	mov ebx, ebx	    ; ebx = ebx

;	mov ax,ax	    ; ax=LBA
	mov cl,   36	    ; cl= sectors_per_head * heads
	div cl		    ; al = LBA / 36 (C) , ah= LBA % 36 (temp)
	mov ch,   al	    ; Cylinder= LBA / 36
	mov al,   ah	    ; al= LBA % 36 (temp)
	cbw		    ; ax = al
	mov cl,   18	    ; cl= sectors_per_head
	div cl		    ; al = temp / 18 (H), ah = temp % 18 (S)
	mov dh, al	    ; Head=temp/18
	mov cl,ah	    ; cl=temp%18
	inc cl		    ; Sector=temp%18 + 1

load_sectors_loop_read:

	mov ah, 2		  ; Load disk data to ES:BX
	mov al,[temp_sec]	  ; Load sector count

	;bh done above		  ; bx : offset
	;bl done above		  ; bx : offset
	;ch done above		  ; ch : start cylinder
	;cl done above		  ; cl : start sector
	;dh done above		  ; dh : start head

	mov dl, 0		  ; Drive=0
	int 13h 		  ; Read!

	jc load_sectors_loop_read

	pop es
	popa
	ret
;----------------------------------




;----------------------------------
; sub to load a file entry from the FAT given its index
; params
;   dl index of entry
;
; dl is the index of the file entry (range 1 - 90)
; sequentially this is sector sector 90 + index (number of first sector : 1)
; therefore LBA = index + 90 - 1
load_entry:
	pusha

	mov al,   dl   ; al=index
	mov ah,   0    ; ax = al (ah=0)
	add ax,   89   ; ax= 90 + index -1 (LBA)

	; setting dx:ebx = segment:offset
	; of file_table_entry_current
	mov dx,   ds
	mov ebx,  file_table_entry_current
	mov cl,1

	call load_sectors

	popa
	ret
;----------------------------------


;----------------------------------
; Lists the entries of all the files on the file system
; params: NONE
; output format (one line per file)
;	 byte	: index of entry
;	 char	: s or -
;	 char	: x or -
;	 char	: r or -
;	 word	: size in hex
;	 string : name of file

fs_list_files:
	pusha

	xor ecx,ecx
	mov cl, 90
	mov bl, 1
fs_list_files_loop:
	mov dl,bl
	call load_entry
	cmp [file_table_entry_current_valid], 1
	jnz fs_list_files_loop_next

	; put index of file
	mov ah,1
	mov dl, [file_table_entry_current_index]
	int 40h

	mov ax,0400h + ' ' ; put a seperator
	int 40h

	; test for attributes and put the info on screen
fs_list_files_loop_system: ; put system attr or '-'
	test [file_table_entry_current_attr],ATTR_SYSTEM
	jnz fs_list_files_loop_system_true

	mov ax,0400h + '-'
	int 40h
	jmp fs_list_files_loop_exec

fs_list_files_loop_system_true:
	mov ax,0400h + 's'
	int 40h

fs_list_files_loop_exec: ; put exec attr or '-'
	test [file_table_entry_current_attr],ATTR_EXEC
	jnz fs_list_files_loop_exec_true

	mov ax,0400h + '-'
	int 40h
	jmp fs_list_files_loop_readonly

fs_list_files_loop_exec_true:
	mov ax,0400h + 'x'
	int 40h

fs_list_files_loop_readonly: ; put readonly attr or '-'
	test [file_table_entry_current_attr],ATTR_READONLY
	jnz fs_list_files_loop_readonly_true

	mov ax,0400h + '-'
	int 40h
	jmp fs_list_files_loop_other

fs_list_files_loop_readonly_true:
	mov ax,0400h + 'r'
	int 40h

fs_list_files_loop_other: ; put out size and name
	mov ax,0400h + ' ' ; put a seperator
	int 40h

	; put size
	mov dx, [file_table_entry_current_size]
	mov ah,2
	int 40h

	mov ax,0400h + ' ' ; put a seperator
	int 40h

	; put name
	mov edx, file_table_entry_current_name
	mov ah,5
	int 40h

	mov ax,040dh ; put a newline
	int 40h

fs_list_files_loop_next:
	inc bl
	loop fs_list_files_loop

	mov dl,1
	call load_entry

	popa
	ret
;----------------------------------



;----------------------------------
; subroutine to load file for executing from the file system
; into specified location
; params are
;   edx address of name : gs
;   ebx buffer offset
;   cx	buffer segment ; 16 bit
; retval
;    cx size if success/ 0 if failure
;    al 0 if file not found / 1 if file not execable
fs_exec_file:
	pusha

	mov [exec_problem], 0

	mov [name_of_file],edx
	mov [dest_seg],cx
	mov [dest_add],ebx

	xor ecx,ecx
	mov cl, 90
	mov bl, 1
fs_exec_file_loop:
	mov dl,bl
	call load_entry
	cmp [file_table_entry_current_valid], 1
	jnz fs_exec_file_loop_next

	; compare file names
	mov eax, file_table_entry_current_name
	mov edx, [name_of_file]

	call cmp_string
	cmp al,0
	jz  fs_exec_file_found

fs_exec_file_loop_next:
	inc bl
	loop fs_exec_file_loop

	mov cx,0
	jmp fs_exec_file_end

fs_exec_file_found:  ; file has been found

	; check if file is executable
	test [file_table_entry_current_attr],ATTR_EXEC
	jnz fs_exec_file_attr_exec_true

	mov [exec_problem],1
	mov cx,0
	jmp fs_exec_file_end

fs_exec_file_attr_exec_true:
	;   cl	count of sectors
	mov ax, [file_table_entry_current_size]
	mov dx, 0
	mov cx,512
	div cx
	cmp dx, 0
	jz fs_exec_file_found_no_remainder

	inc ax
fs_exec_file_found_no_remainder:
	; cl no of sectors to load
	mov cl, al

	; ax LBA of starting sector
	; starting LBA of file with index X is
	;      LBA= 180 + (index - 1) * 30
	mov al,[file_table_entry_current_index]
	dec al
	mov bl,30
	mul bl
	add ax,180

	;   ebx buffer offset
	mov ebx, [disk_buffer]

	;   dx	buffer segment ; 16 bit
	mov dx, [disk_buffer_seg]

	call load_sectors

	popa
	pusha

	push ds
	push es

	; destination
	mov edi, ebx
	mov es, cx
	;size
	mov cx, [file_table_entry_current_size]

	; source
	mov esi, [disk_buffer]
	push [disk_buffer_seg]
	pop ds

	cld
	rep movsb

	pop es
	pop ds

	mov cx, [file_table_entry_current_size]
fs_exec_file_end:
	mov word [name_of_file], cx
	popa
	mov al, [exec_problem]
	mov cx, word [name_of_file]
	ret
;----------------------------------


;----------------------------------
; subroutine to load file for displaying from the file system
; into specified location
; params are
;   edx address of name : gs
;   ebx buffer offset
;   cx	buffer segment ; 16 bit
fs_load_file:

	pusha

	mov [name_of_file],edx
	xor ecx,ecx
	mov cl, 90
	mov bl, 1


fs_load_file_loop:
	mov dl,bl
	call load_entry
	cmp [file_table_entry_current_valid], 1
	jnz fs_load_file_loop_next

	; compare file names
	mov eax, file_table_entry_current_name
	mov edx, [name_of_file]

	call cmp_string
	cmp al,0
	jz  fs_load_file_found

fs_load_file_loop_next:
	inc bl
	loop fs_load_file_loop

	mov cx,0
	jmp fs_load_file_end

fs_load_file_found:  ; file has been found
	;   cl	count of sectors
	mov ax, [file_table_entry_current_size]
	mov dx, 0
	mov cx,512
	div cx
	cmp dx, 0
	jz fs_load_file_found_no_remainder

	inc ax
fs_load_file_found_no_remainder:
	; cl no of sectors to load
	mov cl, al

	; ax LBA of starting sector
	; starting LBA of file with index X is
	;      LBA= 180 + (index - 1) * 30
	mov al,[file_table_entry_current_index]
	dec al
	mov bl,30
	mul bl
	add ax,180

	;   ebx buffer offset
	mov ebx, [disk_buffer]

	;   dx	buffer segment ; 16 bit
	mov dx, [disk_buffer_seg]

	call load_sectors

	popa
	pusha

	push ds
	push es

	; destination
	mov edi, ebx
	mov es, cx
	;size
	mov cx, [file_table_entry_current_size]

	; source
	mov esi, [disk_buffer]
	push [disk_buffer_seg]
	pop ds

	cld
	rep movsb

	pop es
	pop ds

	mov cx, [file_table_entry_current_size]
fs_load_file_end:
	mov word [name_of_file], cx
	popa
	mov cx,  word [name_of_file]
	ret
;----------------------------------
