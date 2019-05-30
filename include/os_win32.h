#pragma once
#ifdef _WIN32

#include <os.h>
#include <windows.h>
#include <string>
#include <iostream>
#include <queue>
#include <atomic>
#include <utility> //std::pair

#define APP_NAME "Denoiser"

class os_win32_context : os_context
{
public:
	void  startup();
	void  window_loop_start();
	void  window_update();
	void  window_test_draw();
    void  initialise_opengl_offscreen();
	void* get_window_bitmap();
	uint32_t get_window_width();
	uint32_t get_window_height();
	void  start_thread(void(*)(void*), void*);

	std::string set_title(std::string);
	std::string get_title();

	LRESULT win32_handler(HWND win, UINT msg, WPARAM wParam, LPARAM lParam);

private:

	void create_win32_window();
	void update_win32_window(HDC);
	void create_bitmap();
    void _initialise_opengl_offscreen_real(std::atomic<bool>&);

    std::queue<std::pair<uint32_t, std::atomic<bool>&>> event_queue;

	HINSTANCE  instance;
	int        cmd_show;
	WNDCLASSEX w32_class;
	HWND       win_h;

	uint32_t width, height;
	std::string window_title;

	BITMAPINFO bitmap_info;
	void*      bitmap_buffer = nullptr;
};

#endif
