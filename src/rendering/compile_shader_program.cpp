#include <iostream>

#include <GL/glew.h>

#include "resources/locator.h"
#include "resources/load_textfile.h"
#include "rendering/compile_shader_program.h"

namespace
{
using resources::locator::SHADERS_PATH;

void printShaderInfoLog(const uint obj, const char* fn)
{
    GLint infologLength = 0;
    GLint charsWritten = 0;
    char* infoLog;

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 2)
    {
        fprintf(stderr, "[From %s:]\n", fn);
        infoLog = (char*)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
        fprintf(stderr, "%s\n", infoLog);
        free(infoLog);
    }
}

void printProgramInfoLog(const uint obj, const char* fn)
{
    GLint infologLength = 0;
    GLint charsWritten = 0;
    char* infoLog;

    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 2)
    {
        fprintf(stderr, "[From %s:]\n", fn);
        infoLog = (char*)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
        fprintf(stderr, "%s\n", infoLog);
        free(infoLog);
    }
}
}  // namespace

namespace rendering
{
std::unique_ptr<ShaderProgram> compileComputeShader(const std::string& name,
                                                    const std::string& computeSource)
{
    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);

    const std::string src = resources::load_textfile(computeSource, SHADERS_PATH);
    const char* s = src.c_str();
    glShaderSource(computeShader, 1, &s, NULL);

    glCompileShader(computeShader);

    GLint isCompiled = 0;
    glGetShaderiv(computeShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        printShaderInfoLog(computeShader, computeSource.c_str());

        glDeleteShader(computeShader);
        throw std::runtime_error("Unable to compile compute shader.");
    }

    GLuint program = glCreateProgram();

    glAttachShader(program, computeShader);

    glLinkProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE)
    {
        printProgramInfoLog(program, "compiled compute shader program");

        glDeleteProgram(program);
        glDeleteShader(computeShader);

        throw std::runtime_error("Unable to link compute shader program.");
    }

    glDetachShader(program, computeShader);

    std::cout << "Creating Compute ShaderProgram (" << name << ")" << std::endl;

    return std::make_unique<ShaderProgram>(name, ShaderType::NORMAL, program);
}

// From https://www.khronos.org/opengl/wiki/Shader_Compilation
std::unique_ptr<ShaderProgram> compileShaders(const std::string& name,
                                              const std::string& vertex_filename,
                                              const std::string& fragment_filename)
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    const std::string vert_src = resources::load_textfile(vertex_filename, SHADERS_PATH);
    const char* vert_chars = vert_src.c_str();
    glShaderSource(vertexShader, 1, &vert_chars, NULL);

    glCompileShader(vertexShader);

    GLint isCompiled = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        printShaderInfoLog(vertexShader, vertex_filename.c_str());

        glDeleteShader(vertexShader);

        throw std::runtime_error("Unable to compile vertex shader.");
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const std::string frag_src = resources::load_textfile(fragment_filename, SHADERS_PATH);
    const char* frag_chars = frag_src.c_str();
    glShaderSource(fragmentShader, 1, &frag_chars, NULL);

    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        printShaderInfoLog(fragmentShader, fragment_filename.c_str());

        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);

        throw std::runtime_error("Unable to compile fragment shader.");
    }

    // Vertex and fragment shaders are successfully compiled.
    // Now time to link them together into a program.
    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE)
    {
        printProgramInfoLog(program, "compiled shader program");

        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        throw std::runtime_error("Unable to link shader program.");
    }

    // Always detach shaders after a successful link.
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);

    // Also safe to delete shaders
    // https://gamedev.stackexchange.com/questions/47910/after-a-succesful-gllinkprogram-should-i-delete-detach-my-shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::cout << "Creating ShaderProgram (" << name << ")" << std::endl;

    return std::make_unique<ShaderProgram>(name, ShaderType::COMPUTE, program);
}
}  // namespace rendering
