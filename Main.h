#pragma once

#include "resource.h"
#include "framework.h"
//------------------------------------------------------------------------------------------------------------
struct SSize
{
	SSize();
	SSize(unsigned short width, unsigned short height);

	unsigned short Width, Height;
};
//------------------------------------------------------------------------------------------------------------
struct SBuf_Color
{
	SSize Buffer_Size;
	unsigned int Color;
};
//------------------------------------------------------------------------------------------------------------
class AsFrame_DC
{
public:
	~AsFrame_DC();
	AsFrame_DC();
	HDC Get_DC(HWND hwnd, HDC hdc);
	char* Get_Buf();

	//int Width, Height;
	SSize Buf_Size;
	HBRUSH BG_Brush;
	HPEN White_Pen;

private:
	HBITMAP Bitmap;
	HDC DC;
	char* Bitmap_Buffer;
};
//------------------------------------------------------------------------------------------------------------
//extern "C" void Asm_Draw(char* video_buffer, SSize size); 
extern "C" void Asm_Draw_Line(char* video_buffer, SSize start_pos, SSize finish_pos, SBuf_Color buffer_color);
//------------------------------------------------------------------------------------------------------------