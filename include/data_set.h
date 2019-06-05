#pragma once

#include <image.h>

#include <linalg.h>


#include <string>
#include <unordered_map>
#include <vector>
#include <json.hpp>
#include <tiny_gltf.h>
using namespace linalg::aliases;
class data_set_elem
{
public:
    uint32_t num_samples;
    image img;

};

class ds_env;
class ds_mesh;
class ds_camera;


class data_set
{
public:
    data_set(std::string path);
    //data_set(nlohmann::json data);

    void render(unsigned int);


    bool get_is_successfully_generated(); //check if it was successfully generated

    std::string name;
    bool is_animated; //NOTE(Ethan): currently no support for animated data sets

    uint32_t width, height;

    std::vector<data_set_elem> sets;
    ds_env* env;
    image img;
private:
    void parse_json(nlohmann::json data, std::string path);

    //flattens scene graph and convert to easily renderable format.

    const std::string LOG_TAG = "DataSet";

    bool successfully_generated = false;
};

class ds_env
{
public:
    data_set* p;

    std::vector<ds_mesh*> meshes;
    ds_camera* cam = nullptr;

    tinygltf::Model environment;
    void parse_model();
private:
    void traverse_scene_graph(int node, double4x4, int depth);

    const std::string LOG_TAG = "DataSetEnvironment";
};

class ds_camera
{
public:
    ds_camera(data_set*, double4x4 inv_camera, tinygltf::Camera);
    data_set* p;
    double4x4 proj;
    double4x4 view;

    float4x4  gl_vp;

private:
    const std::string LOG_TAG = "DataSetCamera";
};

class ds_mesh
{
public:
    ds_mesh(data_set*, tinygltf::Mesh, tinygltf::Primitive, double4x4);

    data_set* p;

    tinygltf::Mesh mesh;
    tinygltf::Primitive prim;

    unsigned int vao = 0;

    unsigned int index_buffer    = 0;

    unsigned int vertex_buffer   = 0;
    unsigned int normal_buffer   = 0;
    unsigned int texcoord_buffer = 0;
    unsigned int num_indices     = 0;
    double4x4 model;

    void init_opengl();

private:
    const std::string LOG_TAG = "DataSetMesh";
};
