
; macroinstructions for defining and invoking cdecl HLL procedures

macro cproc name,[arg]			; define procedure
 { common
    name:
    virtual at ebp+8
    if ~ arg eq
   forward
     local ..arg
     ..arg dd ?
     arg equ ..arg
   common
     end if
    end virtual
    local ..dynamic_data,..dynamic_size
    dynamic_data equ ..dynamic_data
    dynamic_size equ ..dynamic_size
    virtual at ebp - dynamic_size
     dynamic_data: }

macro enterc				; begin procedure instructions
 { dynamic_size = $ - dynamic_data
   end virtual
   enter dynamic_size,0 }

macro retc				; return from procedure
 { leave
   ret }

macro ccall proc,[arg]			; call procedure
 { common local size
   size = 0
   reverse
   pushd arg
   size = size+4
   common
   call proc
   add esp,size }

macro cinvoke proc,[arg]		; invoke procedure (indirect)
 { common
    if ~ arg eq
     ccall [proc],arg
    else
     call [proc]
    end if }
