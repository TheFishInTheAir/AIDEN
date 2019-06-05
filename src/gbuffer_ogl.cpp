#include <gbuffer_ogl.h>
#include <log.h>
#include <shaders.h>

const std::string LOG_TAG = "GBuffer Renderer";

gbuffer_renderer_ogl::gbuffer_renderer_ogl(os_context* ctx)
{
    this->ctx = ctx;

    ogl_context_init(ctx);
}


void gbuffer_renderer_ogl::create_gbuffer_for_set(data_set* set)
{
    current_set = set;

    buffer = new gbuffer(current_set->width, current_set->height);

    buffer->init_ogl();

}


void gbuffer_renderer_ogl::render_gbuffer()
{
    //TODO: add checks to make sure data set is valid
    glBindFramebuffer(GL_FRAMEBUFFER, buffer->world_pos->fbo);
    glClearColor(1.f, 0.f, 1.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
    glEnable(GL_DEPTH_TEST);
    current_set->render(shaders::test_shader_program);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Log::dbg("Finished Render\n");
}

void gbuffer_renderer_ogl::delete_gbuffer()
{

}

gbuffer* gbuffer_renderer_ogl::get_gbuffer()
{
    if(current_set == nullptr)
        Log::err(LOG_TAG, "Attempt to retrieve gbuffer with no active set.");
    return buffer;
}

void gbuffer_renderer_ogl::ogl_context_init(os_context* ctx)
{
    static bool has_init = false;
    if(has_init)
        return;
    has_init = true;

    ctx->opengl_claim_context();

    Log::scc(LOG_TAG, std::string() + "OpenGL Version: " + (char*)glGetString(GL_VERSION));

    //buffer = new gbuffer();

}


const std::string GB_LOG_TAG = "GBuffer";

gbuffer::gbuffer(unsigned int width, unsigned int height)
{\
    this->width = width;
    this->height = height;
}

void gbuffer::init_ogl()
{
    Log::vrb(GB_LOG_TAG, "Beginning GBuffer Creation.");
    world_pos       = new fbo_image(width, height);
    normals         = new fbo_image(width, height);
    mesh_id         = new fbo_image(width, height);
    primitive_id    = new fbo_image(width, height);
    tri_barycentric = new fbo_image(width, height);
    material_id     = new fbo_image(width, height);
    diffuse         = new fbo_image(width, height);
    specular        = new fbo_image(width, height);

    if(
        world_pos->is_valid()       &&
        normals->is_valid()         &&
        mesh_id->is_valid()         &&
        primitive_id->is_valid()    &&
        tri_barycentric->is_valid() &&
        material_id->is_valid()     &&
        diffuse->is_valid()         &&
        specular->is_valid())
    {
        Log::scc(GB_LOG_TAG, "GBuffer Creation Was Successfull.");
    }
    else
    {
        Log::critErr(GB_LOG_TAG, "GBuffer Creation Was Unsuccessfull.");
    }


}

unsigned int gbuffer::get_width()
{
    return width;
}

unsigned int gbuffer::get_height()
{
    return height;
}
