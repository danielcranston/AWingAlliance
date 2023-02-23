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

GLuint createShader(const std::string& filename, GLuint shader_type)
{
    GLuint shader = glCreateShader(shader_type);

    const std::string vert_src = resources::load_textfile(filename, SHADERS_PATH);
    const char* shader_source = vert_src.c_str();
    glShaderSource(shader, 1, &shader_source, NULL);

    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        printShaderInfoLog(shader, filename.c_str());

        glDeleteShader(shader);

        throw std::runtime_error("Unable to compile shader.");
    }

    return shader;
}

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

    // std::cout << "Creating Compute ShaderProgram (" << name << ")" << std::endl;

    return std::make_unique<ShaderProgram>(name, ShaderType::NORMAL, program);
}

// From https://www.khronos.org/opengl/wiki/Shader_Compilation
std::unique_ptr<ShaderProgram> compileShaders(const std::string& name,
                                              const std::string& vertex_filename,
                                              const std::string& fragment_filename,
                                              const std::optional<std::string>& geometry_filename)
{
    GLuint vertexShader = createShader(vertex_filename, GL_VERTEX_SHADER);
    GLuint fragmentShader = createShader(fragment_filename, GL_FRAGMENT_SHADER);
    GLuint geometryShader = 0;
    if (geometry_filename)
    {
        geometryShader = createShader(*geometry_filename, GL_GEOMETRY_SHADER);
    }

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    if (geometry_filename)
    {
        glAttachShader(program, geometryShader);
    }

    glLinkProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE)
    {
        printProgramInfoLog(program, "compiled shader program");

        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        throw std::runtime_error("Unable to link shader program.");
    }

    glValidateProgram(program);

    // Always detach shaders after a successful link.
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);

    // Also safe to delete shaders
    // https://gamedev.stackexchange.com/questions/47910/after-a-succesful-gllinkprogram-should-i-delete-detach-my-shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (geometry_filename)
    {
        glDetachShader(program, geometryShader);
        glDeleteShader(geometryShader);
    }

    // std::cout << "Creating ShaderProgram (" << name << ")" << std::endl;

    return std::make_unique<ShaderProgram>(name, ShaderType::COMPUTE, program);
}
}  // namespace rendering
