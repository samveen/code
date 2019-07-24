
; DirectDraw programming example

format PE GUI 4.0
entry start

include '%include%\win32a.inc'

include 'ddraw.inc'

section '.data' data readable writeable

  _title db 'flat assembler DirectDraw application',0
  _class db 'FDDRAW32',0

  _error db 'Error',0
  _ddraw_error db 'Direct Draw initialization failed.',0
  _open_error db 'Failed opening data file.',0

  picture db 'DDRAW.GIF',0

section '.bss' readable writeable

  hinstance dd ?
  hwnd dd ?
  wc WNDCLASS
  msg MSG

  ddsd DDSURFACEDESC
  ddscaps DDSCAPS

  lpDD DirectDraw
  lpDDSPrimary DirectDrawSurface
  lpDDSBack DirectDrawSurface

  lpDDSPicture DirectDrawSurface
  lpDDPalette DirectDrawPalette

  bytes_count dd ?
  last_tick dd ?
  frame db ?
  active db ?
  LZW_bits db ?
  LZW_table rd (0F00h-2)*2
  buffer rb 40000h
  rect RECT

section '.code' code readable executable

  start:

	invoke	GetModuleHandle,NULL
	mov	[hinstance],eax

	invoke	LoadIcon,NULL,IDI_APPLICATION
	mov	[wc.hIcon],eax

	invoke	LoadCursor,NULL,IDC_ARROW
	mov	[wc.hCursor],eax

	mov	[wc.style],0
	mov	[wc.lpfnWndProc],WindowProc
	mov	[wc.cbClsExtra],0
	mov	[wc.cbWndExtra],0
	mov	eax,[hinstance]
	mov	[wc.hInstance],eax
	mov	[wc.hbrBackground],COLOR_BTNSHADOW
	mov	dword [wc.lpszMenuName],NULL
	mov	dword [wc.lpszClassName],_class
	invoke	RegisterClass,wc

	invoke	CreateWindowEx,\
		0,_class,_title,WS_POPUP+WS_VISIBLE,0,0,0,0,NULL,NULL,[hinstance],NULL
	mov	[hwnd],eax

	invoke	DirectDrawCreate,NULL,lpDD,NULL
	or	eax,eax
	jnz	ddraw_error

	comcall lpDD,SetCooperativeLevel,\
		[hwnd],DDSCL_EXCLUSIVE+DDSCL_FULLSCREEN
	or	eax,eax
	jnz	ddraw_error

	comcall lpDD,SetDisplayMode,\
		640,480,8
	or	eax,eax
	jnz	ddraw_error

	mov	[ddsd.dwSize],sizeof.DDSURFACEDESC
	mov	[ddsd.dwFlags],DDSD_CAPS+DDSD_BACKBUFFERCOUNT
	mov	[ddsd.ddsCaps.dwCaps],DDSCAPS_PRIMARYSURFACE+DDSCAPS_FLIP+DDSCAPS_COMPLEX
	mov	[ddsd.dwBackBufferCount],1
	comcall lpDD,CreateSurface,\
		ddsd,lpDDSPrimary,NULL
	or	eax,eax
	jnz	ddraw_error

	mov	[ddscaps.dwCaps],DDSCAPS_BACKBUFFER
	comcall lpDDSPrimary,GetAttachedSurface,\
		ddscaps,lpDDSBack
	or	eax,eax
	jnz	ddraw_error

	mov	esi,picture
	call	load_picture
	jc	open_error

	mov	esi,picture
	call	load_palette
	jc	open_error

	comcall lpDDSPrimary,SetPalette,eax

	invoke	GetTickCount
	mov	[last_tick],eax

	jmp	paint

main_loop:

	invoke	PeekMessage,msg,NULL,0,0,PM_NOREMOVE
	or	eax,eax
	jz	no_message
	invoke	GetMessage,msg,NULL,0,0
	or	eax,eax
	jz	end_loop
	invoke	TranslateMessage,msg
	invoke	DispatchMessage,msg

	jmp	main_loop

    no_message:

	cmp	[active],0
	je	sleep

	comcall lpDDSPrimary,IsLost
	or	eax,eax
	jz	paint
	cmp	eax,DDERR_SURFACELOST
	jne	end_loop

	comcall lpDDSPrimary,Restore

paint:

	mov	[rect.top],0
	mov	[rect.bottom],480
	mov	[rect.left],0
	mov	[rect.right],640

	comcall lpDDSBack,BltFast,\
		0,0,[lpDDSPicture],rect,DDBLTFAST_SRCCOLORKEY

	movzx	eax,[frame]
	xor	edx,edx
	mov	ebx,10
	div	ebx

	sal	eax,6
	add	eax,480
	mov	[rect.top],eax
	add	eax,64
	mov	[rect.bottom],eax
	sal	edx,6
	mov	[rect.left],edx
	add	edx,64
	mov	[rect.right],edx

	comcall lpDDSBack,BltFast,\
		288,200,[lpDDSPicture],rect,DDBLTFAST_SRCCOLORKEY

	comcall lpDDSPrimary,Flip,0,0

	invoke	GetTickCount
	mov	ebx,eax
	sub	ebx,[last_tick]
	cmp	ebx,20
	jb	main_loop
	add	[last_tick],20

	inc	[frame]
	cmp	[frame],60
	jb	main_loop
	mov	[frame],0
	jmp	main_loop

sleep:
	invoke	WaitMessage
	jmp	main_loop

ddraw_error:
	mov	eax,_ddraw_error
	jmp	error
open_error:
	mov	eax,_open_error
    error:
	invoke	MessageBox,[hwnd],eax,_error,MB_OK
	invoke	DestroyWindow,[hwnd]
	invoke	PostQuitMessage,1
	jmp	main_loop

end_loop:
	invoke	ExitProcess,[msg.wParam]

include 'gif87a.inc'

proc WindowProc, hwnd,wmsg,wparam,lparam
	enter
	push	ebx esi edi
	mov	eax,[wmsg]
	cmp	eax,WM_CREATE
	je	wmcreate
	cmp	eax,WM_DESTROY
	je	wmdestroy
	cmp	eax,WM_ACTIVATE
	je	wmactivate
	cmp	eax,WM_SETCURSOR
	je	wmsetcursor
	cmp	eax,WM_KEYDOWN
	je	wmkeydown
    defwindowproc:
	invoke	DefWindowProc,[hwnd],[wmsg],[wparam],[lparam]
	jmp	finish
    wmcreate:
	xor	eax,eax
	jmp	finish
    wmkeydown:
	cmp	[wparam],VK_ESCAPE
	jne	finish
    wmdestroy:
	comcall lpDD,RestoreDisplayMode
	comcall lpDD,Release
	invoke	PostQuitMessage,0
	xor	eax,eax
	jmp	finish
    wmactivate:
	mov	eax,[wparam]
	mov	[active],al
	jmp	finish
    wmsetcursor:
	invoke	SetCursor,0
	xor	eax,eax
    finish:
	pop	edi esi ebx
	return

section '.idata' import data readable

  library kernel,'KERNEL32.DLL',\
	  user,'USER32.DLL',\
	  ddraw,'DDRAW.DLL'

  import kernel,\
	 GetModuleHandle,'GetModuleHandleA',\
	 CreateFile,'CreateFileA',\
	 ReadFile,'ReadFile',\
	 CloseHandle,'CloseHandle',\
	 GetTickCount,'GetTickCount',\
	 ExitProcess,'ExitProcess'

  import user,\
	 RegisterClass,'RegisterClassA',\
	 CreateWindowEx,'CreateWindowExA',\
	 DestroyWindow,'DestroyWindow',\
	 DefWindowProc,'DefWindowProcA',\
	 GetMessage,'GetMessageA',\
	 PeekMessage,'PeekMessageA',\
	 TranslateMessage,'TranslateMessage',\
	 DispatchMessage,'DispatchMessageA',\
	 LoadCursor,'LoadCursorA',\
	 LoadIcon,'LoadIconA',\
	 SetCursor,'SetCursor',\
	 MessageBox,'MessageBoxA',\
	 PostQuitMessage,'PostQuitMessage',\
	 WaitMessage,'WaitMessage'

  import ddraw,\
	 DirectDrawCreate,'DirectDrawCreate'
