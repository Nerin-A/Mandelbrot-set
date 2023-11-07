.code

;-------------------------------------------------------------------------------------------------------------
Asm_Draw proc
; extern "C" void Asm_Draw(char *video_buffer, SSize size);
; Parameters
; RCX = char *video_buffer, 
; RDX = SSize size
; Return = void;

	mov rdi, rcx
	mov eax, 0ffffffffh

	mov rcx, 100

	rep stosd 

	ret

Asm_Draw endp
;-------------------------------------------------------------------------------------------------------------
Asm_Draw_Line proc
; extern "C" void Asm_Draw_Line(char* video_buffer, SPoint start_pos, SPoint finish_pos, SBuf_Color buffer_color);
; Parameters
; RCX = char *video_buffer, 
; RDX = start_pos
; R8 = finish_pos
; R9 = buffer_color
; Return = void;

	mov rdi, rcx
	mov eax, 0ffffffffh
	
	mov rcx, 100
	
	rep stosd 

	ret

Asm_Draw_Line endp
;-------------------------------------------------------------------------------------------------------------

end