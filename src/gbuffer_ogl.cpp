#include <gbuffer_ogl.h>

gbuffer_renderer_ogl::gbuffer_renderer_ogl(os_context* ctx)
{
    this->ctx = ctx;

    ogl_context_init(ctx);
}


void gbuffer_renderer_ogl::ogl_context_init(os_context* ctx)
{
    ctx->initialise_opengl_offscreen();
}
