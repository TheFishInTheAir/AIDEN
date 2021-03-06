#pragma once
#include <cstdint>

class os_context
{
public:

	//initialise os context
	virtual void  startup() = 0;

	//start gui loop
	virtual void  window_loop_start() = 0;

	//redraw bitmap
	virtual void  window_update() = 0;

    //generate opengl offscreen window & context
	virtual void  initialise_opengl_offscreen() = 0;

    //Set current OpenGL context
    virtual void  opengl_claim_context() = 0;

	//fills window with test bitmap
	virtual void  window_test_draw() = 0;
	
	//get window bitmap pixel buffer
	virtual void* get_window_bitmap() = 0;
	
	virtual uint32_t get_window_width() = 0;
	virtual uint32_t get_window_height() = 0;
	
	//starts a thread (function pointer, user pointer)
	virtual void  start_thread(void(*)(void*), void*) = 0;
};
