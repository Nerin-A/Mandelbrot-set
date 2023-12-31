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

	mov rcx, 100000

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
; XMM2 = [y_0] 
; XMM1 = [x_0]
; R9 = colors_count
; Return = EAX;

	push rcx

	mov rax, 4
	cvtsi2sd xmm8, rax ; XMM8 = 4.0 

	mov rcx, r9 ; RCX = colors_count = iterations count
;	x_n = 0.0;
;	y_n = 0.0;
	xorpd xmm3, xmm3 ; XMM3 = x_n = 0.0
	xorpd xmm4, xmm4 ; XMM4 = y_n = 0.0


_iteration_start:
;	for (i = 0; i < colors_count; i++)
;	{
;		x_n1 = x_n * x_n - y_n * y_n + x_0;
	movapd xmm5, xmm3 ; XMM5 = XMM3 = x_n
	movapd xmm6, xmm4 ; XMM6 = XMM4 = y_n

	mulsd xmm5, xmm5 ; XMM5 = x_n * x_n
	mulsd xmm6, xmm6 ; XMM6 = y_n * y_n

	subsd xmm5, xmm6 ; XMM5 = x_n * x_n - y_n * y_n

	addsd xmm5, xmm1 ; XMM5 = x_n * x_n - y_n * y_n + x_0 = x_n1

;		y_n1 = 2.0 * x_n * y_n + y_0;
	movaps xmm7, xmm3 ; XMM7 = x_n; 
	mulsd xmm7, xmm4 ; XMM7 = x_n * y_n
	addsd xmm7, xmm7 ; XMM7 = 2.0 * x_n * y_n
	addsd xmm7, xmm2 ; XMM7 = 2.0 * x_n * y_n + y_0 = y_n1
;
;		distance = x_n1 * x_n1 + y_n1 * y_n1;

	movaps xmm6, xmm5 ; XMM6 = x_n1
	mulsd xmm6, xmm6 ; XMM6 = x_n1 * x_n1	
	
	movaps xmm0, xmm7 ; XMM7 = y_n1
	mulsd xmm0, xmm0 ; XMM0 = y_n1 * y_n1

	addsd xmm0, xmm6 ; XMM0 = distance = x_n1 * x_n1 + y_n1 * y_n1;

;
;		if (distance > 4.0)
;			break;

	cmpnlesd xmm0, xmm8 ; XMM0 > 4.0 ?

	movmskpd eax, xmm0

	bt eax, 0
	jc _got_index

;		x_n = x_n1;
;		y_n = y_n1;
	movaps xmm3, xmm5 ; XMM3 = x_n = x_n1
	movaps xmm4, xmm7 ; XMM4 = y_n = y_n1;
	
;	}
	loop _iteration_start

_got_index:
	mov rax, r9
	sub rax, rcx ; RAX = EAX = colors_count - ciybt = color_index = iteration at which the loop was interrupted
;
;	return i;

	pop rcx
	
	ret

Asm_Get_Mandelbrot_Index endp
;-------------------------------------------------------------------------------------------------------------
Asm_Set_Mandelbrot_Point proc
; extern "C" int  Asm_Set_Mandelbrot_Point(char* video_buffer, SPoint_Double* x_y_0, int* palette_rgb, int colors_count);
; Parameters
; RAX = video_buffer
; RDX = x_y_0
; R8 = palette_rgb
; R9 = colors_count
; Return = EAX;

	push rbx
	push rcx
	push r10

	mov rax, 4
	cvtsi2sd xmm8, rax ; XMM8 = 4.0 

	mov r10, rcx ; R10 = video_buffer

	mov rcx, r9 ; RCX = colors_count = iterations count

	movupd xmm1, [ rdx ] ; XMM1 = x_0
	movupd xmm2, [ rdx + 8 ] ; XMM1 = y_0

;	x_n = 0.0;
;	y_n = 0.0;
	xorpd xmm3, xmm3 ; XMM3 = x_n = 0.0
	xorpd xmm4, xmm4 ; XMM4 = y_n = 0.0


_iteration_start:
;	for (i = 0; i < colors_count; i++)
;	{
;		x_n1 = x_n * x_n - y_n * y_n + x_0;
	movapd xmm5, xmm3 ; XMM5 = XMM3 = x_n
	movapd xmm6, xmm4 ; XMM6 = XMM4 = y_n

	mulsd xmm5, xmm5 ; XMM5 = x_n * x_n
	mulsd xmm6, xmm6 ; XMM6 = y_n * y_n

	subsd xmm5, xmm6 ; XMM5 = x_n * x_n - y_n * y_n

	addsd xmm5, xmm1 ; XMM5 = x_n * x_n - y_n * y_n + x_0 = x_n1

;		y_n1 = 2.0 * x_n * y_n + y_0;
	movaps xmm7, xmm3 ; XMM7 = x_n; 
	mulsd xmm7, xmm4 ; XMM7 = x_n * y_n
	addsd xmm7, xmm7 ; XMM7 = 2.0 * x_n * y_n
	addsd xmm7, xmm2 ; XMM7 = 2.0 * x_n * y_n + y_0 = y_n1
;
;		distance = x_n1 * x_n1 + y_n1 * y_n1;

	movaps xmm6, xmm5 ; XMM6 = x_n1
	mulsd xmm6, xmm6 ; XMM6 = x_n1 * x_n1	
	
	movaps xmm0, xmm7 ; XMM7 = y_n1
	mulsd xmm0, xmm0 ; XMM0 = y_n1 * y_n1

	addsd xmm0, xmm6 ; XMM0 = distance = x_n1 * x_n1 + y_n1 * y_n1;

;
;		if (distance > 4.0)
;			break;

	cmpnlesd xmm0, xmm8 ; XMM0 > 4.0 ?

	movmskpd eax, xmm0

	bt eax, 0
	jc _got_index

;		x_n = x_n1;
;		y_n = y_n1;
	movaps xmm3, xmm5 ; XMM3 = x_n = x_n1
	movaps xmm4, xmm7 ; XMM4 = y_n = y_n1;
	
;	}
	loop _iteration_start

_got_index:
	mov rbx, r9
	sub rbx, rcx ; RBX = EBX = colors_count - ciybt = color_index = iteration at which the loop was interrupted
;
;	return i;
	; if RCX == 0, selecting black color (0), otherwise - a color from the palette

	xor eax, eax ; EAX = 0 if color == black, also we don't have to compare it with 0 constant
	cmp ecx, eax

	cmovne eax, [ r8 + rbx * 4] ; EAX = palette_rgb[color_index]

	mov [ r10 ], eax ; Saving a pixel

	pop r10
	pop rcx
	pop rbx

	ret

Asm_Set_Mandelbrot_Point endp
;-------------------------------------------------------------------------------------------------------------
Asm_Set_Mandelbrot_2_Points proc
; extern "C" int  Asm_Set_Mandelbrot_2_Points(char* video_buffer, SPacked_X_Y* packed_x_y, int* palette_rgb, int colors_count);
; Parameters
; RAX = video_buffer
; RDX = packed_x_y
; R8 = palette_rgb./
; Return = EAX;

	push rbx
	push rcx
	push r10
	push r11
	push r12

	mov r11, 11b ; R11[1...0] - bitmask of values for which the index has already been calculated (0/1 - calculated/not yet calculated)

	mov rax, 4
	cvtsi2sd xmm8, rax ; XMM8 = 4.0 

	pshufd xmm8, xmm8, 01000100b ; XMM8 = { 4.0 & 4.0 }

	mov r10, rcx ; R10 = video_buffer

	mov rcx, r9 ; RCX = colors_count = iterations count

	movupd xmm2, [ rdx ] ; XMM1 = y_0
	pshufd xmm2, xmm2, 01000100b ; XMM2 = { y_0 & y_0 }

	movupd xmm1, [ rdx + 8 ] ; XMM1 = { x0_0 & x1_0 }

;	x_n = 0.0;
;	y_n = 0.0;
	xorpd xmm3, xmm3 ; XMM3 = { x1_n, x2_n } = { 0.0, 0.0 }
	xorpd xmm4, xmm4 ; XMM4 = { y1_n, y2_n } = { 0.0, 0.0 }


_iteration_start:
;	for (i = 0; i < colors_count; i++)
;	{
;		x_n1 = x_n * x_n - y_n * y_n + x_0;
	movapd xmm5, xmm3 ; XMM5 = XMM3 = { x1_n, x2_n }
	movapd xmm6, xmm4 ; XMM6 = XMM4 = { y1_n, y2_n }

	mulpd xmm5, xmm5 ; XMM5 = { x1_n * x1_n, x2_n * x2_n }
	mulpd xmm6, xmm6 ; XMM6 = { y1_n * y1_n, y2_n * y2_n }

	subpd xmm5, xmm6 ; XMM5 = { x1_n * x1_n - y1_n * y1_n, x2_n * x2_n - y2_n * y2_n }

	addpd xmm5, xmm1 ; XMM5 = { x1_n1, x2_n1 }

;		y_n1 = 2.0 * x_n * y_n + y_0;
	movaps xmm7, xmm3 ; XMM7 = { x1_n, x2_n } 
	mulpd xmm7, xmm4 ; XMM7 = { x1_n * y1_n, x2_n * y2_n }
	addpd xmm7, xmm7 ; XMM7 = { 2.0 * x1_n * y1_n, 2.0 * x2_n * y2_n }
	addpd xmm7, xmm2 ; XMM7 = { 2.0 * x1_n * y1_n + y1_0 = y1_n1, 2.0 * x2_n * y2_n + y2_0 = y2_n1 }
;
;		distance = x_n1 * x_n1 + y_n1 * y_n1;

	movaps xmm6, xmm5 ; XMM6 = { x1_n1,x2_n2 }
	mulpd xmm6, xmm6 ; XMM6 = { x1_n1 * x1_n1, x2_n1 * x2_n1 }	
	
	movaps xmm0, xmm7 ; XMM7 = { y1_n1, y2_n1 }
	mulpd xmm0, xmm0 ; XMM0 = { y1_n1 * y1_n1, y2_n1 * y2_n1 }

	addpd xmm0, xmm6 ; XMM0 = distance = { x1_n1 * x1_n1 + y1_n1 * y1_n1, x2_n1 * x2_n1 + y2_n1 * y2_n1 }

;		x_n = x_n1;
;		y_n = y_n1;
	movaps xmm3, xmm5 ; XMM3 = x_n = x_n1
	movaps xmm4, xmm7 ; XMM4 = y_n = y_n1;

;		if (distance > 4.0)
;			break;

	cmpnlepd xmm0, xmm8 ; XMM0 > 4.0 ?

	movmskpd eax, xmm0
	and rax,  r11  ;  We apply the mask, resetting to zero the bits for which the index has already been calculated

	cmp rax, 0
	jne _check_bits


	; bt eax, 0
	; jc _got_index
	
;	}

	loop _iteration_start ; There are no new indexes - we continue to iterate
	; We'll get here when all the iterations are over, in RAX the result of the last comparison
	; To this result we need to apply the result accumulated in the mask

	or rax, r11 ; We forcefully mark the remaining bits as those for which we need to calculate the index.

_check_bits:
	xor edx, edx ; Checking EAX starting with zero bit

_check_one_bit:
	bt eax, edx
	jnc _check_next_value ;  Going to the next bit if current ==  0

	mov r12, r9
	sub r12, rcx ; R12 = colors_count - ciybt = color_index = iteration at which the loop was interrupted

	; if RCX == 0, selecting black color (0), otherwise - a color from the palette

	xor ebx, ebx ; EBX = 0 if color == black, also we don't have to compare it with 0 constant
	cmp ecx, ebx

	cmovne ebx, [ r8 + r12 * 4] ; EBX = palette_rgb[color_index]

	mov [ r10 + rdx * 4], ebx ; Saving a pixel

	btr r11d, edx ; Let's reset the mask bit so we don't check this index anymore.

_check_next_value:
	inc edx
	cmp edx,  2
	jl _check_one_bit ; keep checking bits [1...0]

	cmp r11d, 0

	je _exit

	dec rcx

	cmp rcx, 0
	jg _iteration_start ; Not all indexes accounted

_exit:
	pop r12
	pop r11
	pop r10
	pop rcx
	pop rbx

	ret

Asm_Set_Mandelbrot_2_Points endp
;-------------------------------------------------------------------------------------------------------------
Asm_Set_Mandelbrot_4_Points proc
; extern "C" int  Asm_Set_Mandelbrot_4_Points(char* video_buffer, SPacked_X_Y_4 * packed_x_y, int* palette_rgb, int colors_count);
; Parameters
; RAX = video_buffer
; RDX = packed_x_y
; R8 = palette_rgb./
; Return = EAX;

	push rbx
	push rcx
	push r10
	push r11
	push r12

	mov r11, 1111b ; R11[3...0] - bitmask of values for which the index has already been calculated (0/1 - calculated/not yet calculated)

	; mov rax, 4
	; cvtsi2sd xmm8, rax ; XMM8 = 4.0 
	; 
	; pshufd xmm8, xmm8, 01000100b ; XMM8 = { 4.0 & 4.0 }

	vmovupd ymm8, [ rdx ] ; YMM8 = { 4.0 & 4.0 & 4.0 & 4.0 }

	mov r10, rcx ; R10 = video_buffer

	mov rcx, r9 ; RCX = colors_count = iterations count

	; movupd xmm2, [ rdx ] ; XMM1 = y_0
	; pshufd xmm2, xmm2, 01000100b ; XMM2 = { y_0 & y_0 }

	; movupd xmm1, [ rdx + 8 ] ; XMM1 = { x0_0 & x1_0 }
	vmovupd ymm1, [ rdx + 8 * 4 ] ; YMM1 = { x0_0 & x1_0 & x2_0 & x3_0 }
	vmovupd ymm2, [ rdx + 8 * 8 ] ; YMM2 = { y0_0 & y1_0 & y2_0 & y3_0 }

;	x_n = 0.0;
;	y_n = 0.0;
	vxorpd ymm3, ymm3, ymm3 ; YMM3 = { x1_n, x2_n, x3_n, x4_n } = { 0.0, 0.0, 0.0, 0.0 }
	vxorpd ymm4, ymm4, ymm4 ; YMM4 = { y1_n, y2_n, y3_n, y4_n } = { 0.0, 0.0, 0.0, 0.0 }


_iteration_start:
;	for (i = 0; i < colors_count; i++)
;	{
;		x_n1 = x_n * x_n - y_n * y_n + x_0;
	vmovapd ymm5, ymm3 ; YMM5 = YMM3 = { x1_n, x2_n, x1_n, x2_n }
	vmovapd ymm6, ymm4 ; YMM6 = YMM4 = { y1_n, y2_n, y1_n, y2_n }

	vmulpd ymm5, ymm3, ymm3 ; YMM5 = { x1_n * x1_n, x2_n * x2_n, x3_n * x3_n, x4_n * x4_n }
	vmulpd ymm6, ymm4, ymm4 ; YMM6 = { y1_n * y1_n, y2_n * y2_n, y3_n * y3_n, y4_n * y4_n }

	vsubpd ymm5, ymm5, ymm6 ; YMM5 = { x1_n * x1_n - y1_n * y1_n, x2_n * x2_n - y2_n * y2_n, x3_n * x3_n - y3_n * y3_n, x4_n * x4_n - y4_n * y4_n }

	vaddpd ymm5, ymm5, ymm1 ; YMM5 = { x1_n1, x2_n1, x3_n1, x4_n1 }

;		y_n1 = 2.0 * x_n * y_n + y_0;
	; movaps xmm7, xmm3 ; XMM7 = { x1_n, x2_n } 
	vmulpd ymm7, ymm4, ymm3 ; YMM7 = { x1_n * y1_n, x2_n * y2_n, x3_n * y3_n, x4_n * y4_n }
	vaddpd ymm7, ymm7, ymm7 ; YMM7 = { 2.0 * x1_n * y1_n, 2.0 * x2_n * y2_n, 2.0 * x3_n * y3_n, 2.0 * x4_n * y4_n }
	vaddpd ymm7,ymm7, ymm2 ; YMM7 = { y1_n1, y2_n1, y3_n1, y4_n1 }
;
;		distance = x_n1 * x_n1 + y_n1 * y_n1;

	;movaps xmm6, xmm5 ; XMM6 = { x1_n1,x2_n2 }
	vmulpd ymm6, ymm5, ymm5 ; YMM6 = { x1_n1 * x1_n1, x2_n1 * x2_n1, x3_n1 * x3_n1, x4_n1 * x4_n1 }	
	
	; movaps xmm0, xmm7 ; XMM7 = { y1_n1, y2_n1 }
	vmulpd ymm0, ymm7, ymm7 ; YMM0 = { y1_n1 * y1_n1, y2_n1 * y2_n1, y3_n1 * y3_n1, y4_n1 * y4_n1 }

	vaddpd ymm0, ymm0, ymm6 ; YMM0 = { distance_1... distance_4 }

;		x_n = x_n1;
;		y_n = y_n1;
	vmovaps ymm3, ymm5 ; YMM3 = x_n = x_n1
	vmovaps ymm4, ymm7 ; YMM4 = y_n = y_n1;

;		if (distance > 4.0)
;			break;

	vcmpnlepd ymm0,ymm0, ymm8 ; YMM0 > 4.0 ?

	vmovmskpd eax, ymm0
	and rax,  r11  ;  We apply the mask, resetting to zero the bits for which the index has already been calculated

	cmp rax, 0
	jne _check_bits

	loop _iteration_start ; There are no new indexes - we continue to iterate
	; We'll get here when all the iterations are over, in RAX the result of the last comparison
	; To this result we need to apply the result accumulated in the mask

	or rax, r11 ; We forcefully mark the remaining bits as those for which we need to calculate the index.

_check_bits:
	xor edx, edx ; Checking EAX starting with zero bit

_check_one_bit:
	bt eax, edx
	jnc _check_next_value ;  Going to the next bit if current ==  0

	mov r12, r9
	sub r12, rcx ; R12 = colors_count - ciybt = color_index = iteration at which the loop was interrupted

	; if RCX == 0, selecting black color (0), otherwise - a color from the palette

	xor ebx, ebx ; EBX = 0 if color == black, also we don't have to compare it with 0 constant
	cmp ecx, ebx

	cmovne ebx, [ r8 + r12 * 4] ; EBX = palette_rgb[color_index]

	mov [ r10 + rdx * 4], ebx ; Saving a pixel

	btr r11d, edx ; Let's reset the mask bit so we don't check this index anymore.

_check_next_value:
	inc edx
	cmp edx,  4
	jl _check_one_bit ; keep checking bits [3...0]

	cmp r11d, 0

	je _exit

	dec rcx

	cmp rcx, 0
	jg _iteration_start ; Not all indexes accounted

_exit:
	pop r12
	pop r11
	pop r10
	pop rcx
	pop rbx

	ret

Asm_Set_Mandelbrot_4_Points endp
;-------------------------------------------------------------------------------------------------------------
end
