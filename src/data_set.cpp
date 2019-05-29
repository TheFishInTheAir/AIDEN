#include <data_set.h>
#include <resource_manager.h>
#include <Log.h>
#include <image.h>
#include <stb_image.h>


const std::string LOG_TAG = "DataSet";

data_set::data_set(std::string path)
{
    if(path.back() != '/' && path.back() != '\\')
        path.append("/");
    resource_handle rh = resource_manager::load_file(path+"manifest.json", RM_FILE_DR);
    nlohmann::json data = nlohmann::json::parse((char*) rh.source);
    parse_json(data, path);
}

void data_set::parse_json(nlohmann::json data, std::string path)
{
    try
    {
        name        = data["data_name"]  .get<std::string>();
        is_animated = data["is_animated"].get<bool>();
        width       = (uint32_t) data["width_resolution" ].get<int>();
        height      = (uint32_t) data["height_resolution"].get<int>();

        if(is_animated)
        {
            Log::err(LOG_TAG, "SKIPPING, Animated sets are currently not supported.");
            width = height = 0;
            return;
        }

        //NOTE(Ethan): this assumes that everything is static
        nlohmann::json jsets = data["static_data_set"];
        sets.resize(jsets.size());
        int i = 0;
        for(auto s : jsets)
        {
            auto& set = sets[i++];
            set.num_samples = (uint32_t) s["num_samples"].get<int>();
            set.img = image::load_from_url(path + s["path"].get<std::string>(),
                                           STBI_rgb_alpha);
        }

        img = image::load_from_url( path + data["static_reference"]["path"].get<std::string>() ,
                                    STBI_rgb_alpha);

        //GLTF loading
        {
            std::string gltf_path = data["scene_path"].get<std::string>();
            resource_handle rh = resource_manager::load_file(path+gltf_path, RM_FILE_DR);

            static tinygltf::TinyGLTF loader;

            std::string err;
            std::string warn;

            bool success = loader.LoadBinaryFromMemory(&environment, &err, &warn,
                                                       (uint8_t*)rh.source,
                                                       rh.size, path+gltf_path);

            if(!warn.empty())
                Log::wrn(LOG_TAG, warn);

            if(!err.empty())
            {
                Log::err(LOG_TAG, err);
                return;
            }

            if(!success)
            {
                Log::err(LOG_TAG, "Loading GLTF File '"+path+gltf_path+"'. Stopping.");
                return;
            }

        }

        //set success flag to true.
        successfully_generated = true;
    }
    catch (nlohmann::json::exception& e)
    {
        Log::err(LOG_TAG, std::string()+"SKIPPING, Json Error, possibly corrupt data set: "
                 + e.what() + '\n'+"exception id: " + std::to_string(e.id));
    }
}

bool data_set::get_is_successfully_generated()
{
    return successfully_generated;
}
