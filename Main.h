#pragma once

#include "resource.h"
#include "framework.h"
//------------------------------------------------------------------------------------------------------------ 
struct SPoint
{
	SPoint();
	SPoint(unsigned short x, unsigned short y);

	unsigned short X, Y;
};
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
	void Create_Colorful_Palette();
	void Create_Web_Palette();
	void Draw_Colorful_Palette(HDC hdc);
	void Draw_Web_Palette(HDC hdc);
	void Draw_Monochrome_Palette(HDC hdc);

	//int Width, Height;
	SSize Buf_Size;
	HBRUSH BG_Brush;
	HPEN White_Pen;

	static const int Colors_Count = 100;

	int Palette_RGB[Colors_Count];
	int Palette_Web[96];

private:
	int Color_To_RGB(int color);

	HBITMAP Bitmap;
	HDC DC;
	char* Bitmap_Buffer;

	HPEN Palette_Pens[Colors_Count];
	HBRUSH Palette_Brushes[Colors_Count];
};
//------------------------------------------------------------------------------------------------------------
extern "C" void Asm_Draw(char* video_buffer, SSize size); 
extern "C" void Asm_Draw_Line(char* video_buffer, SPoint start_pos, SPoint finish_pos, SBuf_Color buffer_color);
extern "C" void Asm_Draw_Horizontal_Line(char* video_buffer, SPoint start_pos, unsigned short length, SBuf_Color buffer_color);
//------------------------------------------------------------------------------------------------------------