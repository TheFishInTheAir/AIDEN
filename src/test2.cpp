#include <test2.h>
#include <iostream>
#include <string>
#include <resource_manager.h>
#include <log.h>
#include <stb_image.h>
#include <data_set.h>

void do_the_test(os_context* ctx)
{
    //'.' is run.bat btw, not the exe (pretty stupid tbh)
    resource_handle handle = resource_manager::load_file(
        resource_manager::get_res_path() + "good.png", RM_FILE_DW);

    int width, height, components;
    width = height = components = 0;

    //load './res/good.png' with stb_image
    unsigned char *imageData = stbi_load_from_memory((uint8_t*)handle.source, handle.size,
                                         &width, &height, &components, STBI_rgb_alpha);


    if(imageData == nullptr)
    {
        Log::err(std::string() + "Failed to load image. Reason: " + stbi_failure_reason());
    }
    else
        Log::scc("Image Loaded: (" + std::to_string(width) + ", " + std::to_string(height) + ")");
    uint8_t* bmap = (uint8_t*) ctx->get_window_bitmap();
    for(int y = 0; y < height; y++) //blit and convert to the win32 colourspace.
        for(int x = 0; x <width; x++)
        {
            bmap[(x+y*ctx->get_window_width())*4]   = imageData[(x+y*width)*4+2];
            bmap[(x+y*ctx->get_window_width())*4+1] = imageData[(x+y*width)*4+1];
            bmap[(x+y*ctx->get_window_width())*4+2] = imageData[(x+y*width)*4];
            bmap[(x+y*ctx->get_window_width())*4+3] = imageData[(x+y*width)*4+3];
        }


    //Set Test
    data_set* ds = new data_set(resource_manager::get_res_path()+"data/test1/");

    //test blit the loaded files
    for(int y = 0; y < height; y++) //blit and convert to the win32 colourspace.
        for(int x = 0; x <width; x++)
        {
            bmap[(x+600+y*ctx->get_window_width())*4]   = ds->img.data[(x+y*width)*4+2];
            bmap[(x+600+y*ctx->get_window_width())*4+1] = ds->img.data[(x+y*width)*4+1];
            bmap[(x+600+y*ctx->get_window_width())*4+2] = ds->img.data[(x+y*width)*4];
            bmap[(x+600+y*ctx->get_window_width())*4+3] = ds->img.data[(x+y*width)*4+3];
        }

    for(int y = 0; y < height; y++) //blit and convert to the win32 colourspace.
        for(int x = 0; x <width; x++)
        {
            bmap[(x+1200+y*ctx->get_window_width())*4]   = ds->sets[0].img.data[(x+y*width)*4+2];
            bmap[(x+1200+y*ctx->get_window_width())*4+1] = ds->sets[0].img.data[(x+y*width)*4+1];
            bmap[(x+1200+y*ctx->get_window_width())*4+2] = ds->sets[0].img.data[(x+y*width)*4];
            bmap[(x+1200+y*ctx->get_window_width())*4+3] = ds->sets[0].img.data[(x+y*width)*4+3];
        }

    Log::scc("Finished Tests");

}
