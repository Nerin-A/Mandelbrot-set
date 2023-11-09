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
	push rbx
	push r10

	movzx r10, r9w ; R10 = R10W = buffer_color.Buffer_Size.Width

	mov rax, rdx
	shr rax, 16
	movzx rax, ax ; RAX = AX = start_pos.Y
	
	movzx rbx, dx ; RBX = BX = start_pos.X

	imul rax, r10 ; RAX = RAX * R10 = start_pos.Y * buffer_color.Buffer_Size.Width
	add rax, rbx ; RAX = index = start_pos.Y * buffer_color.Buffer_Size.Width + start_pos.X
	shl rax, 2 ; RAX = index * 4 = the pixel address in buffer

	mov rdi, r11
	add rdi, rax

	pop r10
	pop rbx
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

	; 1. Are the end point coordinates smaller than the start point coordinates?
	; 1.1 Checking X coord
	cmp r8w, dx
	jbe _exit

	; 1.2 Checking Y coord
	mov eax, edx
	shr eax, 16 ; EAX = AX = start_pos.Y

	mov ebx, r8d
	shr ebx, 16 ; EBX = BX = finish_pos.X

	cmp bx, ax
	jbe _exit



	call Get_Address ; RDI = address of the position start_pos in the buffer 


	mov rax, r9
	shr rax, 32 ; RAX = EAX = buffer_color.Color

	stosd

_exit:

	pop r11
	pop rdi
	pop rax

	ret

Asm_Draw_Line endp
;-------------------------------------------------------------------------------------------------------------
Asm_Draw_Horizontal_Line proc
; extern "C" void Asm_Draw_Horizontal_Line(char* video_buffer, SPoint start_pos,  unsigned short length, SBuf_Color buffer_color);
; Parameters
; RCX = char *video_buffer, 
; RDX = start_pos
; R8 = length
; R9 = buffer_color
; Return = void;

	push rax
	push rcx
	push rdi
	push r11

	mov r11, rcx

	; 1. Calculating the address of the beginning of the line
	call Get_Address ; RDI = address of the position start_pos in the buffer 

	; 2. Drawing the line
	mov rcx, r8 ; RCX = length = number of pixels in the line

	mov rax, r9
	shr rax, 32 ; RAX = EAX = buffer_color.Color

	rep stosd

	pop r11
	pop rdi
	pop rcx
	pop rax

	ret

Asm_Draw_Horizontal_Line endp
;-------------------------------------------------------------------------------------------------------------

end