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
	push rbx
	push rcx
	push rdx
	push rdi
	push r11
	push r12
	push r13
	push r14
	push r15

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

	; 2. Calculating address of the beginning of the line
	call Get_Address ; RDI = address of the position start_pos in the buffer 

	mov ax, r8w ; AX = finish_pos.X
	sub ax, dx ; AX = AX - DX = finish_pos.X - start_pos.X = delta_X
	inc ax
	mov r12w, ax ; R12W = delta_X

	mov ebx, r8d
	shr ebx, 16 ; EBX = BX = finish_pos.Y

	mov ecx, edx
	shr ecx, 16 ; ECX = CX = start_pos.Y

	sub bx, cx ; BX = finish_pos.Y - start_pos.Y = delta_Y
	inc bx
	mov r13w, bx ; R12W = delta_X

	; 3. Chosing the algorhytm - are the line tilds toward vertical horizontal shape?
	cmp ax, bx
	jle _draw_vertical_line


; _draw_horizontal_line:
	; Going to the next line IF overflow

	xor r14w, r14w ; Numerator accumulator

	movzx rcx, r13w ; RCX = delta_Y = number of iterations

	movzx r15, r9w ; R15 = buffer_color.Buffer_Size.Width in pixels
	shl r15, 2 ; buffer width in bytes

	mov rax, r9
	shr rax, 32 ; RAX = EAX = buffer_color.Color

_draw_horizontal_pixel:
	stosd 

	add r14w, r13w ; We add the numerator to the numerator accumulator until there is an overflow there. We form a horizontal chain of pixels.
	cmp r14w, r12w ; Has the numerator exceeded the denominator?
	jl _draw_horizontal_pixel 

	sub r14w, r12w ; Subtract the denominator from the numerator accumulator.

	add rdi, r15
	loop _draw_horizontal_pixel

	jmp _exit



_draw_vertical_line:

	xor r14w, r14w ; Numerator accumulator

	movzx rcx, r12w ; RCX = delta_X = number of iterations

	movzx r15, r9w ; R15 = buffer_color.Buffer_Size.Width in pixels
	dec r15
	shl r15, 2 ; buffer width in bytes

	mov rax, r9
	shr rax, 32 ; RAX = EAX = buffer_color.Color

_draw_vertical_pixel:
	stosd 
	add rdi, r15

	add r14w, r12w ; We add the numerator to the numerator accumulator until there is an overflow there. We form a horizontal chain of pixels.
	cmp r14w, r13w ; Has the numerator exceeded the denominator?
	jl _draw_vertical_pixel 

	sub r14w, r13w ; Subtract the denominator from the numerator accumulator.

	add rdi, 4
	loop _draw_vertical_pixel


_exit:

	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop rdi
	pop rdx
	pop rcx
	pop rbx
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
Asm_Set_Pixel proc
; extern "C" void Asm_Set_Pixel(char* video_buffer, SPoint position, SBuf_Color buffer_color);
; Parameters
; RCX = char *video_buffer, 
; RDX = position
; R8 = buffer_color
; Return = void;

	push rdi
	push r9
	push r11

	mov r9, r8 ; R9 = buffer_color
	mov r11, rcx ; R11 = video_buffer

	; 1. Calculating the address of the pixel
	call Get_Address ; RDI = address of the position variable in the buffer 

	shr r9, 32 ; R9 = R9D = buffer_color.Color
	mov [ rdi ], r9d

	pop r11
	pop r9
	pop rdi

	ret

Asm_Set_Pixel endp
;-------------------------------------------------------------------------------------------------------------
Asm_Get_Mandelbrot_Index proc
; extern "C" int  Asm_Get_Mandelbrot_Index(char* video_buffer, double x_0, double y_0, int colors_count);
; Parameters
; RAX = video_buffer
; XMM1 = [y_0] 
; XMM0 = [x_0]
; R8 = colors_count
; Return = EAX;

	push rcx

	mov rax, 4
	cvtsi2sd xmm8, rax ; XMM8 = 4.0 

	mov rcx, r8 ; RCX = colors_count = iterations count
;	x_n = 0.0;
;	y_n = 0.0;
	xorpd xmm3, xmm3 ; XMM3 = x_n = 0.0
	xorpd xmm4, xmm4 ; XMM4 = y_n = 0.0


_iteration_start:
;	for (i = 0; i < colors_count; i++)
;	{
;		x_n1 = x_n * x_n - y_n * y_n + x_0;
	movaps xmm5, xmm3 ; XMM5 = XMM3 = x_n
	movaps xmm6, xmm4 ; XMM6 = XMM4 = y_n

	mulsd xmm5, xmm5 ; XMM5 = x_n * x_n
	mulsd xmm6, xmm6 ; XMM6 = y_n * y_n

	subsd xmm5, xmm6 ; XMM5 = x_n * x_n - y_n * y_n

	addsd xmm5, xmm0 ; XMM5 = x_n * x_n - y_n * y_n + x_0 = x_n1

;		y_n1 = 2.0 * x_n * y_n + y_0;
	movaps xmm7, xmm3 ; XMM7 = x_n; 
	mulsd xmm7, xmm4 ; XMM7 = x_n * y_n
	addsd xmm7, xmm7 ; XMM7 = 2.0 * x_n * y_n
	addsd xmm7, xmm1 ; XMM7 = 2.0 * x_n * y_n + y_0 = y_n1
;
;		distance = x_n1 * x_n1 + y_n1 * y_n1;

	movaps xmm6, xmm5 ; XMM6 = x_n1
	mulsd xmm6, xmm6 ; XMM6 = x_n1 * x_n1	
	
	movaps xmm2, xmm7 ; XMM2 = XMM7 = y_n1
	mulsd xmm2, xmm2 ; XMM2 = y_n1 * y_n1

	addsd xmm2, xmm6 ; XMM2 = distance = x_n1 * x_n1 + y_n1 * y_n1;

;
;		if (distance > 4.0)
;			break;

	cmpnlesd xmm2, xmm8 ; XMM2 > 4.0 ?

	movmskpd eax, xmm2

	bt eax, 0
	jc _got_index

;		x_n = x_n1;
;		y_n = y_n1;
	movaps xmm3, xmm5 ; XMM3 = x_n = x_n1
	movaps xmm4, xmm7 ; XMM4 = y_n = y_n1;
	
;	}
	loop _iteration_start

_got_index:
	mov rax, r8
	sub rax, rcx ; RAX = EAX = colors_count - ciybt = color_index = iteration at which the loop was interrupted
;
;	return i;

	pop rcx
	
	ret

Asm_Get_Mandelbrot_Index endp
;-------------------------------------------------------------------------------------------------------------
end