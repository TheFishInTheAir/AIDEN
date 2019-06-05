#include <shaders.h>
#include <vector>
#include <utility>
#include <log.h>
#include <GL/gl3w.h>

#define EXIT_ON_SHADER_FAILURE

#ifdef EXIT_ON_SHADER_FAILURE
#define SHADER_ERROR std::exit(1)
#else
#define SHADER_ERROR
#endif

namespace shaders
{
    const std::string LOG_TAG = "Shader";




    const std::string test_shader_vert = R"1SRAW1(
#version 330
layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 texcoord;

uniform mat4 MVP;
void main()
{
    gl_Position =  vec4(vert, 1);// MVP *
}

)1SRAW1";

    const std::string test_shader_frag = R"1SRAW1(
#version 330

out vec4 colour;
void main()
{
    colour = vec4(1,1,1,1);
}

)1SRAW1";
    unsigned int test_shader_program = 0;
    unsigned int test_shader_vert_gl = 0;
    unsigned int test_shader_frag_gl = 0;


    struct shader_table
    {
        const std::string* v;
        const std::string* f;
        unsigned int* prog;
        unsigned int* v_s;
        unsigned int* f_s;

        shader_table(const std::string* vi,
                     const std::string* fi,
                     unsigned int* progi,
                     unsigned int* v_si,
                     unsigned int* f_si)
            : v(vi), f(fi), prog(progi), v_s(v_si), f_s(f_si)
            {
                // No need for assignment here
            }
    };

    void compile()
    {
        Log::vrb(LOG_TAG, "Beginning Shader Compilation.");
        std::vector<shader_table> programs =
            {
                {&test_shader_vert, &test_shader_frag,
                 &test_shader_program, &test_shader_vert_gl, &test_shader_frag_gl}
            };
        for(auto st : programs)
        {
            *st.prog = glCreateProgram();

            *st.v_s = glCreateShader(GL_VERTEX_SHADER);
            *st.f_s = glCreateShader(GL_FRAGMENT_SHADER);
            const char* shaders[] = {st.v->c_str(), st.f->c_str()};
            glShaderSource(*st.v_s, 1, shaders, NULL);
            glShaderSource(*st.f_s, 1, shaders+1, NULL);

            int  success;
            char info_log[512];

            glCompileShader(*st.v_s);
            glGetShaderiv(*st.v_s, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(*st.v_s, 512, NULL, info_log);
                Log::err(LOG_TAG, std::string()+"GLSL Vert Error: "+info_log);
                SHADER_ERROR;
            }

            glCompileShader(*st.f_s);
            glGetShaderiv(*st.f_s, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(*st.f_s, 512, NULL, info_log);
                Log::err(LOG_TAG, std::string()+"GLSL Frag Error: "+info_log);
                SHADER_ERROR;
            }

            glAttachShader(*st.prog, *st.v_s);
            glAttachShader(*st.prog, *st.f_s);
            glLinkProgram(*st.prog);

            glGetProgramiv(*st.prog, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(*st.prog, 512, NULL, info_log);
                Log::err(LOG_TAG, std::string()+"GLSL Program link Error: "+info_log);
                SHADER_ERROR;
            }
        }
        Log::scc(LOG_TAG, "Finished Shader Compilation.");
    }

}
