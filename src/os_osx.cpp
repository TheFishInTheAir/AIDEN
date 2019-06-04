#include <os_osx.h>
#ifndef _WIN32

#include <pthread.h>


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

struct _osx_entry_data
{
    void* args;
    void(*func)(void*);
};

void* _osx_entry_point(void* args)
{
    _osx_entry_data* data = (_osx_entry_data*) args;
    (*data->func)(data->args); //call the function
    delete(data);
}

void os_osx_context::start_thread(void(*func)(void*), void* args)
{
    _osx_entry_data* data = new _osx_entry_data();
    data->args = args;
    data->func = func;
    pthread_t thread;
    pthread_create(&thread, NULL, &_osx_entry_point, data);
}

void os_osx_context::initialise_opengl_offscreen()
{

}


#endif
