; General purpose functions
; Copyright (c) 2003 onwards, Samveen S. Gulati

; get_length : string length of zero terminated string
; cmp_string ; compare two zero terminated strings



;---------------------------
;sub to find length of zero terminated string
; params
;    edx : address of string
; return val
;    dl: length of string
get_length:

	push esi
	push cx
	push eax

	mov esi,edx;
	xor cl,cl
	cld
get_length_next:
	lodsb
	cmp al,0
	jz get_length_end
	inc cl
	jmp get_length_next

get_length_end:
	mov  dl,cl
	pop  eax
	pop  cx
	pop  esi
	ret
;---------------------------

;---------------------------
;sub to compare two zero terminated strings
; params
;    edx : address of string one:ds
;    eax : address of string two:gs
; retval
;    al : 0 if equal / 1 if not equal
_string_1  db  0
cmp_string:
	pusha
cmp_string_loop1:
	mov    esi,	       eax
	lodsb
	cmp    al,	       [gs:edx]
	jne    cmp_string_loop2
	cmp    al,	       0h
	je     cmp_string_loop2
	cmp    byte [gs:edx],	  0h
	je     cmp_string_loop2
	mov    eax,esi
	inc    edx
	jmp    cmp_string_loop1

cmp_string_loop2:
	cmp    al,	       [gs:edx]
	jne    cmp_string_loop3
	mov    byte [_string_1],0h
	jmp    cmp_string_loop4
cmp_string_loop3:
	mov    byte [_string_1],1
cmp_string_loop4:
	popa
	mov    al,		[_string_1]
	ret
;---------------------------
