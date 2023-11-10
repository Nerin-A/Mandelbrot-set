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




//------------------------------------------------------------------------------------------------------------
#define MAX_LOADSTRING 100

// Global Variables:
float Global_Scale = 1.0f;

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
	int i;
	char* buf;
	SBuf_Color buffer_color;
	SPoint start_point(10, 0);
	SPoint finish_point(910, 100);
	//unsigned long long start_cpu_tick, end_cpu_tick, cpu_ticks;

	//SelectObject(frame_dc, DC.White_Pen);

	//start_cpu_tick = __rdtsc();

	//for (i = 0; i < DC.Buf_Size.Height - 100; i++)
	//{
	//	MoveToEx(frame_dc, 10, i, 0);
	//	LineTo(frame_dc, 910, i + 100);
	//}

	buf = DC.Get_Buf();
	buffer_color.Buffer_Size = DC.Buf_Size;
	buffer_color.Color = 0xffffffff;

	for (i = 0; i < DC.Buf_Size.Height - 100; i++)
	{
		start_point.Y = i;
		finish_point.Y = i + 100;
		Asm_Draw_Line(buf, start_point, finish_point, buffer_color);
	}

	//end_cpu_tick = __rdtsc();

	//cpu_ticks = end_cpu_tick - start_cpu_tick; // WinAPI ->>> 3 341 304 || 3 362 976 || 3 545 604 \\\ Asm function ->>> 1 340 640 || 1 299 636 || 1 299 096. So Asm function 3x time faster!
}
//------------------------------------------------------------------------------------------------------------
void Draw_Mandelbrot(HDC frame_dc)
{
	int i;
	int x, y;
	const int iterations_count = 100;
	float x_0, x_n, x_n1;
	float y_0, y_n, y_n1;
	float center_x = -1.0f;
	float center_y = -0.3f;
	float x_scale = (float)DC.Buf_Size.Width / (float)DC.Buf_Size.Height * Global_Scale;
	float distance;
	unsigned char color;
	//unsigned long long start_cpu_tick, end_cpu_tick, cpu_ticks;

	//start_cpu_tick = __rdtsc();

	for (y = 0; y < DC.Buf_Size.Height; ++y)
	{
		y_0 = (float)y / (float)DC.Buf_Size.Height - 0.5f; // y_0 = [-0.5f ... 0.5f)
		y_0 = y_0 * Global_Scale + center_y;

		for (x = 0; x < DC.Buf_Size.Width; x++)
		{
			x_0 = (float)x / (float)DC.Buf_Size.Width - 0.5f; // x_0 = [-0.5f ... 0.5f)
			x_0 = x_0 * x_scale + center_x;

			x_n = 0.0f;
			y_n = 0.0f;

			for (i = 0; i < iterations_count; i++)
			{
				x_n1 = x_n * x_n - y_n * y_n + x_0;
				y_n1 = 2.0f * x_n * y_n + y_0;

				distance = x_n1 * x_n1 + y_n1 * y_n1;

				if (distance > 4.0f)
					break;

				x_n = x_n1;
				y_n = y_n1;
			}

			if (i == iterations_count)
				color = 0;
			else
				color = i * 5 / 2;

			SetPixel(frame_dc, x, y, RGB(color, color, color));
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

	Global_Scale /= 1.1f;

	Draw_Mandelbrot(frame_dc);
	//InvalidateRect(hwnd, &ps.rcPaint, FALSE);

	BitBlt(hdc, 0, 0, DC.Buf_Size.Width, DC.Buf_Size.Height, frame_dc, 0, 0, SRCCOPY);

	EndPaint(hwnd, &ps);
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