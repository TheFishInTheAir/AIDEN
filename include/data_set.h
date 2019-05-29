#pragma once

#include <image.h>

#include <string>
#include <unordered_map>
#include <vector>
#include <json.hpp>
#include <tiny_gltf.h>

class data_set_elem
{
public:
    uint32_t num_samples;
    image img;
};

class data_set
{
public:
    data_set(std::string path);
    //data_set(nlohmann::json data);

    bool get_is_successfully_generated(); //check if it was successfully generated

    std::string name;
    bool is_animated; //NOTE(Ethan): currently no support for animated data sets
    tinygltf::Model environment; //TODO: replace with a gltf object

    uint32_t width, height;

    std::vector<data_set_elem> sets;

    image img;
private:
    void parse_json(nlohmann::json data, std::string path);

    bool successfully_generated = false;
};
