#include <image.h>

#include <GL/gl3w.h>

#include <log.h>
#include <stb_image.h>

const std::string LOG_TAG = "Image";

image image::load_from_url(std::string path, int desired_channels)
{
    resource_handle rh = resource_manager::load_file(path, RM_FILE_DW);

    image i;
    i.data = stbi_load_from_memory((uint8_t*)rh.source, rh.size,
                                   &i.width, &i.height, &i.components, desired_channels);
    i.length = i.components*i.width*i.height*sizeof(float);

    if(i.data==nullptr)
        Log::err(LOG_TAG, "Failed to load Image '"+path+"'. Not Exiting.");

    return i;
}

image image::load_from_res_handle(resource_handle rh, int desired_channels)
{

    image i;
    i.data = stbi_load_from_memory((uint8_t*)rh.source, rh.size,
                                   &i.width, &i.height, &i.components, desired_channels);
    i.length = i.components*i.width*i.height*sizeof(float);

    if(i.data==nullptr)
        Log::err(LOG_TAG, "Failed to load Image 'Unknown, Resource Handle'. Not Exiting.");

    return i;
}

image image::load_from_buffer(void* data, uint64_t size, int desired_channels)
{

    image i;
    i.data = stbi_load_from_memory((uint8_t*)data, size,
                                   &i.width, &i.height, &i.components, desired_channels);
    i.length = i.components*i.width*i.height*sizeof(float);

    if(i.data==nullptr)
        Log::err(LOG_TAG, "Failed to load Image 'Unknown, Resource Handle'. Not Exiting.");

    return i;
}


fbo_image::fbo_image(unsigned int width, unsigned int height) //TODO: add support for 1 component imgs
{
    this->width  = width;
    this->height = height;
    this->components = 3;
    this->length = width*height*this->components*sizeof(float);
    this->data = (uint8_t*) malloc(this->length);

    Log::vrb(LOG_TAG, "Beginning Framebuffer Creation   ("+
             std::to_string(width)+", "+std::to_string(height)+").");

    //Generate Framebuffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);


    //Generate Texture attachment
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    //allocate space
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, nullptr);

    //we never sample so we don't need anything complicated.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //Add the tex as an attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    //unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    //Generate Depth Renderbuffer
    glGenRenderbuffers(1, &depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);

    //allocate space
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    //Add the renderbuffer as an attachment
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                              depth_buffer);

    //unbind texture
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        Log::scc(LOG_TAG, "Successfully Created Framebuffer ("+
                 std::to_string(width)+", "+std::to_string(height)+").");
    else
        Log::critErr(LOG_TAG, "Failed Creating Framebuffer ("+
                 std::to_string(width)+", "+std::to_string(height)+").");

    //unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool fbo_image::is_valid()
{
    return fbo != 0 && tex != 0 && depth_buffer != 0;
}

fbo_image::~fbo_image()
{
    glDeleteFramebuffers(1, &fbo);
    free(data);
}
