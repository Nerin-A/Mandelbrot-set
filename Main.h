#pragma once

#include "resource.h"
#include "framework.h"
//------------------------------------------------------------------------------------------------------------
struct SSize
{
	unsigned short Width = 0, Height = 0;
};
//------------------------------------------------------------------------------------------------------------
class AsFrame_DC
{
public:
	~AsFrame_DC();
	AsFrame_DC();
	HDC Get_DC(HWND hwnd, HDC hdc);

	int Width, Height;
	HBRUSH BG_Brush;
	HPEN White_Pen;

private:
	HBITMAP Bitmap;
	HDC DC;
	char* Bitmap_Buffer;
};
//------------------------------------------------------------------------------------------------------------
extern "C" void Asm_Draw(char *video_buffer, SSize size);