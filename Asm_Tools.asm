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

	push rax
	push rdx
	push rdi
	push r10

	mov rdi, rcx

	movzx r10, r9w ; R10 = R10W = buffer_color.Buffer_Size.Width

	mov rax, rdx
	shr rax, 16
	movzx rax, ax ; RAX = AX = start_pos.Y
	
	movzx rbx, dx ; RBX = BX = start_pos.X

	imul rax, r10 ; RAX = RAX * R10 = start_pos.Y * buffer_color.Buffer_Size.Width
	add rax, rbx ; RAX = index = start_pos.Y * buffer_color.Buffer_Size.Width + start_pos.X
	shl rax, 2 ; RAX = index * 4 = the pixel address in buffer


	add rdi, rax
	mov rax, r9
	shr rax, 32 ; RAX = EAX = buffer_color.Color

	stosd

	pop r10
	pop rdi
	pop rdx
	pop rax

	ret

Asm_Draw_Line endp
;-------------------------------------------------------------------------------------------------------------

end