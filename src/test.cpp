#include <iostream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int main()
{
    std::cout << "The build script works, amazing." << std::endl;

    std::string file = "./res/good.png"; //'.' is run.bat btw, not the exe (pretty stupid tbh)

    int width, height, components;
    width = height = components = 0;

    //load ./res/good.png with stb_image
    unsigned char *imageData = stbi_load(file.c_str(),
                                         &width, &height, &components, STBI_rgb_alpha);

    std::cout << "Image Loaded: (" << width << ", " << height << ")" << std::endl;

    return 0;
}
