#include <os_osx.h>
#ifndef _WIN32

void  os_osx_context::startup()
{

}

void  os_osx_context::window_loop_start()
{

}

void  os_osx_context::window_update()
{

}

void  os_osx_context::window_test_draw()
{

}

void* os_osx_context::get_window_bitmap()
{
    return nullptr;
}

uint32_t os_osx_context::get_window_width()
{
    return 0;
}

uint32_t os_osx_context::get_window_height()
{
    return 0;
}

void  os_osx_context::start_thread(void(*)(void*), void*)
{

}

#endif
