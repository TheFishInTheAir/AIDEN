#pragma once

#include <GL/gl3w.h>

#include <image.h>
#include <os.h>

//This generates the gbuffer for the denoising.
//Just using OpenGL.

struct gbuffer
{
    image world_pos;
    image normals;
    image mesh_id;
    image primitive_id;
    image tri_barycentric;
    image material_id;
    image diffuse;
    image specular;
};

class gbuffer_renderer_ogl
{
public:
    gbuffer_renderer_ogl(os_context*);

private:
    static void ogl_context_init(os_context*);
    static bool ogl_context_intialised;

    os_context* ctx;
};
