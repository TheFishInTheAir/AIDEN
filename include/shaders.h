#pragma once
#include <string>


namespace shaders{
    extern const std::string test_shader_vert;
    extern const std::string test_shader_frag;

    extern unsigned int test_shader_program;
    extern unsigned int test_shader_vert_gl;
    extern unsigned int test_shader_frag_gl;

    void compile();

}
