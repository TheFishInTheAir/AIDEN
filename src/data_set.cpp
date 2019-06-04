#include <data_set.h>
#include <resource_manager.h>
#include <Log.h>
#include <image.h>
#include <stb_image.h>



data_set::data_set(std::string path)
{
    env = new ds_env();
    env->p = this;

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
            //set.parent_set = this;
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

            bool success = loader.LoadBinaryFromMemory(&env->environment, &err, &warn,
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

    env->parse_model(); //TODO(Ethan): move this out.
}

bool data_set::get_is_successfully_generated()
{
    return successfully_generated;
}

void ds_env::traverse_scene_graph(int node, double4x4 mat)
{
    tinygltf::Node n = environment.nodes[node];
    Log::tVrb(LOG_TAG, "Entering Node: "+n.name);

    Log::dbg(LOG_TAG, "  Camera Index: "+std::to_string(n.camera)+".");

    Log::dbg(LOG_TAG, "  Mesh Index: "+std::to_string(n.mesh)+".");

    for(auto const& x : n.extensions)
        Log::dbg(LOG_TAG, "  Extensions: "+x.first);


    double4x4 f_mat = double4x4(linalg::identity);
    double4x4 t_mat = double4x4(linalg::identity);
    double4x4 r_mat = double4x4(linalg::identity);
    double4x4 s_mat = double4x4(linalg::identity);

    if(n.matrix.size() == 16)
    {

        for(int x = 0; x < 4; x++)
            for(int y = 0; y < 4; y++)
            f_mat[x][y] = n.matrix[x+y*4];

    }
    if(n.translation.size() == 3)
    {
        double3 t;
        for(int i = 0; i < 3; i++)
            t[i] = n.translation[i];
        t_mat = translation_matrix(t);
    }
    if(n.scale.size() == 3)
    {
        double3 s;
        for(int i = 0; i < 3; i++)
            s[i] = n.scale[i];
        //mat *= t;
        s_mat = scaling_matrix(s);
    }
    if(n.rotation.size() == 4)
    {
        double4 r; // TODO: check if linalg has quats natively
        for(int i = 0; i < 4; i++)
            r[i] = n.rotation[i];
        r_mat = rotation_matrix(r);
    }

    mat = double4x4(f_mat * t_mat * r_mat * s_mat * mat);

    if(n.mesh!=-1) //Generate a mesh
    {
        auto mesh = environment.meshes[n.mesh];
        for(auto prim : mesh.primitives)
        {
            ds_mesh* m = new ds_mesh();
            m->p = p;
            m->mesh = mesh;
            m->prim = prim;
            m->model = mat;

            Log::dbg(LOG_TAG, "Num Attribs: "+std::to_string(prim.attributes.size()) + ", THING: "+
                     std::to_string(prim.mode));
        }

    }

    for(int c : n.children)
        traverse_scene_graph(c, mat);

    Log::tVrb(LOG_TAG, "Exiting Node: "+n.name);
}


void ds_env::parse_model()
{
    Log::vrb(LOG_TAG, "Converting GLTF Model.");

    tinygltf::Scene s = environment.scenes[0]; //automatically use the first scene.
    Log::vrb(LOG_TAG, "Using first scene: '"+s.name+"'.");

    if(environment.cameras.size() > 1)
        Log::wrn(LOG_TAG, "GLTF Model has more than one camera, using first.");

    if(environment.skins.size() > 0)
        Log::wrn(LOG_TAG, "GLTF Model has skins, currently no support for this.");

    if(environment.animations.size() > 0)
        Log::wrn(LOG_TAG, "GLTF Model has animations, currently no support for this.");

    for(auto const& x : s.extensions)
        Log::dbg(LOG_TAG, "Extensions: "+x.first);

    for(std::string e : environment.extensionsUsed)
        Log::tVrb(LOG_TAG, "GLTF Model uses extension: '" + e + "'.");

    for(std::string e : environment.extensionsRequired)
        Log::tVrb(LOG_TAG, "GLTF Model requires extension: '" + e + "'.");


    for(int n : s.nodes)
        traverse_scene_graph(n, double4x4(linalg::identity));
}

void data_set::render()
{

}
