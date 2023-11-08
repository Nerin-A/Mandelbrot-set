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
Get_Address proc
; Getting address in memory, corresponding to the coordinates X, Y
; Parameters
; RDX = start_pos
; R9 = buffer_color
; R11 = char *video_buffer, 
; Return = RDI = Address of the pixel
	push rax
	push rdx
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

	pop r10
	pop rdx
	pop rax

	ret

Get_Address endp
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
	push rdi
	push r11

	mov r11, rcx

	call Get_Address ; RDI = address of the position start_pos in the buffer 


	mov rax, r9
	shr rax, 32 ; RAX = EAX = buffer_color.Color

	stosd

	pop r11
	pop rdi
	pop rax

	ret

Asm_Draw_Line endp
;-------------------------------------------------------------------------------------------------------------
Draw_Horizontal_Line proc
; extern "C" void Draw_Horizontal_Line(char* video_buffer, SPoint start_pos,  unsigned short length, SBuf_Color buffer_color);
; Parameters
; RCX = char *video_buffer, 
; RDX = start_pos
; R8 = length
; R9 = buffer_color
; Return = void;

	push rax
	push rdi
	push r11

	mov r11, rcx

	call Get_Address ; RDI = address of the position start_pos in the buffer 


	mov rax, r9
	shr rax, 32 ; RAX = EAX = buffer_color.Color

	stosd

	pop r11
	pop rdi
	pop rax

	ret

Draw_Horizontal_Line endp
;-------------------------------------------------------------------------------------------------------------

end