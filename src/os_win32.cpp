#include <os_win32.h>
#include <log.h>
#include <GL/gl3w.h>


#define LOG_TAG "WIN32"

#define EVENT_OGL_CREATE_CONTEXT 1

#ifdef _WIN32


//This function is a wrapper over the handler function.
//All it does is call the handler function with the proper context
LRESULT CALLBACK WndProc(HWND win, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //Retrieve the Window Context
	os_win32_context *ctx = (os_win32_context*)GetWindowLongPtr(win, GWLP_USERDATA);

	return ctx->win32_handler(win, msg, wParam, lParam);
}

//Win32 Class initialisation function.
void os_win32_context::startup()
{
    Log::msg(LOG_TAG, "Win32 Init Starting.");
	instance = GetModuleHandle(NULL);
	cmd_show = SW_SHOWNORMAL;
	w32_class.cbSize = sizeof(WNDCLASSEX);
	w32_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW; //NOTE: should modify
	w32_class.lpfnWndProc = WndProc;
	w32_class.cbClsExtra = 0;
	w32_class.cbWndExtra = 0;
	w32_class.hInstance = instance;
	w32_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	w32_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	w32_class.hbrBackground = 0;
	w32_class.lpszMenuName = NULL;
	w32_class.lpszClassName = "AIDEN"; //TODO: specify proper class name elsewhere.
	w32_class.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    Log::scc(LOG_TAG, "Win32 Init Finished.");

	create_win32_window();
    _initialise_opengl_offscreen_real();
}

//This is the core update loop for the window.
//It processes all events.
void os_win32_context::window_loop_start()
{
    Log::msg(LOG_TAG, "Starting WIN32 Window Loop");
	MSG msg;

	while (true) //NOTE(Ethan): This is super temorary
	{
        if(GetMessage(&msg, 0, 0, 0) > 0)
        {
            if(msg.message == WM_QUIT)
            {
                std::exit(1); //@UNSAFE_EXIT
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        /*
		while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT)
            {
                std::exit(1); //@UNSAFE_EXIT
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if(!event_queue.empty())
        {
            switch(event_queue.front().first)
            {
            case EVENT_OGL_CREATE_CONTEXT:
                _initialise_opengl_offscreen_real();
                break;
            }
            event_queue.front().second = true;
            event_queue.pop();
            }*/
	}
}

//Redraws the bitmap.
//The reason this function exists is just to hide all of the internal win32 context stuff.
void os_win32_context::window_update()
{
	HDC dc = GetDC(win_h); //Capture device context for drawing.
	update_win32_window(dc);
	ReleaseDC(win_h, dc);
}

//Generates a fun pattern, mostly used to test if the bitmap is drawing correctly.
void os_win32_context::window_test_draw()
{
	int pitch = width * 4;
	uint8_t* row = (uint8_t*)bitmap_buffer;

	for (int y = 0; y < height; y++)
	{
		uint8_t* pixel = (uint8_t*)row;
		for (int x = 0; x < width; x++)
		{

            //Pattern function
            *pixel = sin(((float)x + sin(y/50.f)*10.f)/150)*255;
            ++pixel;

            *pixel = cos(((float)x + sin(y/50.f)*10.f)/10)*100;
            ++pixel;

            *pixel = cos(((float)y)/50)*255;
            ++pixel;
			*pixel = 0;
			++pixel;
		}
		row += pitch;
	}
}

//Window creation and setup function.
void os_win32_context::create_win32_window()
{
    Log::msg(LOG_TAG, "Creating WIN32 Window.");

	if (!RegisterClassEx(&w32_class))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return;
	}
    Log::scc(LOG_TAG, "Created WIN32 Class.");

	win_h = CreateWindowEx(
		0,
		"AIDEN",   //TODO: properly specify this stuff somewhere else.
		"AIDEN_MAIN",
		(WS_POPUP | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX), //borderless fullscreen style.
		CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080, //TODO: variable resolution
		NULL, NULL, instance, this);

	if (win_h == NULL)
	{
        Log::critErr(LOG_TAG, "Win32 Window Creation Error: " + std::to_string(GetLastError()));

		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		std::exit(1); //@UNSAFE_EXIT
	}

	ShowWindow(win_h, cmd_show);
	UpdateWindow(win_h);
}

void os_win32_context::_ogl_init(HWND win)
{
		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
			PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
			32,                   // Colordepth of the framebuffer.
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,                   // Number of bits for the depthbuffer
			8,                    // Number of bits for the stencilbuffer
			0,                    // Number of Aux buffers in the framebuffer.
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};


		HDC dc = GetDC(win);
        if(dc==nullptr)
            Log::critErr(LOG_TAG, "fuck"); //TODO: proper error message

		int  letWindowsChooseThisPixelFormat; //TODO: refactor
		letWindowsChooseThisPixelFormat = ChoosePixelFormat(dc, &pfd);
		SetPixelFormat(dc,letWindowsChooseThisPixelFormat, &pfd);


		HGLRC render_ctx = wglCreateContext(dc);
        if(render_ctx == nullptr)
            Log::critErr(LOG_TAG, "fuck2"); //TODO: proper error message


        wglMakeCurrent (dc, render_ctx);

        gl3wInit();

        Log::scc(LOG_TAG, std::string() + "OpenGL Version: " + (char*)glGetString(GL_VERSION));
}

void os_win32_context::_initialise_opengl_offscreen_real()
{

    Log::msg(LOG_TAG, "Creating OpenGL WIN32 Window.");

    HWND hwin;
	hwin = CreateWindowEx(
		0,
		"AIDEN",   //TODO: properly specify this stuff somewhere else.
		"AIDEN_OGL",
        WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080, //TODO: variable resolution
		NULL, NULL, instance, this);

	if (hwin == NULL)
	{
        Log::critErr(LOG_TAG, "Win32 Window Creation Error: " + std::to_string(GetLastError()));

		MessageBox(NULL, "Window Creation Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
		std::exit(1); //@UNSAFE_EXIT
	}

	//ShowWindow(hwin, cmd_show);
	UpdateWindow(hwin);

}

//Creates a hidden window and opengl context
void os_win32_context::initialise_opengl_offscreen()
{
    Log::critErr(LOG_TAG, "This function is currently deprecated.");

    Log::vrb(LOG_TAG, "Queueing OpenGL WIN32 Window Creation.");

    std::atomic<bool> success;
    success = false;

    event_queue.push({(uint32_t)EVENT_OGL_CREATE_CONTEXT, success});

    int _i = 0;
    while(!success) _i += (_i>2 ? -1 : 1); //spinlock

    Log::scc(LOG_TAG, "Completed OpenGL WIN32 Window Creation.");
}

//Bitmap initialisation function.
//This is called at startup and everytime the window gets resized (which should be never in this case)
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

//The function that actually draws the bitmap to the screen
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

//Processes all events from the window.
//This is currently very big and messy but it doesn't make sense to spend that much time on
//the win32 abstraction.
LRESULT os_win32_context::win32_handler(HWND win, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE: //quit immediatly when esc is pressed
			std::exit(1); //@UNSAFE_EXIT
			break;
		default:
			break;
		}
		break;
	case WM_SIZE: //this should never happen besides at startup
	{
		RECT drawable_rect;
		GetClientRect(win, &drawable_rect);

		height = drawable_rect.bottom - drawable_rect.top;
		width = drawable_rect.right - drawable_rect.left;

        create_bitmap();
        window_test_draw();
	} break;
	case WM_NCCREATE: //Properly store the Window Context
	{
		SetWindowLongPtr(win, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);
		SetWindowPos(win, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER); //prevents data from being cached

        os_win32_context* ctx = (os_win32_context*) (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams;
        //test for ogl
        //NOTE(Ethan): this is super quick and bad. just checking if the bitmap buffer has been
        //             defined yet. If it has: window two, if it hasn't: window one.

        if(ctx->bitmap_buffer!=nullptr)
            ctx->_ogl_init(win);

        return true; //Continue Window Creation
	} break;
	case WM_CLOSE:
		std::exit(1); //@UNSAFE_EXIT
		break;
	case WM_DESTROY:
		break;
	case WM_ACTIVATEAPP: //NOTE (Ethan): I used to do something here but I don't remember lol.
		break;
    //NOTE (Ethan): It's fairly unsafe, but this shouldn't get called from the OGL window bc
    //it's never shown. To be honest, we should still test for it though.
	case WM_PAINT: //Redraw the bitmap
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
		return DefWindowProc(win, msg, wParam, lParam); //default window event handling
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

//This is a temporary struct for storing data about a thread to be created.
typedef struct
{
	void* data;
	void(*func)(void*);
} thread_func_meta;

//New thread entry point.
DWORD WINAPI thread_func(void* data)
{
    //Change the thread priority to high.
    //NOTE(Ethan): from my experience this doesn't provide any noticable difference
    //in performance (even though its supposed to).
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

//Abstraction over the win32 thread creation code, fairly self explanitory.
void os_win32_context::start_thread(void(*func)(void*), void* data)
{
	thread_func_meta* meta = new thread_func_meta();
	meta->data = data;
	meta->func = func;
	HANDLE t = CreateThread(NULL, 0, thread_func, meta, 0, NULL);
}

#endif
