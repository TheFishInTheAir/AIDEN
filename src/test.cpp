#include <iostream>
#include <string>
#include <test2.h>
#include <resource_manager.h>

#include <os.h>
#include <os_win32.h>

#include <tensorflow/c/c_api.h>

int main()
{
	os_context* ctx = (os_context*) new os_win32_context();
	ctx->startup();


    std::cout << "The build script works, amazing." << std::endl;

    //NOTE(Ethan): btw i've seen jblow and those people use this kinda comment where it says who
    //             wrote it and stuff like that.

	resource_manager::init();


	ctx->window_loop_start();

    printf("Hello from TensorFlow C library version %s\n", TF_Version());


    //btw this is just using c style functions, so the syntax is kinda bad. (we prob won't use this
    //in the actual project)
    do_the_test();


	getc(stdin);

    return 0;
}
