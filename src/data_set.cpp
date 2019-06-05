#include <data_set.h>
#include <resource_manager.h>
#include <Log.h>
#include <image.h>
#include <stb_image.h>

#include <GL/gl3w.h>

//taken from glview tinygltf demo.
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define MATPRINT(m) for(int i = 0; i < 4; i++) printf("%lf %lf %lf %lf\n", \
                                                      m[i][0], m[i][1], m[i][2], m[i][3])


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

void ds_env::traverse_scene_graph(int node, double4x4 mat, int depth)
{
    tinygltf::Node n = environment.nodes[node];

    std::string preface = "";
    for(int i = 0; i < depth+1; i++)
        preface += "-";

    Log::tVrb(LOG_TAG, preface+"Entering Node: "+n.name);

    for(auto const& x : n.extensions)
        Log::dbg(LOG_TAG, preface+" Extensions: "+x.first);


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
    double4x4 test = linalg::mul(linalg::mul(t_mat, r_mat), s_mat); //TODO: refactor
    //MATPRINT(test);
    mat = linalg::mul(linalg::mul(f_mat, test), mat);
    //MATPRINT(mat);

    if(n.mesh!=-1) //Generate a mesh
    {
        auto mesh = environment.meshes[n.mesh];
        for(auto prim : mesh.primitives)
        {
            ds_mesh* m = new ds_mesh(p, mesh, prim, mat);
            meshes.push_back(m);
        }

    }
    if(n.camera != -1)
    {
        if(cam==nullptr)
        {
            Log::tVrb(LOG_TAG, preface + " Is Active Camera.");
            cam = new ds_camera(p, mat, environment.cameras[n.camera]);
        }
        else
        { Log::wrn(LOG_TAG, preface + " Inactive Camera."); }
    }

    for(int c : n.children)
        traverse_scene_graph(c, mat, ++depth);

    Log::tVrb(LOG_TAG, preface+"Exiting Node:  "+n.name);
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

    for(auto& acc : environment.accessors)
        if(acc.sparse.isSparse)
            Log::err(LOG_TAG, "Sparse allocators aren't currently supported.");



    for(int n : s.nodes)
        traverse_scene_graph(n, double4x4(linalg::identity), 0);
}

void data_set::render(unsigned int program)
{
    for(auto m : env->meshes)
    {
        glUseProgram(program);
        float4x4 mvp = linalg::mul(env->cam->gl_vp, (float4x4)m->model);
        glBindVertexArray(m->vao);
        glUniformMatrix4fv(glGetUniformLocation(program, "MVP"), 1, GL_FALSE, &mvp[0][0]);
        glDrawElements(GL_TRIANGLES, m->num_indices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

ds_mesh::ds_mesh(data_set* p, tinygltf::Mesh mesh, tinygltf::Primitive prim, double4x4 model)
{
    this->p = p;
    this->mesh = mesh;
    this->model = model;
    this->prim  = prim;

    init_opengl();

}

void ds_mesh::init_opengl()
{
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    size_t total_size = 0;
    for(auto k : prim.attributes)
    { //vbos
        unsigned int* vbo;
        unsigned int pos;
        if(k.first == "POSITION")
        {vbo = &vertex_buffer;   pos = 0;}
        else if(k.first == "NORMAL")
        {vbo = &normal_buffer;   pos = 1;}
        else if(k.first == "TEXCOORD_0")
        {vbo = &texcoord_buffer; pos = 2;}
        else
        {
            Log::wrn(LOG_TAG, "Unknown Attribute Type: '"+k.first+"'. SKIPPING ATTRIBUTE.");
            continue;
        }
        auto& accessor   = p->env->environment.accessors[k.second];
        auto& bufferView = p->env->environment.bufferViews[accessor.bufferView];
        auto& buffer     = p->env->environment.buffers[bufferView.buffer];
        total_size += bufferView.byteLength;
        glGenBuffers(1, vbo);

        glBindBuffer(GL_ARRAY_BUFFER, *vbo);
        glBufferData(GL_ARRAY_BUFFER, bufferView.byteLength,
                     buffer.data.data()+bufferView.byteOffset, GL_STATIC_DRAW);
        glEnableVertexAttribArray(pos);

        //accessor type returns an enum, but the enum values work in this context
        //as long as accessor type is never scalar (which it shouldn't be for vertices, normals, or uvs)

        glVertexAttribPointer(pos, accessor.type,
                              accessor.componentType,
                              accessor.normalized ? GL_TRUE : GL_FALSE,
                              accessor.ByteStride(bufferView),
                              BUFFER_OFFSET(accessor.byteOffset));


    }
    { //ibo
        auto& accessor   = p->env->environment.accessors[prim.indices];
        auto& bufferView = p->env->environment.bufferViews[accessor.bufferView];
        auto& buffer     = p->env->environment.buffers[bufferView.buffer];
        Log::tVrb(LOG_TAG, "Mesh has: '"+
                  std::to_string(accessor.count)+"' indices. '"+
                  std::to_string(accessor.count/3)+"' tris. '"+
                  std::to_string(total_size+bufferView.byteLength)+"' bytes total.");
        num_indices = accessor.count;

        glGenBuffers(1, &index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferView.byteLength,
                     buffer.data.data()+bufferView.byteOffset, GL_STATIC_DRAW);
    }

    glBindVertexArray(0);

}

ds_camera::ds_camera(data_set* p, double4x4 inv_view, tinygltf::Camera c)
{
    this->p = p;
    if(c.type == "perspective")
    {
        double4x4 proj = linalg::perspective_matrix(c.perspective.yfov,  c.perspective.aspectRatio,
                                            c.perspective.znear, c.perspective.zfar);
        this->proj = proj;
        this->view = linalg::inverse(inv_view);
        this->gl_vp = (float4x4) linalg::mul(proj, view);
    }
    else
    {
        Log::err(LOG_TAG, "Unsupported Camera type '"+c.type+"'.");
    }
}
