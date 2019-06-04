#pragma once
#ifdef _WIN32

#define OS_CTX_TYPE os_win32_context


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

    void  opengl_claim_context();

	std::string set_title(std::string);
	std::string get_title();
>>>>>>> b562928f92568f05a914b5ffa7c70a04184f5696

	LRESULT win32_handler(HWND win, UINT msg, WPARAM wParam, LPARAM lParam);

private:

	void create_win32_window();
	void update_win32_window(HDC);
	void create_bitmap();
    void _initialise_opengl_offscreen_real();
    void _ogl_init(HWND);

    std::queue<std::pair<uint32_t, std::atomic<bool>&>> event_queue;

	HINSTANCE  instance;
	int        cmd_show;
	WNDCLASSEX w32_class;
	HWND       win_h;

	HWND       ogl_win_h;
    HGLRC      ogl_context;

	uint32_t width, height;
	std::string window_title;

	BITMAPINFO bitmap_info;
	void*      bitmap_buffer = nullptr;
};

#endif
