#include <os_win32.h>

//TODO: document this
LRESULT CALLBACK WndProc(HWND win, UINT msg, WPARAM wParam, LPARAM lParam)
{
	os_win32_context *ctx = (os_win32_context*)GetWindowLongPtr(win, GWLP_USERDATA);
	return ctx->win32_handler(win, msg, wParam, lParam);
}

void os_win32_context::startup()
{
	printf("Win32 Init\n");
	instance = GetModuleHandle(NULL);
	cmd_show = SW_SHOWNORMAL;
	w32_class.cbSize = sizeof(WNDCLASSEX);
	w32_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW; //NOTE: should modify
	w32_class.lpfnWndProc = &WndProc;
	w32_class.cbClsExtra = 0;
	w32_class.cbWndExtra = 0;
	w32_class.hInstance = instance;
	w32_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	w32_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	w32_class.hbrBackground = 0;//(HBRUSH)(COLOR_WINDOW+1);
	w32_class.lpszMenuName = NULL;
	w32_class.lpszClassName = "BD";
	w32_class.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	printf("Win32 Init Done\n");

	create_win32_window();
}

void os_win32_context::window_loop_start()
{
	printf("Starting WIN32 Window Loop\n");
	MSG msg;
	
	while (true) //NOTE(Ethan): This is super temorary
	{
		if (GetMessage(&msg, 0, 0, 0) > 0)
		{
			if (msg.message == WM_QUIT)
			{
				std::exit(1);
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void os_win32_context::window_update()
{
	HDC dc = GetDC(win);
	update_win32_window(dc);
	ReleaseDC(win, dc);
}

void os_win32_context::window_test_draw()
{
	int pitch = width * 4;
	uint8_t* row = (uint8_t*)bitmap_buffer;

	for (int y = 0; y < height; y++)
	{
		uint8_t* pixel = (uint8_t*)row;
		for (int x = 0; x < width; x++)
		{
			*pixel = sin(((float)x) / 150) * 255;
			++pixel;

			*pixel = cos(((float)x) / 10) * 100;
			++pixel;

			*pixel = cos(((float)y) / 50) * 255;
			++pixel;

			*pixel = 0;
			++pixel;
		}
		row += pitch;
	}
}

void os_win32_context::create_win32_window()
{
	printf("Creating WIN32 Window\n"); //@LOG

	if (!RegisterClassEx(&w32_class))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	printf("Created WIN32 Class thing\n"); //@LOG


	win = CreateWindowEx(
		0,//WS_BORDER,
		APP_NAME,
		APP_NAME,
		/* WS_OVERLAPPEDWINDOW, */
		WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,//(WS_POPUP | WS_BORDER | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX), //WS_BORDER
		CW_USEDEFAULT, CW_USEDEFAULT, 900, 600, //TODO: make this configurable
		NULL, NULL, instance, this);

	if (win == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return;
	}

	ShowWindow(win, cmd_show);
	UpdateWindow(win);

}

void os_win32_context::create_bitmap()
{
	if (bitmap_buffer)
		VirtualFree(bitmap_buffer, 0, MEM_RELEASE);

	bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
	bitmap_info.bmiHeader.biWidth = width;
	bitmap_info.bmiHeader.biHeight = -height;
	bitmap_info.bmiHeader.biPlanes = 1;
	bitmap_info.bmiHeader.biBitCount = 32; //24 bits but with 8 bits of padding
	bitmap_info.bmiHeader.biCompression = BI_RGB;
	bitmap_info.bmiHeader.biSizeImage = 0;
	bitmap_info.bmiHeader.biXPelsPerMeter = 0;
	bitmap_info.bmiHeader.biYPelsPerMeter = 0;
	bitmap_info.bmiHeader.biClrUsed = 0;
	bitmap_info.bmiHeader.biClrImportant = 0;

	int bytes_per_pixel = 4;
	int bitmap_memory_size = (width*height)*bytes_per_pixel;
	bitmap_buffer = VirtualAlloc(0, bitmap_memory_size, MEM_COMMIT, PAGE_READWRITE);

}

void os_win32_context::update_win32_window(HDC device_context)
{

	//TODO: Replace with BitBlt this is way too slow... (we don't even need the scaling)
	StretchDIBits(device_context,
		0, 0, width, height,
		0, 0, width, height,

		bitmap_buffer,
		&bitmap_info,
		DIB_RGB_COLORS, SRCCOPY);
}

LRESULT os_win32_context::win32_handler(HWND win, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			std::exit(1);
			break;
		default:
			break;
		}
		break;
	case WM_SIZE:
	{
		std::cout << "you shouldn't be able to do this lol." << std::endl;
		RECT drawable_rect;
		GetClientRect(win, &drawable_rect);

		height = drawable_rect.bottom - drawable_rect.top;
		width = drawable_rect.right - drawable_rect.left;
		create_bitmap();

		window_test_draw();
	} break;
	case WM_NCCREATE:
	{
		SetWindowLongPtr(win, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);
		SetWindowPos(win, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER); //prevents data from being cached
	} break;
	case WM_CLOSE:
		std::exit(1);
		break;
	case WM_DESTROY:
		break;
	case WM_ACTIVATEAPP:

		break;
	case WM_PAINT:
	{
		PAINTSTRUCT paint;
		HDC device_context = BeginPaint(win, &paint);
		EndPaint(win, &paint);

		RECT drawable_rect;
		GetClientRect(win, &drawable_rect);

		int height = drawable_rect.bottom - drawable_rect.top;
		int width = drawable_rect.right - drawable_rect.left;

		GetClientRect(win, &drawable_rect);
		update_win32_window(device_context);

	} break;
	default:
		return DefWindowProc(win, msg, wParam, lParam);
	}
	return 0;
}



void* os_win32_context::get_window_bitmap()
{
	return bitmap_buffer;
}

uint32_t os_win32_context::get_window_width()
{
	return width;
}

uint32_t os_win32_context::get_window_height()
{
	return height;
}



//NOTE(Ethan): I just copy-pasted this from the raytracer so I'm going to need to document this later
typedef struct
{
	void* data;
	void(*func)(void*);
} thread_func_meta;

DWORD WINAPI thread_func(void* data)
{
	if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST))
	{
		DWORD dwError;
		dwError = GetLastError();
		printf(TEXT("Failed to change thread priority (%d)\n"), dwError);
	}

	thread_func_meta* meta = (thread_func_meta*)data;
	(meta->func)(meta->data); //confusing syntax: call the passed function with the passed data
	free(meta);
	return 0;
}

void os_win32_context::start_thread(void(*func)(void*), void* data)
{
	thread_func_meta* meta = (thread_func_meta*)malloc(sizeof(thread_func_meta));
	meta->data = data;
	meta->func = func;
	HANDLE t = CreateThread(NULL, 0, thread_func, meta, 0, NULL);
	//if(SetThreadPriority(t, THREAD_PRIORITY_HIGHEST)==0)
	//    assert(false);

}