
; extended Win32 programming headers with parameters count checking (ASCII)

include '%include%/win32ax.inc'

macro invoke proc,[arg]
 { common local ..count
    ..count = 0
    if ~ arg eq
   forward ..count = ..count + 1
   common
    end if
    if defined proc#%
     if ..count <> proc#%
      display "error: invalid count of parameters for API call.",0Dh,0Ah
      local halt
      halt
     end if
    end if
    invoke proc,arg
 }

include '%include%/pcount/kernel32.inc'
include '%include%/pcount/user32.inc'
include '%include%/pcount/gdi32.inc'
include '%include%/pcount/advapi32.inc'
include '%include%/pcount/comctl32.inc'
include '%include%/pcount/comdlg32.inc'
include '%include%/pcount/shell32.inc'
