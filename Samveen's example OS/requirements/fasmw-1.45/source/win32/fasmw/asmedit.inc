
; Assembly Editor for Win32
; Copyright (c) 2001-2002, Tomasz Grysztar.
; All rights reserved.

; AsmEdit styles

AES_AUTOINDENT	 = 0001h
AES_SMARTTABS	 = 0002h
AES_OPTIMALFILL  = 0004h
AES_SECURESEL	 = 0008h
AES_AUTOBRACKETS = 0010h
AES_CONSOLECARET = 0020h

; AsmEdit messages

AEM_SETMODE	       = WM_USER + 0
AEM_GETMODE	       = WM_USER + 1
AEM_SETSYNTAXHIGHLIGHT = WM_USER + 2
AEM_GETLINEDATA        = WM_USER + 3
AEM_SETPOS	       = WM_USER + 4
AEM_GETPOS	       = WM_USER + 5
AEM_FINDFIRST	       = WM_USER + 6
AEM_FINDNEXT	       = WM_USER + 7
AEM_CANFINDNEXT        = WM_USER + 8
AEM_GETWORDATCARET     = WM_USER + 9
AEM_SETTEXTCOLOR       = WM_USER + 10
AEM_SETSELCOLOR        = WM_USER + 11

; AsmEdit mode flags

AEMODE_OVERWRITE   = 1
AEMODE_VERTICALSEL = 2

; AsmEdit search flags

AEFIND_CASESENSITIVE = 1
AEFIND_WHOLEWORDS    = 2
AEFIND_BACKWARD      = 4

; AsmEdit notifications

AEN_SETFOCUS	= 01h
AEN_KILLFOCUS	= 02h
AEN_TEXTCHANGE	= 03h
AEN_POSCHANGE	= 04h
AEN_MODECHANGE	= 05h
AEN_OUTOFMEMORY = 0Fh

proc AsmEdit, hwnd,wmsg,wparam,lparam

  .editor_memory dd ?
  .editor_style dd ?

  label .editor_data

  .first_line dd ?
  .lines_count dd ?
  .caret_line dd ?
  .caret_position dd ?
  .caret_line_number dd ?
  .window_line dd ?
  .window_position dd ?
  .window_line_number dd ?
  .sel_line dd ?
  .sel_position dd ?
  .sel_line_number dd ?
  .editor_mode dd ?

  .editor_status_size = $ - .editor_data

  .editor_font dd ?
  .font_width dd ?
  .font_height dd ?
  .page_size dd ?
  .editor_screen dd ?
  .screen_width dd ?
  .screen_height dd ?
  .background_color dd ?
  .text_color dd ?
  .selection_background dd ?
  .selection_text dd ?
  .syntax_proc dd ?
  .syntax_colors dd ?
  .unallocated_lines dd ?
  .unallocated_lines_end dd ?
  .memory_search_block dd ?
  .memory_search_line dd ?
  .undo_data dd ?
  .search_text dd ?
  .caret_x dd ?
  .caret_y dd ?
  .current_operation db ?
  .last_operation db ?
  .mouse_select db ?
  .focus db ?

  .editor_data_size = $ - .editor_data

  .current_line dd ?
  .return_value dd ?
  .background_brush dd ?
  .selection_brush dd ?
  .was_selection db ?
  .line_selection db ?
  .redraw_now db ?
  .notification db ?

	enter
	push	ebx esi edi
	invoke	GetWindowLong,[hwnd],GWL_STYLE
	mov	[.editor_style],eax
	cmp	[wmsg],WM_CREATE
	je	.wmcreate
	cmp	[wmsg],WM_GETDLGCODE
	je	.wmgetdlgcode
	invoke	GetWindowLong,[hwnd],GWL_USERDATA
	or	eax,eax
	jz	.defwndproc
	mov	[.editor_memory],eax
	lea	esi,[eax+8]
	lea	edi,[.editor_data]
	mov	ecx,.editor_data_size shr 2
	rep	movsd
	mov	[.return_value],0
	mov	[.notification],0
	mov	[.redraw_now],0
	cmp	[wmsg],WM_DESTROY
	je	.wmdestroy
	cmp	[wmsg],WM_PAINT
	je	.wmpaint
	invoke	HideCaret,[hwnd]
	cmp	[wmsg],WM_HSCROLL
	je	.wmhscroll
	cmp	[wmsg],WM_VSCROLL
	je	.wmvscroll
	cmp	[wmsg],WM_SIZE
	je	.wmsize
	cmp	[.sel_line],0
	setnz	al
	mov	[.was_selection],al
	xor	al,al
	xchg	[.current_operation],al
	mov	[.last_operation],al
	mov	eax,[wmsg]
	cmp	eax,WM_SETFOCUS
	je	.wmsetfocus
	cmp	eax,WM_KILLFOCUS
	je	.wmkillfocus
	cmp	eax,WM_KEYDOWN
	je	.wmkeydown
	cmp	eax,WM_SYSKEYDOWN
	je	.wmsyskeydown
	cmp	eax,WM_CHAR
	je	.wmchar
	cmp	eax,WM_LBUTTONDOWN
	je	.wmlbuttondown
	cmp	eax,WM_LBUTTONUP
	je	.wmlbuttonup
	cmp	eax,WM_MOUSEMOVE
	je	.wmmousemove
	cmp	eax,WM_LBUTTONDBLCLK
	je	.wmlbuttondblclk
	cmp	eax,WM_MOUSEWHEEL
	je	.wmmousewheel
	cmp	eax,WM_COPY
	je	.wmcopy
	cmp	eax,WM_CUT
	je	.wmcut
	cmp	eax,WM_PASTE
	je	.wmpaste
	cmp	eax,WM_CLEAR
	je	.wmclear
	cmp	eax,WM_SETTEXT
	je	.wmsettext
	cmp	eax,WM_GETTEXTLENGTH
	je	.wmgettextlength
	cmp	eax,WM_GETTEXT
	je	.wmgettext
	cmp	eax,WM_SETFONT
	je	.wmsetfont
	cmp	eax,WM_GETFONT
	je	.wmgetfont
	cmp	eax,WM_UNDO
	je	.wmundo
	cmp	eax,EM_UNDO
	je	.wmundo
	cmp	eax,EM_CANUNDO
	je	.emcanundo
	cmp	eax,EM_EMPTYUNDOBUFFER
	je	.ememptyundobuffer
	cmp	eax,EM_REPLACESEL
	je	.emreplacesel
	cmp	eax,AEM_SETMODE
	je	.aemsetmode
	cmp	eax,AEM_GETMODE
	je	.aemgetmode
	cmp	eax,AEM_SETSYNTAXHIGHLIGHT
	je	.aemsetsyntaxhighlight
	cmp	eax,AEM_GETLINEDATA
	je	.aemgetlinedata
	cmp	eax,AEM_SETPOS
	je	.aemsetpos
	cmp	eax,AEM_GETPOS
	je	.aemgetpos
	cmp	eax,AEM_FINDFIRST
	je	.aemfindfirst
	cmp	eax,AEM_FINDNEXT
	je	.aemfindnext
	cmp	eax,AEM_CANFINDNEXT
	je	.aemcanfindnext
	cmp	eax,AEM_GETWORDATCARET
	je	.aemgetwordatcaret
	cmp	eax,AEM_SETTEXTCOLOR
	je	.aemsettextcolor
	cmp	eax,AEM_SETSELCOLOR
	je	.aemsetselcolor
	invoke	ShowCaret,[hwnd]
.defwndproc:
	invoke	DefWindowProc,[hwnd],[wmsg],[wparam],[lparam]
	jmp	.finish
.wmcreate:
	call	.init_editor_memory
	jc	.create_failed
	call	.init_editor_data
	invoke	SetWindowLong,[hwnd],GWL_USERDATA,[.editor_memory]
	invoke	GetSysColor,COLOR_WINDOW
	mov	[.background_color],eax
	invoke	GetSysColor,COLOR_WINDOWTEXT
	mov	[.text_color],eax
	invoke	GetSysColor,COLOR_HIGHLIGHT
	mov	[.selection_background],eax
	invoke	GetSysColor,COLOR_HIGHLIGHTTEXT
	mov	[.selection_text],eax
	mov	eax,[asmedit_font]
	mov	[.editor_font],eax
	invoke	GetDC,[hwnd]
	mov	ebx,eax
	invoke	SelectObject,ebx,[.editor_font]
	invoke	GetTextMetrics,ebx,tm
	mov	eax,[tm.tmHeight]
	mov	[.font_height],eax
	mov	eax,[tm.tmAveCharWidth]
	mov	[.font_width],eax
	invoke	ReleaseDC,[hwnd],ebx
	call	.update_positions
	call	.update_screen
	mov	[.return_value],0
	jmp	.done
    .create_failed:
	or	eax,-1
	jmp	.finish
.wmdestroy:
	invoke	GlobalFree,[.editor_screen]
	call	.release_editor_memory
	invoke	SetWindowLong,[hwnd],GWL_USERDATA,0
	xor	eax,eax
	jmp	.finish
.wmgetdlgcode:
	mov	eax,DLGC_WANTCHARS+DLGC_WANTARROWS
	jmp	.finish
.wmpaint:
	invoke	GetUpdateRect,[hwnd],rect,FALSE
	or	eax,eax
	jz	.finish
	cmp	[.editor_screen],0
	je	.finish
	invoke	BeginPaint,[hwnd],ps
	mov	ebx,eax
	invoke	CreateSolidBrush,[.background_color]
	mov	[.background_brush],eax
	invoke	CreateSolidBrush,[.selection_background]
	mov	[.selection_brush],eax
	invoke	SelectObject,ebx,[.editor_font]
	mov	esi,[.editor_screen]
	mov	eax,[.screen_width]
	mul	[.screen_height]
	lea	edi,[esi+eax]
	mov	[rect.top],0
	mov	eax,[.font_height]
	mov	[rect.bottom],eax
	mov	ecx,[.screen_height]
    .paint_screen:
	push	ecx
	mov	[rect.left],0
	mov	ecx,[.screen_width]
    .paint_line:
	cmp	byte [esi],0
	je	.paint_empty_block
	mov	edx,1
	mov	al,[edi]
    .get_characters_block:
	cmp	edx,ecx
	je	.get_color
	cmp	al,[edi+edx]
	jne	.get_color
	cmp	byte [esi+edx],0
	je	.get_color
	inc	edx
	jmp	.get_characters_block
    .paint_empty_block:
	mov	edx,1
	test	byte [edi],80h
	jnz	.get_empty_selection
    .get_empty_block:
	cmp	edx,ecx
	je	.fill_empty_block
	cmp	byte [esi+edx],0
	jne	.fill_empty_block
	test	byte [edi+edx],80h
	jnz	.fill_empty_block
	inc	edx
	jmp	.get_empty_block
    .fill_empty_block:
	push	ecx edx
	mov	eax,[.font_width]
	mul	edx
	add	eax,[rect.left]
	mov	[rect.right],eax
	invoke	FillRect,ebx,rect,[.background_brush]
	jmp	.paint_next_block
    .get_empty_selection:
	cmp	edx,ecx
	je	.fill_empty_selection
	cmp	byte [esi+edx],0
	jne	.fill_empty_selection
	test	byte [edi+edx],80h
	jz	.fill_empty_selection
	inc	edx
	jmp	.get_empty_selection
    .fill_empty_selection:
	push	ecx edx
	mov	eax,[.font_width]
	mul	edx
	add	eax,[rect.left]
	mov	[rect.right],eax
	invoke	FillRect,ebx,rect,[.selection_brush]
	jmp	.paint_next_block
    .get_color:
	push	ecx edx
	test	byte [edi],80h
	jnz	.highlight_color
	invoke	SetBkColor,ebx,[.background_color]
	mov	al,[edi]
	or	al,al
	jnz	.syntax_color
    .default_color:
	invoke	SetTextColor,ebx,[.text_color]
	jmp	.color_ok
    .syntax_color:
	movzx	eax,al
	mov	edx,[.syntax_colors]
	or	edx,edx
	jz	.default_color
	mov	eax,[edx+(eax-1)*4]
	invoke	SetTextColor,ebx,eax
	jmp	.color_ok
    .highlight_color:
	invoke	SetBkColor,ebx,[.selection_background]
	invoke	SetTextColor,ebx,[.selection_text]
    .color_ok:
	mov	ecx,[esp]
	mov	eax,[.font_width]
	mul	ecx
	add	eax,[rect.left]
	mov	[rect.right],eax
	invoke	DrawText,ebx,esi,ecx,rect,DT_LEFT+DT_NOPREFIX+DT_SINGLELINE
    .paint_next_block:
	pop	edx ecx
	sub	ecx,edx
	add	esi,edx
	add	edi,edx
	mov	eax,[rect.right]
	mov	[rect.left],eax
	or	ecx,ecx
	jnz	.paint_line
	mov	eax,[.font_height]
	add	[rect.top],eax
	add	[rect.bottom],eax
	pop	ecx
	dec	ecx
	jnz	.paint_screen
	invoke	DeleteObject,[.background_brush]
	invoke	DeleteObject,[.selection_brush]
	invoke	EndPaint,[hwnd],ps
	xor	eax,eax
	jmp	.finish
.wmsetfocus:
	or	[.focus],-1
	call	.create_caret
	mov	[.notification],AEN_SETFOCUS
	cmp	[.was_selection],0
	je	.done
	jmp	.moved_window
.wmkillfocus:
	mov	[.focus],0
	invoke	DestroyCaret
	mov	[.notification],AEN_KILLFOCUS
	cmp	[.was_selection],0
	je	.done
	jmp	.moved_window
.wmhscroll:
	mov	[sc.cbSize],sizeof.SCROLLINFO
	mov	[sc.fMask],SIF_PAGE
	invoke	GetScrollInfo,[hwnd],SB_HORZ,sc
	movzx	eax,word [wparam]
	cmp	eax,SB_LINEUP
	je	.hscroll_left
	cmp	eax,SB_LINEDOWN
	je	.hscroll_right
	cmp	eax,SB_THUMBTRACK
	je	.hscroll_pos
	cmp	eax,SB_PAGEUP
	je	.hscroll_wleft
	cmp	eax,SB_PAGEDOWN
	je	.hscroll_wright
    .hscroll_ignore:
	jmp	.done
    .hscroll_left:
	cmp	[.window_position],0
	je	.hscroll_ignore
	dec	[.window_position]
	jmp	.moved_window
    .hscroll_right:
	mov	eax,100h
	sub	eax,[sc.nPage]
	cmp	[.window_position],eax
	jge	.hscroll_ignore
	inc	[.window_position]
	jmp	.moved_window
    .hscroll_pos:
	movzx	eax,word [wparam+2]
	mov	[.window_position],eax
	jmp	.moved_window
    .hscroll_wleft:
	mov	eax,[sc.nPage]
	sub	[.window_position],eax
	jnc	.moved_window
	mov	[.window_position],0
	jmp	.moved_window
    .hscroll_wright:
	mov	eax,[sc.nPage]
	mov	ecx,100h
	sub	ecx,eax
	add	[.window_position],eax
	cmp	[.window_position],ecx
	jbe	.moved_window
	mov	[.window_position],ecx
	jmp	.moved_window
.wmvscroll:
	mov	[sc.cbSize],sizeof.SCROLLINFO
	mov	[sc.fMask],SIF_ALL
	invoke	GetScrollInfo,[hwnd],SB_VERT,sc
	movzx	eax,word [wparam]
	cmp	eax,SB_LINEUP
	je	.vscroll_up
	cmp	eax,SB_LINEDOWN
	je	.vscroll_down
	cmp	eax,SB_THUMBTRACK
	je	.vscroll_pos
	cmp	eax,SB_PAGEUP
	je	.vscroll_pageup
	cmp	eax,SB_PAGEDOWN
	je	.vscroll_pagedown
    .vscroll_ignore:
	jmp	.done
    .vscroll_up:
	mov	esi,[.window_line]
	mov	esi,[esi+4]
	or	esi,esi
	jz	.vscroll_ignore
	dec	[.window_line_number]
	mov	[.window_line],esi
	jmp	.moved_window
    .vscroll_down:
	mov	eax,[sc.nPos]
	add	eax,[sc.nPage]
	cmp	eax,[sc.nMax]
	ja	.vscroll_ignore
	mov	esi,[.window_line]
	mov	esi,[esi]
	or	esi,esi
	jz	.vscroll_ignore
	inc	[.window_line_number]
	mov	[.window_line],esi
	jmp	.moved_window
    .vscroll_pos:
	mov	eax,[sc.nTrackPos]
	call	.find_line
	or	esi,esi
	jz	.vscroll_ignore
	mov	[.window_line],esi
	mov	[.window_line_number],ecx
	jmp	.moved_window
    .vscroll_pageup:
	mov	esi,[.window_line]
	mov	ecx,[sc.nPage]
    .scroll_up:
	mov	eax,[esi+4]
	or	eax,eax
	jz	.scroll_ok
	dec	[.window_line_number]
	mov	esi,eax
	loop	.scroll_up
	jmp	.scroll_ok
    .vscroll_pagedown:
	mov	esi,[.window_line]
	mov	eax,[sc.nPos]
	add	eax,[sc.nPage]
	mov	ecx,[sc.nMax]
	sub	ecx,eax
	inc	ecx
	cmp	ecx,[sc.nPage]
	jbe	.scroll_down
	mov	ecx,[sc.nPage]
    .scroll_down:
	mov	eax,[esi]
	or	eax,eax
	jz	.scroll_ok
	inc	[.window_line_number]
	mov	esi,eax
	loop	.scroll_down
    .scroll_ok:
	mov	[.window_line],esi
	jmp	.moved_window
.wmmousewheel:
	mov	esi,[.window_line]
	mov	eax,[wparam]
	sar	eax,16
	cdq
	mov	ecx,120
	idiv	ecx
	imul	eax,[whell_scroll_lines]
	mov	ecx,eax
	cmp	ecx,0
	jg	.scroll_up
	neg	ecx
	jnz	.scroll_down
	jmp	.done
.wmsize:
	invoke	GlobalFree,[.editor_screen]
	call	.update_positions
	call	.update_screen
	invoke	GetClientRect,[hwnd],rect
	invoke	InvalidateRect,[hwnd],rect,FALSE
	jmp	.done
.wmkeydown:
	invoke	GetKeyboardState,kbstate
	cmp	[.was_selection],0
	jne	.process_key
	mov	eax,[.caret_line]
	mov	[.sel_line],eax
	mov	eax,[.caret_position]
	mov	[.sel_position],eax
	mov	eax,[.caret_line_number]
	mov	[.sel_line_number],eax
    .process_key:
	mov	eax,[wparam]
	cmp	eax,VK_LEFT
	je	.left
	cmp	eax,VK_RIGHT
	je	.right
	cmp	eax,VK_UP
	je	.up
	cmp	eax,VK_DOWN
	je	.down
	cmp	eax,VK_BACK
	je	.back
	cmp	eax,VK_HOME
	je	.line_home
	cmp	eax,VK_END
	je	.line_end
	cmp	eax,VK_PGUP
	je	.pgup
	cmp	eax,VK_PGDN
	je	.pgdn
	cmp	eax,VK_DELETE
	je	.del
	cmp	eax,VK_INSERT
	je	.insert
	cmp	eax,VK_F6
	je	.duplicate_line
	test	[kbstate+VK_CONTROL],80h
	jz	.convert_to_ascii
	cmp	eax,'Y'
	je	.remove_line
    .convert_to_ascii:
	mov	ax,word [lparam+2]
	and	eax,7Fh
	invoke	ToAscii,[wparam],eax,kbstate,char,FALSE
	or	eax,eax
	jz	.ignore
	mov	al,[char]
	cmp	al,20h
	jae	.put_char
	cmp	al,0Dh
	je	.put_char
	cmp	al,9
	je	.put_char
	jmp	.ignore
  .left:
	test	[kbstate+VK_CONTROL],80h
	jnz	.word_left
	cmp	[.caret_position],0
	je	.ignore
	dec	[.caret_position]
	jmp	.moved_caret
  .right:
	test	[kbstate+VK_CONTROL],80h
	jnz	.word_right
	cmp	[.caret_position],100h
	je	.ignore
	inc	[.caret_position]
	jmp	.moved_caret
  .up:
	mov	esi,[.caret_line]
	mov	eax,[esi+4]
	or	eax,eax
	jz	.ignore
	mov	[.caret_line],eax
	dec	[.caret_line_number]
	jmp	.moved_caret
  .down:
	mov	esi,[.caret_line]
	mov	eax,[esi]
	or	eax,eax
	jz	.ignore
	mov	[.caret_line],eax
	inc	[.caret_line_number]
	jmp	.moved_caret
  .line_home:
	test	[kbstate+VK_CONTROL],80h
	jnz	.screen_home
	mov	[.caret_position],0
	jmp	.moved_caret
  .line_end:
	test	[kbstate+VK_CONTROL],80h
	jnz	.screen_end
    .find_line_end:
	mov	edi,[.caret_line]
	add	edi,108h-1
	mov	al,20h
	mov	ecx,100h
	std
	repe	scasb
	setne	al
	movzx	eax,al
	add	ecx,eax
	cld
	mov	[.caret_position],ecx
	jmp	.moved_caret
  .screen_home:
	mov	eax,[.window_line]
	mov	[.caret_line],eax
	mov	eax,[.window_line_number]
	mov	[.caret_line_number],eax
	jmp	.moved_caret
  .screen_end:
	mov	eax,[.window_line_number]
	add	eax,[.page_size]
	dec	eax
	call	.find_line
	mov	[.caret_line],esi
	mov	[.caret_line_number],ecx
	jmp	.moved_caret
  .pgup:
	test	[kbstate+VK_CONTROL],80h
	jnz	.text_home
	mov	eax,[.caret_line_number]
	mov	eax,[.caret_line_number]
	sub	eax,[.page_size]
	ja	.pgup_caret_ok
	mov	eax,1
    .pgup_caret_ok:
	call	.find_line
	mov	[.caret_line],esi
	mov	[.caret_line_number],ecx
	mov	eax,[.window_line_number]
	sub	eax,[.page_size]
	ja	.pgup_window_ok
	mov	eax,1
	cmp	[.window_line_number],eax
	je	.moved_caret
    .pgup_window_ok:
	call	.find_line
	mov	[.window_line],esi
	mov	[.window_line_number],ecx
	jmp	.moved_caret
  .pgdn:
	test	[kbstate+VK_CONTROL],80h
	jnz	.text_end
	mov	eax,[.caret_line_number]
	mov	eax,[.caret_line_number]
	add	eax,[.page_size]
	call	.find_line
	mov	[.caret_line],esi
	mov	[.caret_line_number],ecx
	mov	eax,[.window_line_number]
	mov	ecx,[.page_size]
	add	eax,ecx
	mov	ebx,[.lines_count]
	sub	ebx,ecx
	jbe	.moved_caret
	inc	ebx
	cmp	eax,ebx
	jb	.pgdn_window_ok
	mov	eax,ebx
	cmp	[.window_line_number],eax
	je	.moved_caret
    .pgdn_window_ok:
	call	.find_line
	mov	[.window_line],esi
	mov	[.window_line_number],ecx
	jmp	.moved_caret
  .text_home:
	mov	eax,[.first_line]
	mov	[.caret_line],eax
	mov	[.caret_line_number],1
	jmp	.moved_caret
  .text_end:
	or	eax,-1
	call	.find_line
	mov	[.caret_line],esi
	mov	[.caret_line_number],ecx
	jmp	.moved_caret
  .word_left:
	mov	ecx,[.caret_position]
	mov	esi,[.caret_line]
	add	esi,8
    .find_left_word:
	sub	ecx,1
	jc	.word_line_up
	mov	al,[esi+ecx]
	call	.recognize_character
	jnc	.find_word_start
	jmp	.find_left_word
    .word_line_up:
	sub	esi,8
	mov	eax,[esi+4]
	or	eax,eax
	jz	.word_left_ok
	mov	[.caret_line],eax
	dec	[.caret_line_number]
	lea	esi,[eax+8]
	mov	ecx,100h
	jmp	.find_left_word
    .find_word_start:
	sub	ecx,1
	jc	.word_left_ok
	mov	al,[esi+ecx]
	call	.recognize_character
	jc	.word_left_ok
	jmp	.find_word_start
    .word_left_ok:
	inc	ecx
	mov	[.caret_position],ecx
	jmp	.moved_caret
  .word_right:
	mov	ecx,[.caret_position]
	mov	esi,[.caret_line]
	add	esi,8
	cmp	ecx,100h
	je	.word_line_down
    .find_word_end:
	add	ecx,1
	jc	.word_line_down
	mov	al,[esi+ecx]
	call	.recognize_character
	jc	.find_right_word
	jmp	.find_word_end
    .find_right_word:
	add	cl,1
	jc	.word_line_down
    .check_right_word:
	mov	al,[esi+ecx]
	call	.recognize_character
	jnc	.word_right_ok
	jmp	.find_right_word
    .word_line_down:
	mov	eax,[esi-8]
	or	eax,eax
	jz	.find_line_end
	mov	[.caret_line],eax
	inc	[.caret_line_number]
	lea	esi,[eax+8]
	xor	ecx,ecx
	jmp	.check_right_word
    .word_right_ok:
	mov	[.caret_position],ecx
	jmp	.moved_caret
  .insert:
	test	[kbstate+VK_MENU],80h
	jnz	.switch_blocks
	test	[kbstate+VK_CONTROL],80h
	jnz	.wmcopy
	test	[kbstate+VK_SHIFT],80h
	jnz	.wmpaste
	xor	[.editor_mode],AEMODE_OVERWRITE
	mov	[.notification],AEN_MODECHANGE
	call	.create_caret
	cmp	[.was_selection],1
	je	.done
	mov	[.sel_line],0
	jmp	.done
  .del:
	test	[kbstate+VK_CONTROL],80h
	jnz	.wmclear
	test	[kbstate+VK_SHIFT],80h
	jnz	.wmcut
	cmp	[.was_selection],0
	je	.no_selection
	test	[.editor_style],AES_SECURESEL
	jz	.wmclear
    .no_selection:
	mov	edi,[.caret_line]
	mov	[.current_line],edi
	test	[.editor_mode],AEMODE_OVERWRITE
	jnz	.delete_char
	add	edi,108h-1
	mov	al,20h
	mov	ecx,100h
	std
	repe	scasb
	setne	al
	movzx	eax,al
	add	ecx,eax
	cld
	mov	edx,[.caret_position]
	cmp	edx,ecx
	jb	.delete_char
	mov	ecx,100h
	sub	ecx,edx
	mov	edi,[.caret_line]
	mov	esi,[edi]
	or	esi,esi
	jz	.ignore
	call	.store_status_for_undo
	call	.store_line_for_undo
	mov	[.current_line],esi
	call	.store_line_for_undo
	mov	ebx,[esi]
	mov	[edi],ebx
	or	ebx,ebx
	jz	.append_line
	mov	[.current_line],ebx
	call	.store_line_for_undo
	mov	[ebx+4],edi
    .append_line:
	or	dword [esi],-1
	dec	[.lines_count]
	lea	edi,[edi+8+edx]
	add	esi,8
	rep	movsb
	mov	[.sel_line],0
	jmp	.text_modified
    .delete_char:
	mov	[.current_operation],VK_DELETE
	cmp	[.last_operation],VK_DELETE
	je	.undo_delete_ok
	call	.store_status_for_undo
	call	.store_line_for_undo
    .undo_delete_ok:
	call	.delete_character
	mov	[.sel_line],0
	jmp	.text_modified
  .back:
	cmp	[.was_selection],0
	je	.no_selection_to_clear
	test	[.editor_style],AES_SECURESEL
	jz	.wmclear
    .no_selection_to_clear:
	mov	eax,[.caret_line]
	mov	[.current_line],eax
	cmp	[.caret_position],0
	je	.line_back
	test	[kbstate+VK_CONTROL],80h
	jnz	.word_back
	mov	[.current_operation],VK_BACK
	cmp	[.last_operation],VK_BACK
	je	.undo_back_ok
	call	.store_status_for_undo
	call	.store_line_for_undo
    .undo_back_ok:
	dec	[.caret_position]
	call	.delete_character
	mov	[.sel_line],0
	jmp	.text_modified
    .word_back:
	call	.store_status_for_undo
	call	.store_line_for_undo
	mov	ecx,[.caret_position]
	mov	esi,[.caret_line]
	add	esi,8
    .skip_spaces:
	sub	ecx,1
	jc	.delete_word
	mov	al,[esi+ecx]
	cmp	al,20h
	je	.skip_spaces
	call	.recognize_character
	jnc	.find_word_to_delete_start
    .find_word_to_delete_end:
	sub	ecx,1
	jc	.delete_word
	mov	al,[esi+ecx]
	call	.recognize_character
	jnc	.delete_word
	jmp	.find_word_to_delete_end
    .find_word_to_delete_start:
	sub	ecx,1
	jc	.delete_word
	mov	al,[esi+ecx]
	call	.recognize_character
	jc	.delete_word
	jmp	.find_word_to_delete_start
    .delete_word:
	inc	ecx
	mov	eax,ecx
	lea	edi,[esi+eax]
	xchg	eax,[.caret_position]
	lea	esi,[esi+eax]
	mov	ecx,100h
	sub	ecx,eax
	rep	movsb
	mov	ecx,[.caret_line]
	add	ecx,108h
	sub	ecx,edi
	mov	al,20h
	rep	stosb
	jmp	.text_modified
    .line_back:
	test	[.editor_mode],AEMODE_OVERWRITE
	jnz	.ignore
	mov	esi,[.caret_line]
	mov	edi,[esi+4]
	or	edi,edi
	jz	.ignore
	call	.store_status_for_undo
	call	.store_line_for_undo
	mov	[.caret_line],edi
	dec	[.caret_line_number]
	mov	[.current_line],edi
	call	.store_line_for_undo
	mov	eax,[esi]
	mov	[edi],eax
	or	dword [esi],-1
	dec	[.lines_count]
	or	eax,eax
	jz	.line_removed
	mov	[.current_line],eax
	call	.store_line_for_undo
	mov	[eax+4],edi
    .line_removed:
	add	edi,108h-1
	mov	al,20h
	mov	ecx,100h
	std
	repe	scasb
	setne	al
	movzx	eax,al
	add	ecx,eax
	add	edi,eax
	cld
	inc	edi
	mov	[.caret_position],ecx
	sub	ecx,100h
	neg	ecx
	add	esi,8
	rep	movsb
	mov	[.sel_line],0
	jmp	.text_modified
  .put_char:
	mov	ebx,[.caret_line]
	mov	[.current_line],ebx
	cmp	al,0Dh
	je	.new_line
	cmp	al,9
	je	.tab
	cmp	[.was_selection],0
	je	.no_selection_to_replace
	call	.store_status_for_undo
	test	[.editor_style],AES_SECURESEL
	jnz	.put_new_char
	push	eax
	call	.delete_block
	pop	eax
	call	.insert_character
	jmp	.text_modified
    .no_selection_to_replace:
	mov	[.current_operation],VK_SPACE
	cmp	[.last_operation],VK_SPACE
	je	.undo_put_ok
	call	.store_status_for_undo
    .put_new_char:
	call	.store_line_for_undo
    .undo_put_ok:
	call	.insert_character
	mov	[.sel_line],0
	jmp	.text_modified
  .tab:
	call	.store_status_for_undo
	cmp	[.was_selection],0
	je	.tab_securesel
	test	[.editor_style],AES_SECURESEL
	jnz	.tab_securesel
	call	.delete_block
	call	.make_tab
	jmp	.text_modified
    .tab_securesel:
	call	.store_line_for_undo
	call	.make_tab
	mov	[.sel_line],0
	jmp	.text_modified
    .make_tab:
	test	[.editor_style],AES_SMARTTABS
	jz	.standard_tab
	mov	esi,[.current_line]
	mov	esi,[esi+4]
	or	esi,esi
	jz	.standard_tab
	mov	edx,[.caret_position]
	lea	edi,[esi+8+edx]
	mov	ecx,100h
	sub	ecx,edx
	jecxz	.standard_tab
	mov	al,20h
	repne	scasb
	jne	.standard_tab
	repe	scasb
	je	.standard_tab
	add	ecx,edx
	sub	ecx,100h-1
	neg	ecx
	jmp	.tab_spaces
    .standard_tab:
	mov	ecx,[.caret_position]
	and	ecx,not 111b
	sub	ecx,[.caret_position]
	add	ecx,8
    .tab_spaces:
	push	ecx
	mov	al,20h
	call	.insert_character
	pop	ecx
	loop	.tab_spaces
	ret
  .new_line:
	call	.store_status_for_undo
	cmp	[.was_selection],0
	je	.new_line_securesel
	test	[.editor_style],AES_SECURESEL
	jnz	.new_line_securesel
	call	.delete_block
    .new_line_securesel:
	test	[.editor_mode],AEMODE_OVERWRITE
	jnz	.next_line
	call	.store_line_for_undo
	call	.allocate_line
	jc	.out_of_memory
	mov	edi,eax
	mov	[.current_line],eax
	call	.store_line_for_undo
	mov	esi,[.caret_line]
	mov	ebx,[esi]
	mov	[.current_line],ebx
	call	.store_line_for_undo
	mov	[edi],ebx
	mov	[edi+4],esi
	mov	[esi],edi
	or	ebx,ebx
	jz	.line_prepared
	mov	[ebx+4],edi
    .line_prepared:
	mov	[.caret_line],edi
	inc	[.caret_line_number]
	add	edi,8
	add	esi,8
	mov	edx,[.caret_position]
	mov	[.caret_position],0
	test	[.editor_style],AES_AUTOINDENT
	jz	.indent_ok
	push	edi
	mov	al,20h
	lea	edi,[esi+edx]
	mov	ecx,100h
	sub	ecx,edx
	repe	scasb
	je	.line_ok
	mov	edx,100h-1
	sub	edx,ecx
    .line_ok:
	mov	edi,esi
	mov	ecx,100h
	repe	scasb
	setne	al
	neg	ecx
	add	ecx,100h-1
	movzx	eax,al
	imul	ecx,eax
	pop	edi
	cmp	ecx,edx
	ja	.indent_ok
	mov	[.caret_position],ecx
    .indent_ok:
	mov	eax,20202020h
	mov	ecx,100h shr 2
	rep	stosd
	inc	[.lines_count]
	cmp	edx,100h
	je	.new_line_ok
	sub	edi,100h
	add	edi,[.caret_position]
	add	esi,edx
	cmp	edx,[.caret_position]
	jae	.position_ok
	mov	edx,[.caret_position]
    .position_ok:
	mov	ecx,100h
	sub	ecx,edx
    .move_data:
	movsb
	mov	byte [esi-1],20h
	loop	.move_data
	jmp	.new_line_ok
    .next_line:
	mov	esi,[.caret_line]
	mov	ebx,[esi]
	or	ebx,ebx
	jnz	.next_line_ok
	call	.store_line_for_undo
	call	.allocate_line
	jc	.out_of_memory
	mov	esi,[.caret_line]
	mov	ebx,eax
	mov	[.current_line],eax
	call	.store_line_for_undo
	mov	[esi],ebx
	mov	[ebx],dword 0
	mov	[ebx+4],esi
	lea	edi,[ebx+8]
	mov	eax,20202020h
	mov	ecx,100h shr 2
	rep	stosd
	inc	[.lines_count]
    .next_line_ok:
	mov	[.caret_line],ebx
	inc	[.caret_line_number]
	mov	[.caret_position],0
    .new_line_ok:
	mov	eax,[.caret_line]
	mov	[.current_line],eax
	call	.store_line_for_undo
	mov	[.sel_line],0
	jmp	.text_modified
  .remove_line:
	mov	esi,[.caret_line]
	mov	[.current_line],esi
	mov	ebx,[esi]
	or	ebx,ebx
	jz	.clear_line
	mov	[.caret_position],0
	call	.store_status_for_undo
	call	.store_line_for_undo
	cmp	esi,[.window_line]
	jne	.window_ok
	mov	[.window_line],ebx
    .window_ok:
	mov	edi,[esi+4]
	mov	[.caret_line],ebx
	mov	[.current_line],ebx
	call	.store_line_for_undo
	mov	[ebx+4],edi
	or	dword [esi],-1
	dec	[.lines_count]
	or	edi,edi
	jz	.removed_first
	mov	[.current_line],edi
	call	.store_line_for_undo
	mov	[edi],ebx
	mov	[.sel_line],0
	jmp	.text_modified
    .removed_first:
	mov	[.first_line],ebx
	mov	[.sel_line],0
	jmp	.text_modified
    .clear_line:
	lea	edi,[esi+8]
	mov	ecx,100h
	mov	al,20h
	repe	scasb
	je	.ignore
	mov	[.caret_position],0
	call	.store_status_for_undo
	call	.store_line_for_undo
	lea	edi,[esi+8]
	mov	eax,20202020h
	mov	ecx,100h shr 2
	rep	stosd
	mov	[.sel_line],0
	jmp	.text_modified
  .duplicate_line:
	mov	esi,[.caret_line]
	mov	[.current_line],esi
	call	.store_status_for_undo
	call	.store_line_for_undo
	call	.allocate_line
	jc	.out_of_memory
	mov	edi,eax
	mov	[.current_line],eax
	call	.store_line_for_undo
	mov	esi,[.caret_line]
	mov	ebx,[esi]
	mov	[.current_line],ebx
	call	.store_line_for_undo
	mov	[edi],ebx
	mov	[edi+4],esi
	mov	[esi],edi
	or	ebx,ebx
	jz	.line_ready
	mov	[ebx+4],edi
    .line_ready:
	add	esi,8
	add	edi,8
	mov	ecx,100h shr 2
	rep	movsd
	inc	[.lines_count]
	jmp	.text_modified
.wmsyskeydown:
	mov	eax,[wparam]
	cmp	eax,VK_INSERT
	je	.switch_blocks
	cmp	eax,VK_BACK
	je	.wmundo
	mov	al,[.last_operation]
	mov	[.current_operation],al
	invoke	ShowCaret,[hwnd]
	jmp	.defwndproc
  .switch_blocks:
	xor	[.editor_mode],AEMODE_VERTICALSEL
	mov	[.notification],AEN_MODECHANGE
	cmp	[.was_selection],0
	je	.ignore
	jmp	.moved_window
.wmchar:
	test	[lparam],1 shl 31
	jz	.ignore
	mov	eax,[wparam]
	jmp	.put_char
.wmlbuttondown:
	cmp	[.focus],0
	jne	.focus_ok
	invoke	SetFocus,[hwnd]
	mov	esi,[.editor_memory]
	add	esi,8
	lea	edi,[.editor_data]
	mov	ecx,.editor_data_size shr 2
	rep	movsd
    .focus_ok:
	invoke	GetKeyboardState,kbstate
	cmp	[.was_selection],0
	jne	.sel_ok
	mov	eax,[.caret_line]
	mov	[.sel_line],eax
	mov	eax,[.caret_position]
	mov	[.sel_position],eax
	mov	eax,[.caret_line_number]
	mov	[.sel_line_number],eax
    .sel_ok:
	call	.get_mouse_position
	invoke	SetCapture,[hwnd]
	or	[.mouse_select],-1
	jmp	.moved_caret
    .get_mouse_position:
	mov	ax,word [lparam]
	cwde
	cdq
	idiv	[.font_width]
	add	eax,[.window_position]
	cmp	eax,0
	jl	.lowest_position
	cmp	eax,100h
	jg	.highest_position
	jmp	.click_position_ok
    .lowest_position:
	xor	eax,eax
	jmp	.click_position_ok
    .highest_position:
	mov	eax,100h
    .click_position_ok:
	mov	[.caret_position],eax
	mov	ax,word [lparam+2]
	cwde
	cdq
	idiv	[.font_height]
	add	eax,[.window_line_number]
	cmp	eax,0
	jg	.click_line_ok
	mov	eax,1
    .click_line_ok:
	call	.find_line
	mov	[.caret_line],esi
	mov	[.caret_line_number],ecx
	ret
.wmlbuttonup:
	invoke	ReleaseCapture
	mov	[.mouse_select],0
	jmp	.done
.wmmousemove:
	cmp	[.mouse_select],0
	je	.ignore
	cmp	[.was_selection],0
	jne	.select
	mov	eax,[.caret_line]
	mov	ebx,[.caret_line_number]
	mov	[.sel_line],eax
	mov	[.sel_line_number],ebx
	mov	eax,[.caret_position]
	mov	[.sel_position],eax
    .select:
	call	.get_mouse_position
	jmp	.moved_selection
.wmlbuttondblclk:
	mov	[.mouse_select],0
	call	.get_mouse_position
	call	.get_word_edges
	mov	[.sel_position],edx
	mov	[.caret_position],ecx
	mov	eax,[.caret_line]
	mov	ebx,[.caret_line_number]
	mov	[.sel_line],eax
	mov	[.sel_line_number],ebx
	jmp	.moved_selection
.wmcopy:
	cmp	[.was_selection],0
	je	.ignore
	call	.copy_to_clipboard
	jmp	.ignore
    .copy_to_clipboard:
	call	.get_block_size
	invoke	GlobalAlloc,GMEM_MOVEABLE+GMEM_DDESHARE,ecx
	mov	ebx,eax
	invoke	GlobalLock,ebx
	mov	edi,eax
	push	ebx
	call	.copy_block
	pop	ebx
	invoke	GlobalUnlock,ebx
	invoke	OpenClipboard,[hwnd]
	invoke	EmptyClipboard
	invoke	SetClipboardData,CF_TEXT,ebx
	or	eax,eax
	jz	.copy_failed
	invoke	CloseClipboard
	ret
    .copy_failed:
	invoke	GlobalFree,ebx
	ret
.wmcut:
	cmp	[.was_selection],0
	je	.ignore
	call	.copy_to_clipboard
.wmclear:
	cmp	[.was_selection],0
	je	.ignore
	call	.store_status_for_undo
	call	.delete_block
	mov	[.sel_line],0
	jmp	.text_modified
.wmpaste:
	invoke	OpenClipboard,NULL
	invoke	GetClipboardData,CF_TEXT
	or	eax,eax
	jz	.close_clipboard
	mov	ebx,eax
	invoke	GlobalLock,ebx
	mov	esi,eax
	push	ebx
	call	.store_status_for_undo
	call	.get_selection_start_position
	cmp	[.was_selection],0
	je	.do_paste
	test	[.editor_style],AES_SECURESEL
	jnz	.do_paste
	push	esi
	call	.delete_block
	pop	esi
    .do_paste:
	call	.insert_block
	pop	ebx
	jc	.close_clipboard
	invoke	GlobalUnlock,ebx
	invoke	CloseClipboard
	test	[.editor_style],AES_SECURESEL
	jnz	.text_modified
	mov	eax,[.sel_line]
	mov	ebx,[.sel_position]
	mov	ecx,[.sel_line_number]
	mov	[.caret_line],eax
	mov	[.caret_position],ebx
	mov	[.caret_line_number],ecx
	jmp	.text_modified
    .close_clipboard:
	invoke	CloseClipboard
	jmp	.ignore
    .get_selection_start_position:
	mov	eax,[.sel_line_number]
	mov	ebx,[.sel_position]
	cmp	[.was_selection],0
	je	.new_position_ok
	cmp	eax,[.caret_line_number]
	ja	.new_position_ok
	jb	.update_position
	cmp	ebx,[.caret_position]
	ja	.new_position_ok
    .update_position:
	xchg	eax,[.caret_line_number]
	xchg	ebx,[.caret_position]
	push	eax ebx
	call	.update_positions
	pop	[.caret_position] [.caret_line_number]
    .new_position_ok:
	ret
.wmsettext:
	call	.reset_editor_memory
	call	.allocate_line
	mov	[.first_line],eax
	mov	[.lines_count],1
	mov	[.caret_line],eax
	mov	[.caret_line_number],1
	mov	[.window_line],eax
	mov	[.window_line_number],1
	mov	edi,eax
	xor	eax,eax
	stosd
	stosd
	mov	eax,20202020h
	mov	ecx,100h shr 2
	rep	stosd
	mov	[.caret_position],0
	mov	[.window_position],0
	mov	[.sel_line],0
	mov	esi,[lparam]
	or	esi,esi
	jz	.text_modified
	mov	ebx,[.first_line]
	lea	edi,[ebx+8]
	mov	ecx,100h
    .copy_line:
	lodsb
	or	al,al
	jz	.text_modified
	cmp	al,0Ah
	je	.copy_lf
	cmp	al,0Dh
	je	.copy_cr
	cmp	al,9
	je	.copy_tab
	stosb
	loop	.copy_line
    .skip_overlap:
	lodsb
	or	al,al
	jz	.text_modified
	cmp	al,0Ah
	je	.copy_lf
	cmp	al,0Dh
	je	.copy_cr
	jmp	.skip_overlap
    .copy_tab:
	mov	edx,ecx
	and	ecx,111b
	setz	al
	shl	al,3
	or	cl,al
	sub	edx,ecx
	jc	.skip_overlap
	mov	al,20h
	rep	stosb
	mov	ecx,edx
	jmp	.copy_line
    .copy_lf:
	cmp	byte [esi],0Dh
	jne	.copy_new_line
	inc	esi
	jmp	.copy_new_line
    .copy_cr:
	cmp	byte [esi],0Ah
	jne	.copy_new_line
	inc	esi
    .copy_new_line:
	push	ebx esi
	call	.allocate_line
	jc	.out_of_memory
	pop	esi ebx
	mov	edi,eax
	mov	edx,[ebx]
	mov	[edi],edx
	mov	[edi+4],ebx
	mov	[ebx],edi
	mov	ebx,edi
	add	edi,8
	mov	eax,20202020h
	mov	ecx,100h shr 2
	rep	stosd
	inc	[.lines_count]
	or	edx,edx
	jz	.do_copy
	mov	[edx+4],ebx
    .do_copy:
	mov	ecx,100h
	lea	edi,[ebx+8]
	jmp	.copy_line
.wmgettextlength:
	mov	esi,[.first_line]
	xor	edx,edx
    .get_line_length:
	lea	edi,[esi+8+100h-1]
	mov	ecx,100h
	mov	al,20h
	std
	repe	scasb
	cld
	setne	al
	movzx	eax,al
	add	edx,ecx
	add	edx,eax
	add	edx,2
	mov	esi,[esi]
	or	esi,esi
	jnz	.get_line_length
	sub	edx,2
	mov	[.return_value],edx
	jmp	.ignore
.wmgettext:
	mov	edi,[lparam]
	mov	ebx,[.first_line]
	mov	edx,[wparam]
	sub	edx,1
	jc	.ignore
    .get_line:
	push	edi
	lea	edi,[ebx+8+100h-1]
	mov	ecx,100h
	mov	al,20h
	std
	repe	scasb
	cld
	setne	al
	movzx	eax,al
	add	ecx,eax
	pop	edi
	lea	esi,[ebx+8]
	test	[.editor_style],AES_OPTIMALFILL
	jz	.copy_data
    .optimal_fill:
	cmp	ecx,8
	jbe	.copy_data
	push	ecx edi
	lea	edi,[esi+7]
	mov	al,20h
	mov	ecx,8
	std
	repe	scasb
	cld
	setne	al
	add	cl,al
	pop	edi eax
	cmp	ecx,7
	jb	.put_tab
	mov	ecx,8
	sub	edx,ecx
	jc	.cut_overlap
	rep	movsb
	mov	ecx,eax
	sub	ecx,8
	jmp	.optimal_fill
    .put_tab:
	sub	edx,ecx
	jc	.cut_overlap
	push	esi
	rep	movsb
	pop	esi
	add	esi,8
	or	edx,edx
	jz	.end_text
	dec	edx
	mov	ecx,eax
	sub	ecx,8
	mov	al,9
	stosb
	jmp	.optimal_fill
    .copy_data:
	sub	edx,ecx
	jc	.cut_overlap
	rep	movsb
	mov	ebx,[ebx]
	or	ebx,ebx
	jz	.end_text
	sub	edx,2
	jc	.end_text
	mov	ax,0A0Dh
	stosw
	jmp	.get_line
    .cut_overlap:
	neg	edx
	sub	ecx,edx
	rep	movsb
    .end_text:
	xor	al,al
	stosb
	mov	eax,edi
	dec	eax
	sub	eax,[lparam]
	mov	[.return_value],eax
	jmp	.ignore
.wmsetfont:
	mov	esi,[wparam]
	or	esi,esi
	jnz	.get_metrics
	mov	esi,[asmedit_font]
    .get_metrics:
	invoke	GetDC,[hwnd]
	mov	ebx,eax
	invoke	SelectObject,ebx,esi
	invoke	GetTextMetrics,ebx,tm
	invoke	ReleaseDC,[hwnd],ebx
	test	[tm.tmPitchAndFamily],TMPF_FIXED_PITCH
	jnz	.ignore
	mov	[.return_value],esi
	mov	[.editor_font],esi
	mov	eax,[tm.tmHeight]
	mov	[.font_height],eax
	mov	eax,[tm.tmAveCharWidth]
	mov	[.font_width],eax
	call	.create_caret
	mov	eax,[lparam]
	mov	[.redraw_now],al
	jmp	.wmsize
.wmgetfont:
	mov	eax,[.editor_font]
	cmp	eax,[asmedit_font]
	je	.ignore
	mov	[.return_value],eax
	jmp	.ignore
.wmundo:
	cmp	[.undo_data],0
	je	.ignore
	call	.undo
	mov	[.last_operation],0
	call	.create_caret
	jmp	.text_modified
.emcanundo:
	mov	eax,[.undo_data]
	or	eax,eax
	jz	.ignore
	mov	[.return_value],TRUE
	jmp	.ignore
.ememptyundobuffer:
	call	.clear_undo_data
	jmp	.ignore
.emreplacesel:
	test	[.editor_mode],AEMODE_VERTICALSEL
	jnz	.ignore
	push	[.editor_mode]
	and	[.editor_mode],not AEMODE_OVERWRITE
	push	[.caret_line] [.caret_position]
	cmp	[wparam],0
	je	.status_ok
	call	.store_status_for_undo
    .status_ok:
	call	.get_selection_start_position
	cmp	[.was_selection],0
	je	.ready_for_replace
	call	.delete_block
	mov	[.sel_line],0
    .ready_for_replace:
	mov	esi,[lparam]
	call	.insert_block
	mov	[.notification],AEN_TEXTCHANGE
	pop	eax esi
	pop	[.editor_mode]
	sub	esi,[.caret_line]
	sub	eax,[.caret_position]
	or	eax,esi
	jz	.text_modified
	mov	eax,[.sel_line]
	xchg	eax,[.caret_line]
	mov	[.sel_line],eax
	mov	eax,[.sel_line_number]
	xchg	eax,[.caret_line_number]
	mov	[.sel_line_number],eax
	mov	eax,[.sel_position]
	xchg	eax,[.caret_position]
	mov	[.sel_position],eax
	jmp	.text_modified
.aemsetmode:
	mov	eax,[wparam]
	xchg	[.editor_mode],eax
	cmp	eax,[.editor_mode]
	je	.ignore
	mov	[.notification],AEN_MODECHANGE
	call	.create_caret
	cmp	[.was_selection],0
	jne	.moved_window
	jmp	.done
.aemgetmode:
	mov	eax,[.editor_mode]
	mov	[.return_value],eax
	jmp	.ignore
.aemsetsyntaxhighlight:
	mov	eax,[wparam]
	mov	ebx,[lparam]
	mov	[.syntax_colors],eax
	mov	[.syntax_proc],ebx
	or	eax,eax
	jnz	.wmsize
	mov	[.syntax_proc],SyntaxProc
	jmp	.wmsize
.aemsettextcolor:
	mov	eax,[wparam]
	mov	ebx,[lparam]
	mov	[.text_color],eax
	mov	[.background_color],ebx
	jmp	.wmsize
.aemsetselcolor:
	mov	eax,[wparam]
	mov	ebx,[lparam]
	mov	[.selection_text],eax
	mov	[.selection_background],ebx
	jmp	.wmsize
.aemgetlinedata:
	mov	esi,[.caret_line]
	mov	eax,[wparam]
	or	eax,eax
	jz	.line_data_ok
	call	.find_line
	cmp	ecx,[wparam]
	jne	.ignore
    .line_data_ok:
	add	esi,8
	mov	edi,[lparam]
	mov	ecx,100h shr 2
	rep	movsd
	mov	[.return_value],100h
	jmp	.ignore
.aemsetpos:
	mov	edi,[wparam]
	cmp	[.sel_line],0
	jne	.sel_start_ok
	mov	eax,[.caret_line]
	mov	[.sel_line],eax
	mov	eax,[.caret_line_number]
	mov	[.sel_line_number],eax
	mov	eax,[.caret_position]
	mov	[.sel_position],eax
    .sel_start_ok:
	mov	eax,[edi+rect.top-rect]
	or	eax,eax
	jz	.sel_line_ok
	call	.find_line
	mov	[.sel_line],esi
	mov	[.sel_line_number],ecx
    .sel_line_ok:
	mov	eax,[edi+rect.left-rect]
	or	eax,eax
	jz	.sel_position_ok
	dec	eax
	mov	[.sel_position],eax
	cmp	eax,100h
	jbe	.sel_position_ok
	mov	[.sel_position],100h
    .sel_position_ok:
	mov	eax,[edi+rect.bottom-rect]
	or	eax,eax
	jz	.caret_line_ok
	call	.find_line
	mov	[.caret_line],esi
	mov	[.caret_line_number],ecx
    .caret_line_ok:
	mov	eax,[edi+rect.right-rect]
	or	eax,eax
	jz	.caret_position_ok
	dec	eax
	cmp	eax,100h
	jbe	.caret_position_ok
	mov	eax,100h
    .caret_position_ok:
	mov	[.caret_position],eax
	cmp	esi,[.sel_line]
	jne	.moved_selection
	cmp	eax,[.sel_position]
	jne	.moved_selection
	mov	[.sel_line],0
	jmp	.moved_selection
.aemgetpos:
	mov	edi,[wparam]
	mov	eax,[.caret_line_number]
	mov	[edi+rect.top-rect],eax
	mov	[edi+rect.bottom-rect],eax
	mov	eax,[.caret_position]
	inc	eax
	mov	[edi+rect.left-rect],eax
	mov	[edi+rect.right-rect],eax
	cmp	[.sel_line],0
	je	.ignore
	mov	eax,[.sel_line_number]
	mov	[edi+rect.top-rect],eax
	mov	eax,[.sel_position]
	inc	eax
	mov	[edi+rect.left-rect],eax
	jmp	.ignore
.aemfindfirst:
	mov	eax,[.search_text]
	or	eax,eax
	jnz	.buffer_ok
	call	.allocate_line
	jc	.out_of_memory
    .buffer_ok:
	mov	esi,[lparam]
	mov	edi,eax
	mov	eax,[wparam]
	push	edi
	stosd
	stosd
	mov	edx,eax
	mov	ebx,case_table
	mov	ecx,100h
	lodsb
    .copy_text:
	test	edx,AEFIND_CASESENSITIVE
	jnz	.text_case_ok
	xlatb
    .text_case_ok:
	stosb
	lodsb
	or	al,al
	loopnz	.copy_text
	pop	edi
	jnz	.ignore
	neg	ecx
	add	ecx,100h
	mov	[edi+4],ecx
	mov	[.search_text],edi
.aemfindnext:
	mov	esi,[.search_text]
	or	esi,esi
	jz	.ignore
	cmp	dword [esi+4],0
	je	.ignore
	mov	edi,[.caret_line]
	mov	ecx,[.caret_line_number]
	mov	edx,[.caret_position]
	test	byte [esi],AEFIND_BACKWARD
	jnz	.search_backward
    .do_search:
	push	ecx
	call	.find_text
	pop	ecx
	jnc	.text_found
	inc	ecx
	xor	edx,edx
	mov	edi,[edi]
	or	edi,edi
	jnz	.do_search
	jmp	.search_done
    .text_found:
	mov	[.caret_line],edi
	mov	[.sel_line],edi
	mov	[.caret_line_number],ecx
	mov	[.sel_line_number],ecx
	mov	[.caret_position],eax
	mov	esi,[.search_text]
	add	eax,[esi+4]
	mov	[.sel_position],eax
	call	.update_positions
	call	.let_caret_appear
	mov	eax,[.caret_position]
	xchg	eax,[.sel_position]
	mov	[.caret_position],eax
	mov	[.return_value],TRUE
	jmp	.moved_selection
    .backward_found:
	mov	[.caret_line],edi
	mov	[.sel_line],edi
	mov	[.caret_line_number],ecx
	mov	[.sel_line_number],ecx
	mov	[.sel_position],eax
	mov	esi,[.search_text]
	add	eax,[esi+4]
	mov	[.caret_position],eax
	call	.update_positions
	call	.let_caret_appear
	mov	eax,[.caret_position]
	xchg	eax,[.sel_position]
	mov	[.caret_position],eax
	mov	[.return_value],TRUE
	jmp	.moved_selection
    .search_backward:
	push	ecx
	call	.find_text
	pop	ecx
	jnc	.backward_found
	dec	ecx
	mov	edx,100h
	mov	edi,[edi+4]
	or	edi,edi
	jnz	.search_backward
    .search_done:
	xor	eax,eax
	mov	esi,[.search_text]
	mov	[esi],eax
	mov	[.search_text],eax
	jmp	.done
.aemcanfindnext:
	cmp	[.search_text],0
	je	.ignore
	mov	[.return_value],TRUE
	jmp	.ignore
.aemgetwordatcaret:
	call	.get_word_edges
	mov	edi,[lparam]
	lea	esi,[esi+8+edx]
	sub	ecx,edx
	inc	ecx
	cmp	ecx,[wparam]
	jbe	.size_ok
	mov	ecx,[wparam]
    .size_ok:
	dec	ecx
	rep	movsb
	xor	al,al
	stosb
	jmp	.ignore
    .get_word_edges:
	mov	esi,[.caret_line]
	mov	edx,[.caret_position]
    .find_left_edge:
	or	edx,edx
	jz	.left_edge_ok
	mov	al,[esi+8+edx-1]
	call	.recognize_character
	jc	.left_edge_ok
	dec	edx
	jmp	.find_left_edge
    .left_edge_ok:
	mov	ecx,[.caret_position]
    .find_right_edge:
	cmp	ecx,100h
	je	.right_edge_ok
	mov	al,[esi+8+ecx]
	call	.recognize_character
	jc	.right_edge_ok
	inc	ecx
	jmp	.find_right_edge
    .right_edge_ok:
	ret
.moved_caret:
	test	[kbstate+VK_SHIFT],80h
	jnz	.moved_selection
	mov	[.sel_line],0
.moved_selection:
	mov	[.notification],AEN_POSCHANGE
	jmp	.update
.moved_window:
	call	.update_positions
	jmp	.refresh
.text_modified:
	mov	[.notification],AEN_TEXTCHANGE
    .update:
	call	.update_positions
	call	.let_caret_appear
    .refresh:
	mov	eax,[.editor_screen]
	or	eax,eax
	jz	.wmsize
	push	eax
	call	.update_screen
	mov	esi,[esp]
	mov	edi,[.editor_screen]
	or	edi,edi
	jz	.refresh_failed
	mov	[rect.top],0
	mov	edx,[.font_height]
	mov	[rect.bottom],edx
	mov	ecx,[.screen_height]
    .refresh_screen:
	push	ecx
	mov	ecx,[.screen_width]
	mov	ebx,[.screen_height]
	imul	ebx,ecx
	mov	edx,[.font_width]
	xor	eax,eax
	mov	[rect.left],eax
	mov	[rect.right],eax
    .refresh_line:
	mov	al,[esi]
	mov	ah,[esi+ebx]
	cmp	al,[edi]
	jne	.refresh_changed
	cmp	ah,[edi+ebx]
	jne	.refresh_changed
	inc	esi
	inc	edi
	add	[rect.left],edx
	add	[rect.right],edx
	loop	.refresh_line
	jmp	.refresh_next_line
    .refresh_changed:
	mov	al,[esi]
	mov	ah,[esi+ebx]
	inc	esi
	add	[rect.right],edx
	cmp	al,[edi]
	jne	.changed_more
	cmp	ah,[edi+ebx]
	jne	.changed_more
	inc	edi
	jmp	.invalidate
    .changed_more:
	inc	edi
	loop	.refresh_changed
    .invalidate:
	push	ecx edx
	invoke	InvalidateRect,[hwnd],rect,FALSE
	pop	edx ecx
	mov	eax,[rect.right]
	mov	[rect.left],eax
	jecxz	.refresh_next_line
	loop	.refresh_line
    .refresh_next_line:
	mov	eax,[.font_height]
	add	[rect.top],eax
	add	[rect.bottom],eax
	pop	ecx
	dec	ecx
	jnz	.refresh_screen
	invoke	GlobalFree
	jmp	.done
    .refresh_failed:
	pop	[.editor_screen]
	jmp	.wmsize
.ignore:
	mov	dl,[.last_operation]
	mov	[.current_operation],dl
	cmp	[.was_selection],0
	jne	.done
	mov	[.sel_line],0
.done:
	cmp	[.focus],0
	je	.caret_ok
	call	.update_caret_position
	invoke	ShowCaret,[hwnd]
    .caret_ok:
	lea	esi,[.editor_data]
	mov	edi,[.editor_memory]
	add	edi,8
	mov	ecx,.editor_data_size shr 2
	rep	movsd
	cmp	[.notification],0
	je	.notification_ok
	invoke	GetWindowLong,[hwnd],GWL_HWNDPARENT
	mov	edi,eax
	invoke	GetWindowLong,[hwnd],GWL_ID
	movzx	ebx,[.notification]
	shl	ebx,16
	or	eax,ebx
	invoke	SendMessage,edi,WM_COMMAND,eax,[hwnd]
    .notification_ok:
	cmp	[.redraw_now],0
	je	.redraw_ok
	invoke	UpdateWindow,[hwnd]
    .redraw_ok:
	mov	eax,[.return_value]
.finish:
	pop	edi esi ebx
	return
.out_of_memory:
	call	.undo
.not_enough_memory:
	lea	esp,[.editor_memory-10h]
	mov	[.notification],AEN_OUTOFMEMORY
	or	[.return_value],-1
	jmp	.ignore

.update_positions:
	invoke	GetClientRect,[hwnd],rect
	mov	eax,[rect.right]
	sub	eax,[rect.left]
	cdq
	div	[.font_width]
	add	edx,-1
	adc	eax,0
	mov	[.screen_width],eax
	mov	eax,[rect.bottom]
	sub	eax,[rect.top]
	cdq
	div	[.font_height]
	add	edx,-1
	adc	eax,0
	mov	[.screen_height],eax
    .setup_vscroll:
	mov	ecx,[.lines_count]
	mov	[sc.cbSize],sizeof.SCROLLINFO
	mov	[sc.fMask],SIF_DISABLENOSCROLL+SIF_RANGE+SIF_PAGE+SIF_POS
	mov	[sc.nMin],1
	mov	[sc.nMax],ecx
	mov	eax,[rect.bottom]
	sub	eax,[rect.top]
	xor	edx,edx
	div	[.font_height]
	mov	[.page_size],eax
	mov	[sc.nPage],eax
	mov	edx,[.window_line_number]
	mov	[sc.nPos],edx
	cmp	edx,1
	je	.vscroll_ok
	add	edx,eax
	dec	edx
	cmp	edx,ecx
	jle	.vscroll_ok
	sub	edx,ecx
    .vscroll_correction:
	mov	esi,[.window_line]
	mov	esi,[esi+4]
	or	esi,esi
	jz	.setup_vscroll
	mov	[.window_line],esi
	dec	[.window_line_number]
	dec	edx
	jnz	.vscroll_correction
	jmp	.setup_vscroll
    .vscroll_ok:
	test	[.editor_style],WS_VSCROLL
	jz	.setup_hscroll
	invoke	SetScrollInfo,[hwnd],SB_VERT,sc,TRUE
    .setup_hscroll:
	mov	[sc.nMin],0
	mov	[sc.nMax],100h
	mov	eax,[rect.right]
	sub	eax,[rect.left]
	xor	edx,edx
	div	[.font_width]
	mov	[sc.nPage],eax
	mov	edx,[.window_position]
	mov	[sc.nPos],edx
	or	edx,edx
	jz	.hscroll_ok
	add	edx,eax
	cmp	edx,101h
	jbe	.hscroll_ok
	sub	edx,101h
	sub	[.window_position],edx
	jnc	.setup_hscroll
	mov	[.window_position],0
	jmp	.setup_hscroll
    .hscroll_ok:
	test	[.editor_style],WS_HSCROLL
	jz	.setup_caret
	invoke	SetScrollInfo,[hwnd],SB_HORZ,sc,TRUE
    .setup_caret:
	mov	eax,[.font_width]
	mov	edx,[.caret_position]
	sub	edx,[.window_position]
	imul	eax,edx
	mov	[.caret_x],eax
	mov	eax,[.font_height]
	mov	edx,[.caret_line_number]
	sub	edx,[.window_line_number]
	imul	eax,edx
	mov	[.caret_y],eax
	ret
.let_caret_appear:
	mov	eax,[.caret_position]
	cmp	eax,[.window_position]
	jl	.horizontal_correction
	mov	eax,[rect.right]
	sub	eax,[rect.left]
	xor	edx,edx
	div	[.font_width]
	or	eax,eax
	jz	.horizontal_check
	dec	eax
    .horizontal_check:
	neg	eax
	add	eax,[.caret_position]
	cmp	[.window_position],eax
	jge	.horizontal_ok
    .horizontal_correction:
	mov	[.window_position],eax
	call	.update_positions
    .horizontal_ok:
	mov	esi,[.caret_line]
	mov	ecx,[.caret_line_number]
	cmp	ecx,[.window_line_number]
	jl	.vertical_correction
	mov	eax,[rect.bottom]
	sub	eax,[rect.top]
	xor	edx,edx
	div	[.font_height]
	or	eax,eax
	jz	.vertical_check
	dec	eax
    .vertical_check:
	neg	eax
	add	eax,[.caret_line_number]
	cmp	[.window_line_number],eax
	jge	.vertical_ok
	mov	esi,[.window_line]
	mov	ecx,[.window_line_number]
    .vertical_find:
	mov	esi,[esi]
	inc	ecx
	cmp	ecx,eax
	jl	.vertical_find
    .vertical_correction:
	mov	[.window_line],esi
	mov	[.window_line_number],ecx
	call	.update_positions
    .vertical_ok:
	ret
.create_caret:
	xor	eax,eax
	test	[.editor_mode],AEMODE_OVERWRITE
	jnz	.block_caret
	test	[.editor_style],AES_CONSOLECARET
	jnz	.console_caret
	invoke	CreateCaret,[hwnd],NULL,0,[.font_height]
	jmp	.update_caret_position
    .block_caret:
	invoke	CreateCaret,[hwnd],NULL,[.font_width],[.font_height]
	jmp	.update_caret_position
    .console_caret:
	invoke	CreateCaret,[hwnd],NULL,[.font_width],2
.update_caret_position:
	test	[.editor_mode],AEMODE_OVERWRITE
	jnz	.set_position
	test	[.editor_style],AES_CONSOLECARET
	jz	.set_position
	mov	eax,[.caret_y]
	add	eax,[.font_height]
	sub	eax,2
	invoke	SetCaretPos,[.caret_x],eax
	ret
      .set_position:
	invoke	SetCaretPos,[.caret_x],[.caret_y]
	ret
.update_screen:
	mov	eax,[.screen_width]
	mul	[.screen_height]
	mov	ebx,eax
	shl	eax,1
	or	eax,eax
	jz	.screen_allocated
	invoke	GlobalAlloc,GMEM_FIXED,eax
    .screen_allocated:
	mov	[.editor_screen],eax
	or	eax,eax
	jz	.screen_prepared
	mov	edi,eax
	add	ebx,edi
	push	ebx
	mov	ecx,[.screen_height]
	mov	esi,[.window_line]
    .prepare_screen:
	push	ecx
	push	edi
	mov	edi,line_colors
	mov	ecx,100h shr 2
	xor	eax,eax
	rep	stosd
	lea	eax,[esi+8]
	invoke	.syntax_proc,eax,line_colors
	pop	edi
	push	esi
	mov	edx,[.window_position]
	lea	esi,[esi+8+edx]
	mov	eax,100h
	sub	eax,edx
	cmp	eax,[.screen_width]
	jbe	.prepare_line
	mov	eax,[.screen_width]
    .prepare_line:
	mov	ecx,eax
	rep	movsb
	lea	esi,[line_colors+edx]
	mov	ecx,eax
	xchg	edi,ebx
	rep	movsb
	xchg	edi,ebx
	pop	esi
	cmp	eax,[.screen_width]
	je	.prepare_next_line
	mov	ecx,[.screen_width]
	sub	ecx,eax
	xor	al,al
	mov	edx,ecx
	rep	stosb
	xchg	edi,ebx
	mov	ecx,edx
	rep	stosb
	xchg	edi,ebx
    .prepare_next_line:
	pop	ecx
	mov	esi,[esi]
	dec	ecx
	jz	.prepare_selection
	or	esi,esi
	jnz	.prepare_screen
    .prepare_empty_lines:
	imul	ecx,[.screen_width]
	xor	al,al
	mov	edx,ecx
	rep	stosb
	xchg	edi,ebx
	mov	ecx,edx
	rep	stosb
    .prepare_selection:
	pop	ebx
	test	[.editor_style],ES_NOHIDESEL
	jnz	.hidesel_ok
	cmp	[.focus],0
	je	.screen_prepared
    .hidesel_ok:
	cmp	[.sel_line],0
	je	.screen_prepared
	mov	eax,[.window_line_number]
	mov	esi,[.sel_line_number]
	mov	edi,[.caret_line_number]
	sub	esi,eax
	sub	edi,eax
	mov	ecx,[.window_position]
	mov	eax,[.sel_position]
	mov	edx,[.caret_position]
	sub	eax,ecx
	sub	edx,ecx
	cmp	esi,edi
	jle	.sel_boundaries_ok
	xchg	esi,edi
	xchg	eax,edx
    .sel_boundaries_ok:
	mov	ecx,[.screen_height]
	cmp	edi,0
	jl	.screen_prepared
	cmp	esi,ecx
	jge	.screen_prepared
	cmp	esi,edi
	je	.prepare_vsel
	test	[.editor_mode],AEMODE_VERTICALSEL
	jz	.prepare_hsel
    .prepare_vsel:
	cmp	eax,edx
	jle	.vsel_boundaries_ok
	xchg	eax,edx
    .vsel_boundaries_ok:
	cmp	esi,0
	jge	.vsel_start_ok
	xor	esi,esi
    .vsel_start_ok:
	inc	edi
	cmp	edi,ecx
	jle	.vsel_end_ok
	mov	edi,ecx
    .vsel_end_ok:
	mov	ecx,[.screen_width]
	cmp	edx,0
	jl	.screen_prepared
	cmp	eax,ecx
	jge	.screen_prepared
	cmp	eax,0
	jge	.vsel_line_start_ok
	xor	eax,eax
    .vsel_line_start_ok:
	cmp	edx,ecx
	jle	.vsel_line_end_ok
	mov	edx,ecx
    .vsel_line_end_ok:
	mov	ecx,edi
	sub	ecx,esi
	imul	esi,[.screen_width]
	add	ebx,esi
    .prepare_vsel_line:
	push	eax ecx
	mov	edi,ebx
	mov	ecx,edx
	sub	ecx,eax
	lea	edi,[ebx+eax]
	mov	al,80h
	rep	stosb
	add	ebx,[.screen_width]
	pop	ecx eax
	loop	.prepare_vsel_line
	jmp	.screen_prepared
    .prepare_hsel:
	cmp	esi,0
	jge	.hsel_start_ok
	xor	esi,esi
	xor	eax,eax
    .hsel_start_ok:
	cmp	edi,ecx
	jl	.hsel_end_ok
	mov	edi,ecx
	xor	edx,edx
    .hsel_end_ok:
	inc	esi
	mov	ecx,edi
	sub	ecx,esi
	imul	ecx,[.screen_width]
	imul	esi,[.screen_width]
	lea	edi,[ebx+esi]
	neg	eax
	add	eax,[.screen_width]
	cmp	eax,0
	jle	.hsel_start_line_ok
	sub	edi,eax
	add	ecx,eax
	sub	eax,[.screen_width]
	jle	.hsel_start_line_ok
	add	edi,eax
	sub	ecx,eax
    .hsel_start_line_ok:
	cmp	edx,0
	jle	.hsel_end_line_ok
	add	ecx,edx
	sub	edx,[.screen_width]
	jle	.hsel_end_line_ok
	sub	ecx,edx
    .hsel_end_line_ok:
	mov	al,80h
	rep	stosb
    .screen_prepared:
	ret

.init_editor_memory:
	invoke	VirtualAlloc,0,10800h,MEM_COMMIT,PAGE_READWRITE
	or	eax,eax
	jz	.memory_error
	mov	[.editor_memory],eax
	mov	dword [eax],0
	mov	dword [eax+4],0
	lea	ebx,[eax+108h]
	mov	[.unallocated_lines],ebx
	mov	[.memory_search_line],ebx
	add	eax,10800h
	mov	[.unallocated_lines_end],eax
	mov	[.memory_search_block],eax
	clc
	ret
    .memory_error:
	stc
	ret
.reset_editor_memory:
	mov	esi,[.editor_memory]
	lea	eax,[esi+108h]
	mov	[.unallocated_lines],eax
	mov	[.memory_search_line],eax
	lea	eax,[esi+10800h]
	mov	[.unallocated_lines_end],eax
	mov	[.memory_search_block],eax
    .decommit:
	mov	ebx,[esi]
	or	ebx,ebx
	jz	.decommit_done
	invoke	VirtualFree,ebx,0,MEM_RELEASE
	mov	esi,ebx
	jmp	.decommit
    .decommit_done:
	mov	eax,[.editor_memory]
	mov	[eax],dword 0
	mov	[.undo_data],0
	mov	[.search_text],0
	mov	[.current_operation],0
	ret
.release_editor_memory:
	mov	esi,[.editor_memory]
    .release:
	mov	ebx,[esi]
	invoke	VirtualFree,esi,0,MEM_RELEASE
	mov	esi,ebx
	or	esi,esi
	jnz	.release
	mov	[.editor_memory],0
	ret
.allocate_line:
	mov	eax,[.unallocated_lines]
	mov	ebx,[.memory_search_block]
	mov	esi,[.memory_search_line]
	cmp	eax,[.unallocated_lines_end]
	je	.find_free_line
	add	[.unallocated_lines],108h
	clc
	ret
    .find_free_line:
	cmp	esi,ebx
	je	.find_in_next_block
	cmp	dword [esi],-1
	je	.reuse_line
	add	esi,108h
	cmp	esi,[.memory_search_line]
	jne	.find_free_line
	sub	ebx,10800h
    .find_last_memory_block:
	cmp	dword [ebx],0
	je	.allocate_more_memory
	mov	ebx,[ebx]
	jmp	.find_last_memory_block
    .allocate_more_memory:
	invoke	VirtualAlloc,0,10800h,MEM_COMMIT,PAGE_READWRITE
	or	eax,eax
	jz	.failed
	mov	[ebx],eax
	mov	[eax],dword 0
	mov	[eax+4],ebx
	lea	ebx,[eax+10800h]
	mov	[.unallocated_lines_end],ebx
	add	eax,108h
	lea	ebx,[eax+108h]
	mov	[.unallocated_lines],ebx
	clc
	ret
    .failed:
	stc
	ret
    .reuse_line:
	mov	eax,esi
	mov	[.memory_search_block],ebx
	add	esi,108h
	mov	[.memory_search_line],esi
	clc
	ret
    .find_in_next_block:
	sub	ebx,10800h
	mov	esi,[ebx]
	lea	ebx,[esi+10800h]
	or	esi,esi
	jnz	.find_free_line
	mov	ebx,[.editor_memory]
	mov	esi,ebx
	add	ebx,10800h
	jmp	.find_free_line
.init_editor_data:
	call	.allocate_line
	mov	[.first_line],eax
	mov	[.lines_count],1
	mov	[.caret_line],eax
	mov	[.caret_line_number],1
	mov	[.window_line],eax
	mov	[.window_line_number],1
	mov	edi,eax
	xor	eax,eax
	stosd
	stosd
	mov	eax,20202020h
	mov	ecx,100h shr 2
	rep	stosd
	mov	[.caret_position],0
	mov	[.window_position],0
	mov	[.sel_line],0
	mov	[.undo_data],0
	mov	[.search_text],0
	mov	[.current_operation],0
	mov	[.editor_mode],0
	mov	[.syntax_proc],SyntaxProc
	mov	[.syntax_colors],0
	mov	[.focus],0
	mov	[.mouse_select],0
	ret
.insert_character:
	mov	edx,[.caret_position]
	cmp	edx,100h
	je	.insert_done
	mov	esi,[.caret_line]
	lea	esi,[esi+8+edx]
	test	[.editor_mode],AEMODE_OVERWRITE
	jnz	.overwrite
	mov	ecx,100h
	sub	ecx,edx
	inc	edx
	mov	[.caret_position],edx
	mov	ah,al
	mov	ebx,esi
	mov	edx,ecx
    .shift_data_right:
	xchg	al,[esi]
	inc	esi
	loop	.shift_data_right
	test	[.editor_style],AES_AUTOBRACKETS
	jz	.insert_done
	cmp	ah,'('
	je	.round
	cmp	ah,'['
	je	.square
	cmp	ah,'{'
	je	.brace
    .insert_done:
	ret
    .round:
	mov	al,')'
	jmp	.auto_bracket
    .square:
	mov	al,']'
	jmp	.auto_bracket
    .brace:
	mov	al,'}'
    .auto_bracket:
	xor	ah,ah
	mov	ecx,edx
	dec	ecx
	jz	.insert_done
	lea	esi,[ebx+1]
	xchg	al,[esi]
	call	.recognize_character
	jnc	.cancel_bracket
	inc	esi
	dec	ecx
	jnz	.shift_data_right
	ret
    .cancel_bracket:
	mov	[esi],al
	ret
    .overwrite:
	mov	[esi],al
	inc	[.caret_position]
	ret
.delete_character:
	mov	edx,[.caret_position]
	cmp	edx,100h
	je	.delete_done
	mov	esi,[.caret_line]
	mov	al,20h
	test	[.editor_mode],AEMODE_OVERWRITE
	jnz	.delete
	add	esi,108h
	mov	ecx,100h
	sub	ecx,[.caret_position]
    .shift_data_left:
	dec	esi
	xchg	al,[esi]
	loop	.shift_data_left
    .delete_done:
	ret
    .delete:
	mov	[esi+8+edx],al
	ret
.recognize_character:
	cmp	al,20h
	jb	.neutral_character
	cmp	al,30h
	jb	.separator_character
	cmp	al,3Ah
	jb	.neutral_character
	cmp	al,40h
	jb	.separator_character
	cmp	al,5Bh
	jb	.neutral_character
	cmp	al,5Fh
	jb	.separator_character
	cmp	al,7Bh
	jb	.neutral_character
	cmp	al,7Fh
	jb	.separator_character
    .neutral_character:
	clc
	ret
    .separator_character:
	stc
	ret
.find_line:
	mov	esi,[.first_line]
	mov	ecx,1
	mov	edx,[.window_line_number]
	cmp	eax,edx
	jae	.forward_from_window
	sub	edx,eax
	cmp	edx,eax
	jb	.backward_from_window
	jmp	.find_forward
    .forward_from_window:
	mov	esi,[.window_line]
	mov	ecx,edx
    .find_forward:
	cmp	ecx,eax
	je	.line_found
	cmp	[esi],dword 0
	je	.line_found
	inc	ecx
	mov	esi,[esi]
	jmp	.find_forward
    .backward_from_window:
	mov	esi,[.window_line]
	mov	ecx,[.window_line_number]
    .find_backward:
	cmp	ecx,eax
	je	.line_found
	cmp	[esi+4],dword 0
	je	.line_found
	dec	ecx
	mov	esi,[esi+4]
	jmp	.find_backward
    .line_found:
	ret

.insert_block:
	test	[.editor_mode],AEMODE_VERTICALSEL
	jz	.do_insert_block
	push	esi
	or	edx,-1
	xor	ecx,ecx
    .count:
	lodsb
	cmp	al,9
	je	.cannot_insert
	cmp	al,0Dh
	je	.count_next_line
	or	al,al
	jz	.check_count
	inc	ecx
	jmp	.count
    .count_next_line:
	lodsb
    .check_count:
	cmp	edx,0
	jl	.line_to_insert_ok
	je	.cannot_insert
	cmp	edx,ecx
	je	.line_to_insert_ok
	jmp	.cannot_insert
    .line_to_insert_ok:
	mov	edx,ecx
	xor	ecx,ecx
	or	al,al
	jz	.cannot_insert
	cmp	byte [esi],0
	jne	.count
	pop	esi
    .do_insert_block:
	mov	eax,[.caret_line]
	mov	[.current_line],eax
	call	.store_line_for_undo
	mov	edx,[eax]
	mov	[.current_line],edx
	call	.store_line_for_undo
	mov	ebx,eax
	mov	[.sel_line],ebx
	mov	edx,[.caret_position]
	mov	eax,[.caret_line_number]
	mov	[.sel_line_number],eax
	lea	edi,[ebx+8+edx]
	test	[.editor_mode],AEMODE_OVERWRITE
	jnz	.begin_insert
	test	[.editor_mode],AEMODE_VERTICALSEL
	jnz	.begin_insert
	push	esi edi
	mov	esi,edi
	mov	edi,line_buffer
	mov	ecx,100h
	sub	ecx,[.caret_position]
	rep	movsb
	mov	ecx,[.caret_position]
	mov	al,20h
	rep	stosb
	mov	edi,[esp]
	mov	ecx,100h
	sub	ecx,[.caret_position]
	mov	al,20h
	rep	stosb
	pop	edi esi
    .begin_insert:
	mov	edx,esi
	xor	ecx,ecx
    .count_characters:
	lodsb
	cmp	al,0Dh
	je	.put_string
	cmp	al,9
	je	.convert_string
	or	al,al
	jz	.put_string
	inc	ecx
	jmp	.count_characters
    .convert_string:
	mov	esi,edx
	push	edi
	mov	edi,text_buffer
	xor	ecx,ecx
    .convert_tabs:
	lodsb
	cmp	al,0Dh
	je	.convert_done
	cmp	al,9
	je	.insert_tab
	or	al,al
	jz	.convert_done
	cmp	ecx,100h
	jae	.convert_tabs
	inc	ecx
	stosb
	jmp	.convert_tabs
    .insert_tab:
	mov	edx,ecx
	and	ecx,not 111b
	sub	ecx,edx
	add	ecx,8
	add	edx,ecx
	mov	al,20h
	rep	stosb
	mov	ecx,edx
	jmp	.convert_tabs
    .convert_done:
	mov	edx,text_buffer
	pop	edi
    .put_string:
	xchg	esi,edx
	lea	eax,[ebx+108h]
	sub	eax,edi
	cmp	eax,ecx
	jbe	.larger_string
	test	[.editor_mode],AEMODE_VERTICALSEL
	jz	.simple_put
	test	[.editor_mode],AEMODE_OVERWRITE
	jnz	.simple_put
	sub	eax,ecx
	xchg	eax,ecx
	push	esi edi
	lea	esi,[edi+ecx-1]
	lea	edi,[ebx+108h-1]
	std
	rep	movsb
	cld
	pop	edi esi
	xchg	eax,ecx
    .simple_put:
	rep	movsb
	jmp	.put_ok
    .larger_string:
	xchg	ecx,eax
	sub	eax,ecx
	rep	movsb
    .put_ok:
	mov	esi,edx
	cmp	byte [esi-1],0
	je	.last_line
	lodsb
	or	al,al
	jz	.last_line
	cmp	byte [esi],0
	je	.last_line
	test	[.editor_mode],AEMODE_VERTICALSEL
	jnz	.insert_next_line
	test	[.editor_mode],AEMODE_OVERWRITE
	jnz	.overwrite_line
    .insert_new_line:
	push	ebx esi
	call	.allocate_line
	jc	.out_of_memory
	pop	esi ebx
	mov	[.sel_line],eax
	inc	[.sel_line_number]
	mov	[.current_line],eax
	call	.store_line_for_undo
	mov	edi,eax
	mov	edx,[ebx]
	mov	[edi],edx
	mov	[edi+4],ebx
	mov	[ebx],edi
	mov	ebx,edi
	add	edi,8
	mov	eax,20202020h
	mov	ecx,100h shr 2
	rep	stosd
	inc	[.lines_count]
	or	edx,edx
	jz	.insert_into_line
	mov	[edx+4],ebx
	jmp	.insert_into_line
    .overwrite_line:
	lea	ecx,[ebx+8+100h]
	sub	ecx,edi
	mov	al,20h
	rep	stosb
    .insert_next_line:
	mov	eax,[ebx]
	or	eax,eax
	jz	.insert_new_line
	mov	ebx,eax
	mov	[.sel_line],ebx
	inc	[.sel_line_number]
	mov	[.current_line],eax
	call	.store_line_for_undo
    .insert_into_line:
	mov	edx,esi
	xor	ecx,ecx
	lea	edi,[ebx+8]
	test	[.editor_mode],AEMODE_VERTICALSEL
	jz	.count_characters
	add	edi,[.caret_position]
	jmp	.count_characters
    .last_line:
	test	[.editor_mode],AEMODE_OVERWRITE
	jnz	.block_inserted
	test	[.editor_mode],AEMODE_VERTICALSEL
	jnz	.block_inserted
	lea	eax,[ebx+108h]
	cmp	edi,eax
	jae	.block_inserted
	push	edi
	mov	ecx,eax
	sub	ecx,edi
	mov	esi,line_buffer
	rep	movsb
	pop	edi
    .block_inserted:
	lea	ebx,[ebx+8]
	mov	eax,edi
	sub	eax,ebx
	mov	[.sel_position],eax
	clc
	ret
    .cannot_insert:
	pop	esi
	stc
	ret
.delete_block:
	mov	eax,[.sel_line]
	mov	ebx,[.sel_position]
	mov	ecx,[.caret_line]
	mov	edx,[.caret_position]
	mov	esi,[.sel_line_number]
	test	[.editor_mode],AEMODE_VERTICALSEL
	jnz	.delete_vertical
	cmp	esi,[.caret_line_number]
	jb	.boundaries_for_delete_ok
	ja	.exchange_boundaries_for_delete
	cmp	ebx,edx
	jb	.boundaries_for_delete_ok
    .exchange_boundaries_for_delete:
	xchg	eax,ecx
	xchg	ebx,edx
	mov	esi,[.caret_line_number]
    .boundaries_for_delete_ok:
	mov	[.caret_line],eax
	mov	[.caret_line_number],esi
	mov	esi,eax
	mov	[.sel_line],0
	test	[.editor_mode],AEMODE_OVERWRITE
	jnz	.overwrite_delete
	mov	[.caret_position],ebx
	mov	[.current_line],eax
	call	.store_line_for_undo
	mov	edi,eax
	cmp	eax,ecx
	je	.lines_deleted
	mov	eax,[eax]
    .delete_line:
	mov	[.current_line],eax
	call	.store_line_for_undo
	mov	esi,eax
	cmp	eax,[.window_line]
	jne	.release_line
	mov	eax,[.caret_line]
	mov	[.window_line],eax
	mov	eax,[.caret_line_number]
	mov	[.window_line_number],eax
    .release_line:
	or	eax,-1
	xchg	[esi],eax
	dec	[.lines_count]
	cmp	esi,ecx
	jne	.delete_line
	mov	[edi],eax
	or	eax,eax
	jz	.lines_deleted
	mov	[.current_line],eax
	call	.store_line_for_undo
	mov	eax,[.current_line]
	mov	[eax+4],edi
    .lines_deleted:
	lea	edi,[edi+8+ebx]
	lea	esi,[esi+8+edx]
	mov	ecx,100h
	sub	ecx,ebx
	xor	eax,eax
	cmp	ebx,edx
	jae	.move_remaining_data
	mov	ecx,100h
	sub	ecx,edx
	mov	eax,edx
	sub	eax,ebx
    .move_remaining_data:
	rep	movsb
	mov	ecx,eax
	mov	al,20h
	rep	stosb
    .block_deleted:
	ret
    .overwrite_delete:
	push	ecx
	mov	ecx,100h
	sub	ecx,ebx
	lea	edi,[esi+ebx+8]
	pop	ebx
    .fill_with_spaces:
	cmp	esi,ebx
	jne	.end_fill_ok
	sub	ecx,100h
	add	ecx,edx
    .end_fill_ok:
	mov	al,20h
	rep	stosb
	cmp	esi,ebx
	je	.block_deleted
	mov	esi,[esi]
	mov	[.current_line],esi
	call	.store_line_for_undo
	mov	ecx,100h
	lea	edi,[esi+8]
	jmp	.fill_with_spaces
    .delete_vertical:
	cmp	esi,[.caret_line_number]
	jbe	.vboundaries_for_delete_ok
	xchg	eax,ecx
    .vboundaries_for_delete_ok:
	cmp	ebx,edx
	jbe	.hboundaries_for_delete_ok
	xchg	ebx,edx
    .hboundaries_for_delete_ok:
	mov	[.sel_line],0
	mov	esi,eax
	mov	edi,ecx
	mov	[.current_line],esi
	call	.store_line_for_undo
	test	[.editor_mode],AEMODE_OVERWRITE
	jnz	.vertical_fill
	mov	[.caret_position],ebx
    .cut_from_line:
	push	esi edi
	lea	edi,[esi+8+ebx]
	lea	esi,[esi+8+edx]
	mov	ecx,100h
	sub	ecx,edx
	rep	movsb
	mov	ecx,edx
	sub	ecx,ebx
	mov	al,20h
	rep	stosb
	pop	edi esi
	cmp	esi,edi
	je	.block_deleted
	mov	esi,[esi]
	mov	[.current_line],esi
	call	.store_line_for_undo
	jmp	.cut_from_line
    .vertical_fill:
	push	edi
	lea	edi,[esi+8+ebx]
	mov	ecx,edx
	sub	ecx,ebx
	mov	al,20h
	rep	stosb
	pop	edi
	cmp	esi,edi
	je	.block_deleted
	mov	esi,[esi]
	mov	[.current_line],esi
	call	.store_line_for_undo
	jmp	.vertical_fill
.copy_block:
	mov	esi,[.sel_line]
	mov	eax,[.caret_line]
	mov	ebx,[.sel_position]
	mov	edx,[.caret_position]
	mov	ecx,[.sel_line_number]
	test	[.editor_mode],AEMODE_VERTICALSEL
	jnz	.copy_vertical_block
	cmp	ecx,[.caret_line_number]
	jb	.boundaries_for_copy_ok
	ja	.exchange_boundaries_for_copy
	cmp	ebx,edx
	jb	.boundaries_for_copy_ok
    .exchange_boundaries_for_copy:
	xchg	esi,eax
	xchg	ebx,edx
    .boundaries_for_copy_ok:
	cmp	esi,eax
	je	.hboundaries_for_copy_ok
	push	esi edi
	lea	edi,[esi+108h-1]
	lea	esi,[esi+8+ebx]
	mov	ecx,100h
	sub	ecx,ebx
	mov	ebx,eax
	jmp	.get_line_size
    .copy_whole_line:
	mov	esi,[esi]
	cmp	esi,ebx
	je	.copy_last_line
	push	esi edi
	add	esi,8
	lea	edi,[esi+100h-1]
	mov	ecx,100h
    .get_line_size:
	mov	al,20h
	std
	repe	scasb
	setne	al
	movzx	eax,al
	add	ecx,eax
	cld
	pop	edi
	rep	movsb
	mov	word [edi],0A0Dh
	add	edi,2
	pop	esi
	jmp	.copy_whole_line
    .copy_last_line:
	add	esi,8
	mov	ecx,edx
	rep	movsb
	mov	ax,0A0Dh
	stosw
	xor	al,al
	stosb
	ret
    .copy_vertical_block:
	cmp	ecx,[.caret_line_number]
	jb	.vboundaries_for_copy_ok
	xchg	esi,eax
    .vboundaries_for_copy_ok:
	cmp	ebx,edx
	jb	.hboundaries_for_copy_ok
	xchg	ebx,edx
    .hboundaries_for_copy_ok:
	push	esi
	lea	esi,[esi+8+ebx]
	mov	ecx,edx
	sub	ecx,ebx
	rep	movsb
	mov	word [edi],0A0Dh
	add	edi,2
	pop	esi
	cmp	esi,eax
	je	.block_copied
	mov	esi,[esi]
	jmp	.hboundaries_for_copy_ok
    .block_copied:
	xor	al,al
	stosb
	ret
.get_block_size:
	xor	ecx,ecx
	mov	esi,[.sel_line]
	mov	eax,[.caret_line]
	mov	ebx,[.sel_position]
	mov	edx,[.caret_position]
	mov	edi,[.sel_line_number]
	test	[.editor_mode],AEMODE_VERTICALSEL
	jnz	.vertical_block
	cmp	edi,[.caret_line_number]
	jb	.boundaries_ok
	ja	.exchange_boundaries
	cmp	ebx,edx
	jb	.boundaries_ok
    .exchange_boundaries:
	xchg	esi,eax
	xchg	ebx,edx
    .boundaries_ok:
	xor	ecx,ecx
	cmp	esi,eax
	je	.hboundaries_ok
	add	edx,ecx
	mov	ecx,100h
	sub	ecx,ebx
	mov	ebx,eax
	lea	edi,[esi+8+100h-1]
	jmp	.count_line_bytes
    .whole_line:
	mov	esi,[esi]
	cmp	esi,ebx
	je	.block_size_ok
	lea	edi,[esi+8+100h-1]
	mov	ecx,100h
    .count_line_bytes:
	mov	al,20h
	std
	repe	scasb
	setne	al
	movzx	eax,al
	add	ecx,eax
	cld
	add	edx,ecx
	add	edx,2
	jmp	.whole_line
    .block_size_ok:
	mov	ecx,edx
	add	ecx,3
	ret
    .vertical_block:
	cmp	edi,[.caret_line_number]
	jb	.vboundaries_ok
	xchg	esi,eax
    .vboundaries_ok:
	cmp	ebx,edx
	jb	.hboundaries_ok
	xchg	ebx,edx
    .hboundaries_ok:
	add	ecx,edx
	sub	ecx,ebx
	add	ecx,2
	cmp	esi,eax
	je	.vertical_block_size_ok
	mov	esi,[esi]
	jmp	.hboundaries_ok
    .vertical_block_size_ok:
	inc	ecx
	ret
.find_text:
	push	edi edx
	add	edi,8
	mov	esi,[.search_text]
	test	byte [esi],AEFIND_BACKWARD
	jnz	.backward
	mov	ecx,100h
	sub	ecx,edx
	jz	.not_found
	add	edi,edx
	test	byte [esi],AEFIND_CASESENSITIVE
	jnz	.case_ok
	mov	esi,edi
	mov	edi,text_buffer
	push	ecx
	mov	ebx,case_table
    .convert_case:
	lodsb
	xlatb
	stosb
	loop	.convert_case
	pop	ecx
	mov	edi,text_buffer
	mov	esi,[.search_text]
    .case_ok:
	mov	edx,ecx
	cmp	ecx,[esi+4]
	jb	.not_found
    .search:
	mov	al,[esi+8]
	repne	scasb
	jne	.not_found
	mov	eax,[esi+4]
	dec	eax
	jz	.found
	cmp	ecx,eax
	jb	.not_found
	push	ecx esi edi
	mov	ecx,eax
	lea	esi,[esi+8+1]
	repe	cmpsb
	pop	edi esi ecx
	jne	.search
    .found:
	mov	eax,edx
	sub	eax,ecx
	dec	eax
	add	eax,[esp]
	test	byte [esi],AEFIND_WHOLEWORDS
	jz	.found_ok
	or	eax,eax
	jz	.left_bound_ok
	mov	ebx,[esp+4]
	push	eax
	mov	al,[ebx+8+eax-1]
	call	.recognize_character
	pop	eax
	jnc	.search
    .left_bound_ok:
	mov	ebx,[esi+4]
	add	ebx,eax
	cmp	ebx,100h
	je	.found_ok
	add	ebx,[esp+4]
	push	eax
	mov	al,[ebx+8]
	call	.recognize_character
	pop	eax
	jnc	.search
    .found_ok:
	pop	edx edi
	cld
	clc
	ret
    .backward:
	std
	or	edx,edx
	jz	.not_found
	mov	ecx,edx
	lea	edi,[edi+edx-1]
	mov	esi,[.search_text]
	test	byte [esi],AEFIND_CASESENSITIVE
	jnz	.backward_case_ok
	mov	esi,edi
	lea	edi,[text_buffer+ecx-1]
	push	ecx
	mov	ebx,case_table
    .convert_case_backward:
	lodsb
	xlatb
	stosb
	loop	.convert_case_backward
	pop	ecx
	lea	edi,[text_buffer+ecx-1]
	mov	esi,[.search_text]
    .backward_case_ok:
	cmp	ecx,[esi+4]
	jb	.not_found
    .backward_search:
	mov	eax,[esi+4]
	mov	al,[esi+8+eax-1]
	repne	scasb
	jne	.not_found
	mov	eax,[esi+4]
	dec	eax
	jz	.found_in_backward
	cmp	ecx,eax
	jb	.not_found
	push	ecx esi edi
	mov	ecx,eax
	lea	esi,[esi+8+eax-1]
	repe	cmpsb
	pop	edi esi ecx
	jne	.backward_search
    .found_in_backward:
	mov	eax,ecx
	inc	eax
	mov	ebx,eax
	sub	eax,[esi+4]
	test	byte [esi],AEFIND_WHOLEWORDS
	jz	.found_ok
	cmp	ebx,100h
	je	.right_bound_ok
	add	ebx,[esp+4]
	push	eax
	mov	al,[ebx+8]
	call	.recognize_character
	pop	eax
	jnc	.backward_search
    .right_bound_ok:
	or	eax,eax
	jz	.found_ok
	mov	ebx,[esp+4]
	push	eax
	mov	al,[ebx+8+eax-1]
	call	.recognize_character
	pop	eax
	jnc	.backward_search
	jmp	.found_ok
    .not_found:
	pop	edx edi
	cld
	stc
	ret

.store_status_for_undo:
	pusha
	cmp	[.was_selection],0
	jne	.selection_ok
	mov	[.sel_line],0
    .selection_ok:
	call	.allocate_line
	jc	.not_enough_memory
	mov	[eax],dword 0
	mov	edi,eax
	xchg	eax,[.undo_data]
	push	eax
	call	.allocate_line
	jnc	.store_editor_status
	or	eax,-1
	stosd
	jmp	.not_enough_memory
    .store_editor_status:
	mov	[eax],dword 0
	mov	[eax+4],dword 0
	stosd
	pop	eax
	stosd
	lea	esi,[.editor_data]
	mov	ecx,.editor_status_size shr 2
	rep	movsd
	popa
	clc
	ret
.store_line_for_undo:
	pusha
	cmp	[.current_line],0
	je	.line_for_undo_ok
	mov	esi,[.undo_data]
	or	esi,esi
	jz	.line_for_undo_ok
	mov	esi,[esi]
	mov	ecx,[esi+4]
	lea	edi,[esi+8+ecx*8]
	mov	dword [edi],0
	inc	ecx
	mov	[esi+4],ecx
	cmp	ecx,20h
	jb	.slot_ok
	push	esi
	call	.allocate_line
	jc	.out_of_memory
	mov	ebx,[.undo_data]
	mov	[ebx],eax
	pop	dword [eax]
	mov	[eax+4],dword 0
    .slot_ok:
	mov	esi,[.current_line]
	mov	eax,[esi]
	cmp	eax,-1
	jne	.store_line
	stosd
	mov	eax,esi
	stosd
	jmp	.line_for_undo_ok
    .store_line:
	call	.allocate_line
	jc	.out_of_memory
	mov	ebx,eax
	stosd
	mov	eax,[.current_line]
	stosd
	mov	esi,eax
	mov	edi,ebx
	mov	ecx,108h shr 2
	rep	movsd
    .line_for_undo_ok:
	popa
	ret
.undo:
	mov	esi,[.undo_data]
	or	esi,esi
	jz	.undo_ok
	or	ebx,-1
	xchg	ebx,[esi]
	add	esi,4
	lodsd
	mov	[.undo_data],eax
	lea	edi,[.editor_data]
	mov	ecx,.editor_status_size shr 2
	rep	movsd
    .lines_block:
	or	ebx,ebx
	jz	.undo_ok
	mov	esi,ebx
	or	ebx,-1
	xchg	ebx,[esi]
	add	esi,4
	lodsd
	mov	ecx,eax
	jecxz	.undo_ok
	lea	esi,[esi+ecx*8]
    .restore_lines:
	sub	esi,8
	push	esi ecx
	mov	edi,[esi+4]
	mov	esi,[esi]
	or	esi,esi
	jz	.restore_next
	cmp	esi,-1
	jne	.restore_data
	mov	eax,esi
	stosd
	jmp	.restore_next
    .restore_data:
	mov	ecx,108h shr 2
	rep	movsd
	or	dword [esi-108h],-1
    .restore_next:
	pop	ecx esi
	loop	.restore_lines
	jmp	.lines_block
    .undo_ok:
	ret
.clear_undo_data:
	mov	esi,[.undo_data]
	or	esi,esi
	jz	.undo_data_ok
	or	ebx,-1
	xchg	ebx,[esi]
	add	esi,4
	lodsd
	mov	[.undo_data],eax
    .release_lines_block:
	or	ebx,ebx
	jz	.clear_undo_data
	mov	esi,ebx
	or	ebx,-1
	xchg	ebx,[esi]
	add	esi,4
	lodsd
	mov	ecx,eax
	jecxz	.clear_undo_data
	lea	esi,[esi+ecx*8]
    .release_lines:
	sub	esi,8
	mov	eax,[esi]
	cmp	eax,-1
	je	.release_next
	or	dword [eax],-1
    .release_next:
	loop	.release_lines
	jmp	.release_lines_block
    .undo_data_ok:
	mov	[.last_operation],0
	ret

proc SyntaxProc, lpLine,lpColors
	enter
	return
