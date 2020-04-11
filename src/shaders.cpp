#include <vector>
#include <iostream>

#include <GL/glew.h>

#include "shaders.h"

// From computer-graphics.se
char* readFile(const char* file)
{
    FILE* fptr;
    long length;
    char* buf;

    if (file == NULL)
        return NULL;

    fptr = fopen(file, "rb");

    if (!fptr) /* Return NULL on failure */
        return NULL;
    fseek(fptr, 0, SEEK_END);        /* Seek to the end of the file */
    length = ftell(fptr);            /* Find out how many bytes into the file we are */
    buf = (char*)malloc(length + 1); /* Allocate a buffer for the entire length of the file and a
                                        null terminator */
    memset(buf, 0, sizeof(char) * (length + 1)); /* Clean the buffer - suggested for safety by
                                                    Mateusz 2016 */
    fseek(fptr, 0, SEEK_SET);                    /* Go back to the beginning of the file */
    fread(buf, length, 1, fptr); /* Read the contents of the file in to the buffer */
    fclose(fptr);                /* Close the file */
    buf[length] = 0;             /* Null terminator */

    return buf; /* Return the buffer */
}

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

uint compileComputeShader(const std::string& computeSource)
{
    // Create an empty vertex shader handle
    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);

    // Send the compute shader source code to GL
    // Note that std::string's .c_str is NULL character terminated.
    char* src;
    src = readFile(computeSource.c_str());  // does a malloc, but we free below
    glShaderSource(computeShader, 1, &src, NULL);
    free(src);

    // Compile the compute shader
    glCompileShader(computeShader);

    GLint isCompiled = 0;
    glGetShaderiv(computeShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(computeShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(computeShader, maxLength, &maxLength, &infoLog[0]);

        free(src);
        printShaderInfoLog(computeShader, computeSource.c_str());

        // We don't need the shader anymore.
        glDeleteShader(computeShader);

        // Use the infoLog as you see fit.
        throw std::runtime_error("Unable to compile compute shader.");
    }

    GLuint program = glCreateProgram();

    // Attach our shaders to our program
    glAttachShader(program, computeShader);

    // Link our program
    glLinkProgram(program);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(program);
        // Don't leak shaders either.
        glDeleteShader(computeShader);

        // Use the infoLog as you see fit.

        throw std::runtime_error("Unable to link compute shader program.");
    }

    // Always detach shaders after a successful link.
    glDetachShader(program, computeShader);

    std::cout << "Compute Shader Compiled and Linked Successfully." << '\n';
    return program;
}

// From https://www.khronos.org/opengl/wiki/Shader_Compilation
uint compileShaders(const std::string& vertexSource, const std::string& fragmentSource)
{
    // Create an empty vertex shader handle
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Send the vertex shader source code to GL
    // Note that std::string's .c_str is NULL character terminated.
    char* src;
    src = readFile(vertexSource.c_str());  // does a malloc, but we free below
    glShaderSource(vertexShader, 1, &src, NULL);
    free(src);

    // Compile the vertex shader
    glCompileShader(vertexShader);

    GLint isCompiled = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

        free(src);
        printShaderInfoLog(vertexShader, vertexSource.c_str());

        // We don't need the shader anymore.
        glDeleteShader(vertexShader);

        // Use the infoLog as you see fit.
        throw std::runtime_error("Unable to compile vertex shader.");
    }

    // Create an empty fragment shader handle
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Send the fragment shader source code to GL
    // Note that std::string's .c_str is NULL character terminated.

    src = readFile(fragmentSource.c_str());
    glShaderSource(fragmentShader, 1, &src, NULL);
    free(src);

    // Compile the fragment shader
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

        // We don't need the shader anymore.
        glDeleteShader(fragmentShader);
        // Either of them. Don't leak shaders.
        glDeleteShader(vertexShader);

        // Use the infoLog as you see fit.

        throw std::runtime_error("Unable to compile fragment shader.");
    }

    // Vertex and fragment shaders are successfully compiled.
    // Now time to link them together into a program.
    // Get a program object.
    GLuint program = glCreateProgram();

    // Attach our shaders to our program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // Link our program
    glLinkProgram(program);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(program);
        // Don't leak shaders either.
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Use the infoLog as you see fit.

        throw std::runtime_error("Unable to link shader program.");
    }

    // Always detach shaders after a successful link.
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);

    return program;
}
