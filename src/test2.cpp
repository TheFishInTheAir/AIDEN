#include <test2.h>
#include <iostream>
#include <string>
#include <resource_manager.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void do_the_test()
{
    //'.' is run.bat btw, not the exe (pretty stupid tbh)
    resource_handle handle = resource_manager::load_file(
        resource_manager::get_res_path() +"good.png", RM_FILE_DR);

    int width, height, components;
    width = height = components = 0;

    //load './res/good.png' with stb_image
    unsigned char *imageData = stbi_load_from_memory((uint8_t*)handle.source, handle.size,
                                         &width, &height, &components, STBI_rgb_alpha);

    std::cout << "Image Loaded: (" << width << ", " << height << ")" << std::endl;
}
