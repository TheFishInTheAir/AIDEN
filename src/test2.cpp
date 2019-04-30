#include <test2.h>
#include <iostream>
#include <string>
#include <resource_manager.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void do_the_test()
{
	// test
    //'.' is run.bat btw, not the exe (pretty stupid tbh)
    std::string file = resource_manager::get_res_path() +"good.png";

    int width, height, components;
    width = height = components = 0;

    //load './res/good.png' with stb_image
    unsigned char *imageData = stbi_load(file.c_str(),
                                         &width, &height, &components, STBI_rgb_alpha);

    std::cout << "Image Loaded: (" << width << ", " << height << ")" << std::endl;
}
