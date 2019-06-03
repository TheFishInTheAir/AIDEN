#pragma once

#include <GL/gl3w.h>

#include <image.h>
#include <os.h>
#include <data_set.h>


//This generates the gbuffer for the denoising.
//Just using OpenGL.

struct gbuffer
{
    gbuffer(unsigned int width, unsigned int height);

    void init_ogl();

    fbo_image* world_pos;
    fbo_image* normals;
    fbo_image* mesh_id;
    fbo_image* primitive_id;
    fbo_image* tri_barycentric;
    fbo_image* material_id;
    fbo_image* diffuse;
    fbo_image* specular;

    unsigned int get_width();
    unsigned int get_height();

private:
    unsigned int width, height;
};

class gbuffer_renderer_ogl
{
public:
    gbuffer_renderer_ogl(os_context*);

    void render_gbuffer();

    void create_gbuffer_for_set(data_set*);
    void delete_gbuffer();

    gbuffer* get_gbuffer();
private:
    static void ogl_context_init(os_context*);

    gbuffer*  buffer = nullptr;
    data_set* current_set = nullptr;

    os_context* ctx;
};
