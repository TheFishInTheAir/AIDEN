#include <image.h>
#include <log.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

const std::string LOG_TAG = "Image";

image image::load_from_url(std::string path, int desired_channels)
{
    resource_handle rh = resource_manager::load_file(path, RM_FILE_DW);

    image i;
    i.data = stbi_load_from_memory((uint8_t*)rh.source, rh.size,
                                   &i.width, &i.height, &i.components, desired_channels);
    i.length = i.components*i.width*i.height*sizeof(float);

    if(i.data==nullptr)
        Log::err(LOG_TAG, "Failed to load Image '"+path+"'. Not Exiting.");

    return i;
}

image image::load_from_res_handle(resource_handle rh, int desired_channels)
{

    image i;
    i.data = stbi_load_from_memory((uint8_t*)rh.source, rh.size,
                                   &i.width, &i.height, &i.components, desired_channels);
    i.length = i.components*i.width*i.height*sizeof(float);

    if(i.data==nullptr)
        Log::err(LOG_TAG, "Failed to load Image 'Unknown, Resource Handle'. Not Exiting.");

    return i;
}

image image::load_from_buffer(void* data, uint64_t size, int desired_channels)
{

    image i;
    i.data = stbi_load_from_memory((uint8_t*)data, size,
                                   &i.width, &i.height, &i.components, desired_channels);
    i.length = i.components*i.width*i.height*sizeof(float);

    if(i.data==nullptr)
        Log::err(LOG_TAG, "Failed to load Image 'Unknown, Resource Handle'. Not Exiting.");

    return i;
}
