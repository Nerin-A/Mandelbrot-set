﻿#pragma once

#include "resource.h"
#include "framework.h"

//------------------------------------------------------------------------------------------------------------
class AsFrame_DC
{
public:
	~AsFrame_DC();
	AsFrame_DC();
	HDC Get_DC(HWND hwnd, HDC hdc);

	int Width, Height;

	static HBRUSH BG_Brush;

private:
	HBITMAP Bitmap;
	HDC DC;
};
//------------------------------------------------------------------------------------------------------------