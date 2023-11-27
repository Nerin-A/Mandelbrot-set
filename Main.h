#pragma once

#include "resource.h"
#include "framework.h"
//------------------------------------------------------------------------------------------------------------ 
struct SRGB
{
	SRGB(unsigned char r, unsigned char g, unsigned char b)
		: R(r), G(g), B(b)
	{
	}

	unsigned char R, G, B;
};
//------------------------------------------------------------------------------------------------------------
struct SPoint
{
	SPoint();
	SPoint(unsigned short x, unsigned short y);

	unsigned short X, Y;
};
//------------------------------------------------------------------------------------------------------------
struct SPoint_Double
{
	SPoint_Double();
	SPoint_Double(double x, double y);

	double X, Y;
};
//------------------------------------------------------------------------------------------------------------
struct SPacked_X_Y
{
	double Y_0;
	double X0_0, X0_1; // SIMD packet
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
	void Create_Two_Colors_Palette(int start_index, const SRGB& color_1, const SRGB& color_2);
	void Draw_Colorful_Palette(HDC hdc);
	void Draw_Web_Palette(HDC hdc);
	void Draw_Multi_Color_Palette(HDC hdc);
	void Draw_Monochrome_Palette(HDC hdc);

	//int Width, Height;
	SSize Buf_Size;
	HBRUSH BG_Brush;
	HPEN White_Pen;

	static const int Colors_Count = 400;

	int Palette_RGB[Colors_Count];
	int Palette_Web[Colors_Count];

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
extern "C" void Asm_Set_Pixel(char* video_buffer, SPoint position, SBuf_Color buffer_color);
extern "C" int  Asm_Get_Mandelbrot_Index(char* video_buffer, double x_0, double y_0, int colors_count);
extern "C" int  Asm_Set_Mandelbrot_Point(char* video_buffer, SPoint_Double* x_y_0, int* palette_rgb, int colors_count);
extern "C" int  Asm_Set_Mandelbrot_2_Points(char* video_buffer, SPacked_X_Y* packed_x_y, int* palette_rgb, int colors_count);
//------------------------------------------------------------------------------------------------------------