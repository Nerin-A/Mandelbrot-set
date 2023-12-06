#include "Main.h"

// SPoint
//------------------------------------------------------------------------------------------------------------
SPoint::SPoint()
	: X(0), Y(0)
{
}
//------------------------------------------------------------------------------------------------------------
SPoint::SPoint(unsigned short x, unsigned short y)
	: X(x), Y(y)
{
}
//------------------------------------------------------------------------------------------------------------




// SPoint_Double
//------------------------------------------------------------------------------------------------------------
SPoint_Double::SPoint_Double()
	: X(0), Y(0)
{
}
//------------------------------------------------------------------------------------------------------------
SPoint_Double::SPoint_Double(double x, double y)
	: X(x), Y(y)
{
}
//------------------------------------------------------------------------------------------------------------




// SPacked_X_Y_4
//------------------------------------------------------------------------------------------------------------
SPacked_X_Y_4::SPacked_X_Y_4()
{
	int i;

	for (i = 0; i < 4; i++)
		FF[i] = 4.0;
}
//------------------------------------------------------------------------------------------------------------




// SSize
//------------------------------------------------------------------------------------------------------------
SSize::SSize()
	: Width(0), Height(0)
{
}
//------------------------------------------------------------------------------------------------------------
SSize::SSize(unsigned short width, unsigned short height)
	: Width(width), Height(height)
{
}
//------------------------------------------------------------------------------------------------------------




// AsFrame_DC
//------------------------------------------------------------------------------------------------------------
AsFrame_DC::~AsFrame_DC()
{
	if (Bitmap != 0)
		DeleteObject(Bitmap);

	if (DC != 0)
		DeleteObject(DC);
}
//------------------------------------------------------------------------------------------------------------
AsFrame_DC::AsFrame_DC()
	: DC(0), Bitmap(0), BG_Brush(0), White_Pen(0), Bitmap_Buffer(0)
{
	BG_Brush = CreateSolidBrush(RGB(0, 0, 0));
	White_Pen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255) );

	Create_Colorful_Palette();
	//Create_Web_Palette();
	//Create_Two_Colors_Palette(0, SRGB(255, 127, 63), SRGB(0, 127, 63));
	//Create_Two_Colors_Palette(Colors_Count / 2, SRGB(0, 127, 63), SRGB(128, 0, 255));
}
//------------------------------------------------------------------------------------------------------------
HDC AsFrame_DC::Get_DC(HWND hwnd, HDC hdc)
{
	int dc_width, dc_height;
	RECT rect;
	BITMAPINFO bitmap_info{};

	GetClientRect(hwnd, &rect);

	dc_width = rect.right - rect.left;
	dc_height = rect.bottom - rect.top;

	if (dc_width != Buf_Size.Width && dc_height != Buf_Size.Height)
	{
		if (Bitmap != 0)
			DeleteObject(Bitmap);

		if (DC != 0)
			DeleteObject(DC);

		Buf_Size.Width = dc_width;
		Buf_Size.Height = dc_height;

		DC = CreateCompatibleDC(hdc);
		//Bitmap = CreateCompatibleBitmap(hdc, Width, Height);

		bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFO);
		bitmap_info.bmiHeader.biWidth = Buf_Size.Width;
		bitmap_info.bmiHeader.biHeight = Buf_Size.Height;
		bitmap_info.bmiHeader.biPlanes = 1;
		bitmap_info.bmiHeader.biBitCount = 32;
		bitmap_info.bmiHeader.biCompression = BI_RGB;

		Bitmap = CreateDIBSection(hdc, &bitmap_info, DIB_RGB_COLORS, (void**)&Bitmap_Buffer, NULL, NULL);

		if (Bitmap != 0)
			SelectObject(DC, Bitmap);

		rect.right++;
		rect.bottom++;

		SelectObject(DC, BG_Brush);
		Rectangle(DC, rect.left, rect.top, rect.right, rect.bottom);	
	}

	return DC;
}
//------------------------------------------------------------------------------------------------------------
char* AsFrame_DC::Get_Buf()
{
	return Bitmap_Buffer;
}
//------------------------------------------------------------------------------------------------------------
void AsFrame_DC::Create_Colorful_Palette()
{
	int i;
	int rgb_color;
	int color_angle = 0;

	for (i = 0; i < Colors_Count; i++)
	{
		//color_angle = (int)((double)i / (double)Colors_Count * 360.0);
		color_angle += 13;

		if (color_angle >= 360)
			color_angle -= 360;

		rgb_color = Color_To_RGB(color_angle);

		Palette_RGB[i] = rgb_color;

		Palette_Pens[i] = CreatePen(PS_SOLID, 0, rgb_color);
		Palette_Brushes[i] = CreateSolidBrush(rgb_color);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsFrame_DC::Create_Web_Palette()
{
	int i, j;
	int position = 0;
	unsigned char r, g, b;
	unsigned int default_color = 0;

	for (i = 0; i < 12; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (j & 1)
				r = 21;
			else
				r = 0;

			if (j & 2)
				g = 21;
			else
				g = 0;

			if (j & 4)
				b = 21;
			else
				b = 0;

			Palette_Web[position++] = default_color + RGB(r, g, b);
		}

		default_color += 0x00151515;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsFrame_DC::Create_Two_Colors_Palette(int start_index, const SRGB &color_1, const SRGB &color_2)
{
	int i;
	int length = Colors_Count / 2;
	double current_r, current_g, current_b;
	double delta_r, delta_g, delta_b;
	SRGB current_color(0, 0, 0);

	delta_r = (double)(color_2.R - color_1.R) / (double)length;
	delta_g = (double)(color_2.G - color_1.G) / (double)length;
	delta_b = (double)(color_2.B - color_1.B) / (double)length;

	current_r = (double)color_1.R;
	current_g = (double)color_1.G;
	current_b = (double)color_1.B;

	for (i = 0; i < length; i++)
	{
		current_color.R = (unsigned char)current_r;
		current_color.G = (unsigned char)current_g;
		current_color.B = (unsigned char)current_b;

		current_r += delta_r;
		current_g += delta_g;
		current_b += delta_b;

		Palette_RGB[start_index + i] = RGB(current_color.R, current_color.G, current_color.B);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsFrame_DC::Draw_Colorful_Palette(HDC hdc)
{
	int i;
	double x_pos = 0.0;
	double bar_width = (double)Buf_Size.Width / (double)Colors_Count;

	for (i = 0; i < Colors_Count; i++)
	{
		SelectObject(hdc, Palette_Pens[i]);
		SelectObject(hdc, Palette_Brushes[i]);

		Rectangle(hdc, (int)x_pos, Buf_Size.Height / 2, (int)(x_pos + bar_width), Buf_Size.Height);

		x_pos += bar_width;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsFrame_DC::Draw_Web_Palette(HDC hdc)
{
	int i;
	int len = sizeof(Palette_Web) / sizeof(Palette_Web[0]);
	HPEN pen;
	HBRUSH brush;
	double x_pos = 0.0;
	double bar_width = (double)Buf_Size.Width / (double)len;

	for (i = 0; i < len; i++)
	{
		pen = CreatePen(PS_SOLID, 0, Palette_Web[i]);
		brush = CreateSolidBrush(Palette_Web[i]);

		SelectObject(hdc, pen);
		SelectObject(hdc, brush);

		Rectangle(hdc, (int)x_pos, Buf_Size.Height / 2, (int)(x_pos + bar_width), Buf_Size.Height);

		x_pos += bar_width;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsFrame_DC::Draw_Multi_Color_Palette(HDC hdc)
{
	int i;
	//unsigned char len = sizeof(Palette_RGB) / sizeof(Palette_RGB[0]);
	HPEN pen;
	HBRUSH brush;
	double x_pos = 0.0;
	double bar_width = (double)Buf_Size.Width / (double)Colors_Count;

	for (i = 0; i < Colors_Count; i++)
	{
		pen = CreatePen(PS_SOLID, 0, Palette_RGB[i]);
		brush = CreateSolidBrush(Palette_RGB[i]);

		SelectObject(hdc, pen);
		SelectObject(hdc, brush);

		Rectangle(hdc, (int)x_pos, Buf_Size.Height / 2, (int)(x_pos + bar_width), Buf_Size.Height);

		x_pos += bar_width;
	}

}
//------------------------------------------------------------------------------------------------------------
void AsFrame_DC::Draw_Monochrome_Palette(HDC hdc)
{
	int i;
	HPEN pen;
	HBRUSH brush;
	double x_pos = 0.0;
	double bar_width = (double)Buf_Size.Width / (double)Colors_Count;

	for (i = 0; i < Colors_Count; i++)
	{
		pen = CreatePen(PS_SOLID, 0, RGB(i, i, i));
		brush = CreateSolidBrush(RGB(i, i, i));

		SelectObject(hdc, pen);
		SelectObject(hdc, brush);

		Rectangle(hdc, (int)x_pos, 0, (int)(x_pos + bar_width), Buf_Size.Height / 2);

		x_pos += bar_width;
	}
}
//------------------------------------------------------------------------------------------------------------
int AsFrame_DC::Color_To_RGB(int color)
{// Parameters:
 // color = [0 .. 360) = H for HSV, S = 1.0, V = 1.0
 // Return: RGB

	unsigned char r, g, b;
	unsigned char v_inc, v_dec;
	int h;
	double a;

	h = color / 60; // h = [0 .. 5]

	a = double(color % 60) / 60.0; // a = [0 .. 1.0)

	v_inc = (unsigned char)(a * 255.0);
	v_dec = (unsigned char)((1.0 - a) * 255.0);

	switch (h)
	{
	case 0:
		r = 255;
		g = v_inc;
		b = 0;
		break;

	case 1:
		r = v_dec;
		g = 255;
		b = 0;
		break;

	case 2:
		r = 0;
		g = 255;
		b = v_inc;
		break;

	case 3:
		r = 0;
		g = v_dec;
		b = 255;
		break;

	case 4:
		r = v_inc;
		g = 0;
		b = 255;
		break;

	case 5:
		r = 255;
		g = 0;
		b = v_dec;
		break;

	default:
		throw 13;
	}

	return RGB(r, g, b);
}
//------------------------------------------------------------------------------------------------------------




//------------------------------------------------------------------------------------------------------------
#define MAX_LOADSTRING 100

// Global Variables:
//float Global_Scale = 0.001f;
double Global_Scale = 2.0;
//double Center_X = -0.99439399990001;
//double Center_Y = -0.3; 
double Center_X = -0.5;
double Center_Y = 0.0;

AsFrame_DC DC;
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
//------------------------------------------------------------------------------------------------------------
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
//------------------------------------------------------------------------------------------------------------
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MANDELBROTSET, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MANDELBROTSET));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}
//------------------------------------------------------------------------------------------------------------
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MANDELBROTSET));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = DC.BG_Brush;
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MANDELBROTSET);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}
//------------------------------------------------------------------------------------------------------------
BOOL InitInstance(HINSTANCE instance, int cmd_show)
{
	RECT window_rect;
	HWND hwnd;

	hInst = instance;

	window_rect.left = 0;
	window_rect.top = 0;
	window_rect.right = 320 * 3;
	window_rect.bottom = 200 * 3;

	AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW - WS_THICKFRAME, TRUE);

	hwnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW - WS_THICKFRAME, 0, 0, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, 0, 0, instance, 0);

	if (hwnd == 0)
		return FALSE;

	ShowWindow(hwnd, cmd_show);
	UpdateWindow(hwnd);

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------
void Clear_Screen(HDC frame_dc)
{
	int i;
	char* buf;
	SBuf_Color buffer_color;
	SPoint start_point(0, 0);
	SPoint finish_point(400, 500);
	unsigned long long start_cpu_tick, end_cpu_tick, cpu_ticks;

	buf = DC.Get_Buf();

	// Asm_Draw(buf, DC.Buf_Size);

	buffer_color.Buffer_Size = DC.Buf_Size;
	buffer_color.Color = 0xffffffff;

	start_cpu_tick = __rdtsc();

	for (i = 0; i < DC.Buf_Size.Height; i++)
	{
		start_point.Y = i;
		Asm_Draw_Horizontal_Line(buf, start_point, DC.Buf_Size.Width, buffer_color);
	}

	//SelectObject(frame_dc, DC.White_Pen);


	//for (i = 0; i < DC.Buf_Size.Height; i++)
	//{

	//	MoveToEx(frame_dc, 0, i, 0);
	//	LineTo(frame_dc, DC.Buf_Size.Width, i);
	//}

	end_cpu_tick = __rdtsc();

	cpu_ticks = end_cpu_tick - start_cpu_tick; // 4243752 || 4153716 || 4280760 on WinAPI AND 202068 || 127692 || 127224 using Asm function!
}
//------------------------------------------------------------------------------------------------------------
void Draw_Line(HDC frame_dc)
{
	//int i;
	//char* buf;
	//SBuf_Color buffer_color;
	//SPoint start_point(10, 0);
	//SPoint finish_point(910, 100);
	//unsigned long long start_cpu_tick, end_cpu_tick, cpu_ticks;

	//SelectObject(frame_dc, DC.White_Pen);

	//start_cpu_tick = __rdtsc();

	//for (i = 0; i < DC.Buf_Size.Height - 100; i++)
	//{
	//	MoveToEx(frame_dc, 10, i, 0);
	//	LineTo(frame_dc, 910, i + 100);
	//}

	//buf = DC.Get_Buf();
	//buffer_color.Buffer_Size = DC.Buf_Size;
	//buffer_color.Color = 0xffffffff;

	//for (i = 0; i < DC.Buf_Size.Height - 100; i++)
	//{
	//	start_point.Y = i;
	//	finish_point.Y = i + 100;
	//	Asm_Draw_Line(buf, start_point, finish_point, buffer_color);
	//}

	//end_cpu_tick = __rdtsc();

	//cpu_ticks = end_cpu_tick - start_cpu_tick; // WinAPI ->>> 3 341 304 || 3 362 976 || 3 545 604 \\\ Asm function ->>> 1 340 640 || 1 299 636 || 1 299 096. So Asm function 3x time faster!
}
//------------------------------------------------------------------------------------------------------------
void Draw_Mandelbrot(HDC frame_dc)
{
	int i;
	int x, y;
	double x_0, x_n, x_n1;
	double y_0, y_n, y_n1;
	double x_scale = (double)DC.Buf_Size.Width / (double)DC.Buf_Size.Height * Global_Scale;
	double distance;
	int color;

	//unsigned long long start_cpu_tick, end_cpu_tick, cpu_ticks;

	//start_cpu_tick = __rdtsc();

	for (y = 0; y < DC.Buf_Size.Height; ++y)
	{
		y_0 = (double)y / (double)DC.Buf_Size.Height - 0.5; // y_0 = [-0.5 ... 0.5)
		y_0 = y_0 * Global_Scale + Center_Y;

		for (x = 0; x < DC.Buf_Size.Width; x++)
		{
			x_0 = (double)x / (double)DC.Buf_Size.Width - 0.5; // x_0 = [-0.5 ... 0.5)
			x_0 = x_0 * x_scale + Center_X;

			x_n = 0.0;
			y_n = 0.0;

			for (i = 0; i < DC.Colors_Count; i++)
			{
				x_n1 = x_n * x_n - y_n * y_n + x_0;
				y_n1 = 2.0 * x_n * y_n + y_0;

				distance = x_n1 * x_n1 + y_n1 * y_n1;

				if (distance > 4.0)
					break;

				x_n = x_n1;
				y_n = y_n1;
			}

			if (i == DC.Colors_Count)
				color = 0;
			else
				color = DC.Palette_RGB[i];

			SetPixel(frame_dc, x, y, color);
		}
	}

	//end_cpu_tick = __rdtsc();

	//cpu_ticks = end_cpu_tick - start_cpu_tick; 
	// DEBUG WinAPI ->>> 3 264 438 708 || 3 271 177 618 || 3 219 223 174 || 3 208 083 697 - almost 1 sec on my 3.6Hhz CPU \\\ Asm function ->>> TODO
	// RELEASE WinAPI ->>> 2 668 194 145 || 2 663 908 377 || 2 700 979 201 \\ Asm ->>> TODO
	// so using WinAPI RELEASE mode is 1.22 times faster and using Asm RELEASE mode is TODO times faster

	SetPixel(frame_dc, DC.Buf_Size.Width / 2, DC.Buf_Size.Height / 2, RGB(255, 255, 255));
}
//------------------------------------------------------------------------------------------------------------
int Get_Mandelbrot_Index(double x_0, double y_0, int colors_count)
{
	int i;
	double x_n, x_n1;
	double y_n, y_n1;
	double distance;

	x_n = 0.0;
	y_n = 0.0;

	for (i = 0; i < colors_count; i++)
	{
		x_n1 = x_n * x_n - y_n * y_n + x_0;
		y_n1 = 2.0 * x_n * y_n + y_0;

		distance = x_n1 * x_n1 + y_n1 * y_n1;

		if (distance > 4.0)
			break;

		x_n = x_n1;
		y_n = y_n1;
	}

	return i;
}
//------------------------------------------------------------------------------------------------------------
// TODO - have to fix a bug - the function doesn't work in RELEASE mode
#pragma optimize("", off)
void Draw_Mandelbrot_Asm(HDC frame_dc)
{
	int i;
	int x, y;
	double x_scale = (double)DC.Buf_Size.Width / (double)DC.Buf_Size.Height * Global_Scale;
	//SPoint_Double x_y_0;
	//SPacked_X_Y packed_x_y;
	SPacked_X_Y_4 packed_x_y;
	SBuf_Color buffer_color;
	char* video_buffer;
	char* current_video_buffer;
	SPoint position;
	
	video_buffer = DC.Get_Buf();
	current_video_buffer = video_buffer;
	buffer_color.Buffer_Size = DC.Buf_Size;

	unsigned long long start_cpu_tick, end_cpu_tick, cpu_ticks;

	start_cpu_tick = __rdtsc();

	for (y = 0; y < DC.Buf_Size.Height; y+=2)
	{
		packed_x_y.Y_0[0] = (double)y / (double)DC.Buf_Size.Height - 0.5; // y_0 = [-0.5 ... 0.5)
		packed_x_y.Y_0[0] = packed_x_y.Y_0[0] * Global_Scale + Center_Y;

		for (i = 1; i < 4; i++)
			packed_x_y.Y_0[i] = packed_x_y.Y_0[0];

		for (x = 0; x < DC.Buf_Size.Width; x++)
		{
			for (i = 0; i < 4; i++)
			{
				packed_x_y.X_0[i] = (double)x / (double)DC.Buf_Size.Width - 0.5; // x_0 = [-0.5 ... 0.5)
				packed_x_y.X_0[i] = packed_x_y.X_0[i] * x_scale + Center_X;
			}

			//i = Get_Mandelbrot_Index(x_0, y_0,  DC.Colors_Count);

			//i = Asm_Get_Mandelbrot_Index(video_buffer, x_y_0.X, x_y_0.X, DC.Colors_Count);

			//if (i == DC.Colors_Count)
			//	color = 0;
			//else
			//	color = DC.Palette_RGB[i];

			//position.X = x;
			//position.Y = y;
			//buffer_color.Color = color;

			//Asm_Set_Pixel(video_buffer, position, buffer_color);

			//Asm_Set_Mandelbrot_Point(current_video_buffer, &x_y_0, DC.Palette_RGB, DC.Colors_Count);

			//Asm_Set_Mandelbrot_2_Points(current_video_buffer, &packed_x_y, DC.Palette_RGB, DC.Colors_Count);

			Asm_Set_Mandelbrot_4_Points(current_video_buffer, &packed_x_y, DC.Palette_RGB, DC.Colors_Count);

			current_video_buffer += 4 * 4;
		}
	}

	end_cpu_tick = __rdtsc();

	cpu_ticks = end_cpu_tick - start_cpu_tick; 
	// DEBUG WinAPI ->>> 3 264 438 708 || 3 271 177 618 || 3 219 223 174 || 3 208 083 697 - almost 1 sec on my 3.6Hhz CPU \\\ Asm function ->>> 1 384 816 392 || 1 380 186 107 || 1 384 397 391 || 1 381 998 887 || 1 376 514 180
	// RELEASE WinAPI ->>> 2 668 194 145 || 2 663 908 377 || 2 700 979 201 \\ Asm ->>> TODO
	// so using WinAPI RELEASE mode is 1.22 times faster compared to WinAPI DEBUG mode
	// and using Asm RELEASE mode is TODO times faster compared to Asm RELEASE mode
	// DEBUG Asm is 2.37 times faster than WinAPI DEBUG
	// RELEASE Asm is TODO times faster than WinAPI RELEASE

	// DEBUG Asm 562 901 795 || 561 991 854 || 562 981 681 
	// with new Asm_Set_Pixel function 575 260 092 || 569 660 502 || 571 467 170
	// 
	
	SetPixel(frame_dc, DC.Buf_Size.Width / 2, DC.Buf_Size.Height / 2, RGB(255, 255, 255));
}
#pragma optimize("", on)
//------------------------------------------------------------------------------------------------------------
void On_Paint(HWND hwnd)
{
	HDC hdc, frame_dc;
	PAINTSTRUCT ps;

	hdc = BeginPaint(hwnd, &ps);
	frame_dc = DC.Get_DC(hwnd, hdc);
	//Engine.Draw_Frame(frame_dc, ps.rcPaint);

	GdiFlush();

	//	Clear_Screen(frame_dc);
	// Draw_Line(frame_dc);

	//Global_Scale /= 2.0;
	//Global_Scale = 0.00000000000001;

	Draw_Mandelbrot_Asm(frame_dc);

	//Draw_Mandelbrot(frame_dc);

	//DC.Draw_Monochrome_Palette(frame_dc);
	//DC.Draw_Colorful_Palette(frame_dc);
	//DC.Draw_Multi_Color_Palette(frame_dc);
	//DC.Draw_Web_Palette(frame_dc);

	InvalidateRect(hwnd, &ps.rcPaint, FALSE);

	BitBlt(hdc, 0, 0, DC.Buf_Size.Width, DC.Buf_Size.Height, frame_dc, 0, 0, SRCCOPY);

	EndPaint(hwnd, &ps);
}
//------------------------------------------------------------------------------------------------------------
void Invalidate_Window_Rect(HWND hwnd)
{
	RECT window_rectangle;

	window_rectangle.left = 0;
	window_rectangle.top = 0;
	window_rectangle.right = DC.Buf_Size.Width;
	window_rectangle.bottom = DC.Buf_Size.Height;

	InvalidateRect(hwnd, &window_rectangle, FALSE);
}
//------------------------------------------------------------------------------------------------------------
void On_Mouse_Wheel(HWND hwnd, int wParam)
{
	int wheel_delta = wParam >> 16;

	if (wheel_delta > 0)
		Global_Scale /= 1.5;
	else
		Global_Scale *= 1.5;

	Invalidate_Window_Rect(hwnd);
}
//------------------------------------------------------------------------------------------------------------
void On_Mouse_Left_Key_Down(HWND hwnd, int lParam)
{
	int x_pos, y_pos;
	int window_center_x_pos = DC.Buf_Size.Width / 2;
	int window_center_y_pos = DC.Buf_Size.Height / 2;
	double center_x_offset;
	double center_y_offset;
	double x_ratio_correction = (double)DC.Buf_Size.Width / (double)DC.Buf_Size.Height;

	x_pos = lParam & 0xffff;
	y_pos = DC.Buf_Size.Height - (lParam >> 16) & 0xffff;

	center_x_offset = ((double)(x_pos - window_center_x_pos) / (double)DC.Buf_Size.Width) * x_ratio_correction;
	center_y_offset = (double)(y_pos - window_center_y_pos) / (double)DC.Buf_Size.Height;

	Center_X += center_x_offset * Global_Scale;
	Center_Y += center_y_offset * Global_Scale;

	Invalidate_Window_Rect(hwnd);
}
//------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);

		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;


		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;


		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;


	case WM_PAINT:
		On_Paint(hWnd);
		break;

	case WM_LBUTTONDOWN:
		if (wParam == MK_LBUTTON)
			On_Mouse_Left_Key_Down(hWnd, (unsigned int)lParam);
		break;	
	
	case WM_MOUSEWHEEL:
		On_Mouse_Wheel(hWnd, (int)wParam);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;


	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
//------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}

	return (INT_PTR)FALSE;
}
//------------------------------------------------------------------------------------------------------------