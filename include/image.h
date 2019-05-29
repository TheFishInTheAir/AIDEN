#pragma once
#include <string>
#include <resource_manager.h>

//this is just a super leightweight wrapper over stb image

//NOTE(Ethan): tbh idk if this deserves its own file.
class image
{
public:
    static image load_from_url(std::string path, int desired_channels);
    static image load_from_res_handle(resource_handle rh, int desired_channels);
    static image load_from_buffer(void* data, uint64_t size, int desired_channels);

    //TODO: make this read only (tbh not a fan of getters though)
    int width = 0, height = 0;
    uint8_t* data   = nullptr;
    uint64_t length = 0;
    int components = 0;
};
