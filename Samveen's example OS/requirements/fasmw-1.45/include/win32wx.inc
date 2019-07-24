
; extended Win32 programming headers (WIDE)

format PE GUI 4.0

include '%include%/win32w.inc'

include '%include%/macro/if.inc'

addr equ
ADDR equ

macro pushd arg
 { if _#arg in <_addr arg,_ADDR arg>
    local ..arg,..op
    label ..arg at arg
    virtual at 0
     mov eax,..arg
     load ..opcode from 0
    end virtual
    if ..opcode = 0A1h
     push arg
    else
     lea edx,..arg
     push edx
    end if
   else if arg eqtype ""
    local ..continue
    call ..continue
    du arg,0
    ..continue:
   else
    push arg
   end if }

macro .data { section '.data' data readable writeable }

macro .code { section '.text' code readable executable }

macro .end label
 {
   entry label

   section '.idata' import data readable writeable

     library kernel32,'KERNEL32.DLL',\
	     user32,'USER32.DLL',\
	     gdi32,'GDI32.DLL',\
	     advapi32,'ADVAPI32.DLL',\
	     comctl32,'COMCTL32.DLL',\
	     comdlg32,'COMDLG32.DLL',\
	     shell32,'SHELL32.DLL'

     include '%include%/apiw/kernel32.inc'
     include '%include%/apiw/user32.inc'
     include '%include%/apiw/gdi32.inc'
     include '%include%/apiw/advapi32.inc'
     include '%include%/apiw/comctl32.inc'
     include '%include%/apiw/comdlg32.inc'
     include '%include%/apiw/shell32.inc'
 }
