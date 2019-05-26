#include <iostream>
#include <string>
#include <test2.h>
#include <resource_manager.h>

#include <os.h>
#include <os_win32.h>
#include <log.h>

#include <tensorflow/c/c_api.h>

void run(void* meta)
{
    os_context* ctx = (os_context*)meta;

    do_the_test(ctx);
    ctx->window_update();

}

int main()
{
	os_context* ctx = (os_context*) new os_win32_context();
	ctx->startup();

    Log::scc("This build script actually works.");
    //NOTE(Ethan): btw i've seen jblow and those people use this kinda comment where it says who
    //             wrote it and stuff like that.

	resource_manager::init();


    Log::scc(std::string() + "Hello from TensorFlow C library version: " + TF_Version());


    //btw this is just using c style functions, so the syntax is kinda bad. (we prob won't use this
    //in the actual project)

    ctx->start_thread(run, ctx);
	ctx->window_loop_start();



	getc(stdin);

    return 0;
}
