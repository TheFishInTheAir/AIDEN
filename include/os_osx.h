#pragma once
#ifndef _WIN32

#define OS_CTX_TYPE os_osx_context

#include <os.h>
#include <string>
#include <iostream>
#define APP_NAME "Denoiser"

class os_osx_context : os_context
{
public:
    void  startup();
	void  window_loop_start();
	void  window_update();
	void  window_test_draw();
	void* get_window_bitmap();
	uint32_t get_window_width();
	uint32_t get_window_height();

    void  initialise_opengl_offscreen();

	void  start_thread(void(*)(void*), void*);

};

#endif
