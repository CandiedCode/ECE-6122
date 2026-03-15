#include "shader.h"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <vector>

// // @brief Load vertex and fragment shaders from files, compile them, and link into a shader program
// // @details Code adapted from https://github.com/opengl-tutorials/ogl/blob/master/common/shader.cpp
// // @param vertex_file_path Path to vertex shader source file
// // @param fragment_file_path Path to fragment shader source file
// // @return OpenGL shader program ID, or 0 on failure
// GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path)
// {

// 	// Create the shaders
// 	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
// 	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

// 	// Read the Vertex Shader code from the file
// 	std::string VertexShaderCode;
// 	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
// 	if(VertexShaderStream.is_open())
//     {
// 		std::stringstream sstr;
// 		sstr << VertexShaderStream.rdbuf();
// 		VertexShaderCode = sstr.str();
// 		VertexShaderStream.close();
// 	}
//     else
//     {
// 		printf("Impossible to open %s. Are you in the right directory? Don't forget to read the FAQ !\n", vertex_file_path);
// 		getchar();
// 		return 0;
// 	}

// 	// Read the Fragment Shader code from the file
// 	std::string FragmentShaderCode;
// 	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
// 	if(FragmentShaderStream.is_open()){
// 		std::stringstream sstr;
// 		sstr << FragmentShaderStream.rdbuf();
// 		FragmentShaderCode = sstr.str();
// 		FragmentShaderStream.close();
// 	}

// 	GLint Result = GL_FALSE;
// 	int InfoLogLength;

// 	// Compile Vertex Shader
// 	printf("Compiling shader : %s\n", vertex_file_path);
// 	char const * VertexSourcePointer = VertexShaderCode.c_str();
// 	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
// 	glCompileShader(VertexShaderID);

// 	// Check Vertex Shader
// 	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
// 	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
// 	if ( InfoLogLength > 0 ){
// 		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
// 		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
// 		printf("%s\n", &VertexShaderErrorMessage[0]);
// 	}

// 	// Compile Fragment Shader
// 	printf("Compiling shader : %s\n", fragment_file_path);
// 	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
// 	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
// 	glCompileShader(FragmentShaderID);

// 	// Check Fragment Shader
// 	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
// 	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
// 	if ( InfoLogLength > 0 ){
// 		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
// 		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
// 		printf("%s\n", &FragmentShaderErrorMessage[0]);
// 	}

// 	// Link the program
// 	printf("Linking program\n");
// 	GLuint ProgramID = glCreateProgram();
// 	glAttachShader(ProgramID, VertexShaderID);
// 	glAttachShader(ProgramID, FragmentShaderID);
// 	glLinkProgram(ProgramID);

// 	// Check the program
// 	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
// 	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
// 	if ( InfoLogLength > 0 ){
// 		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
// 		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
// 		printf("%s\n", &ProgramErrorMessage[0]);
// 	}

// 	// cleanup
// 	glDetachShader(ProgramID, VertexShaderID);
// 	glDetachShader(ProgramID, FragmentShaderID);
// 	glDeleteShader(VertexShaderID);
// 	glDeleteShader(FragmentShaderID);

// 	return ProgramID;
// }

Shader::Shader(const char *vertPath, const char *fragPath)
{
    // Read vertex shader source
    std::string vertexCode;
    std::ifstream vShaderFile(vertPath);
    if (vShaderFile.is_open())
    {
        std::stringstream vShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        vertexCode = vShaderStream.str();
        vShaderFile.close();
    }
    else
    {
        std::cerr << "Failed to open vertex shader: " << vertPath << std::endl;
    }

    // Read fragment shader source
    std::string fragmentCode;
    std::ifstream fShaderFile(fragPath);
    if (fShaderFile.is_open())
    {
        std::stringstream fShaderStream;
        fShaderStream << fShaderFile.rdbuf();
        fragmentCode = fShaderStream.str();
        fShaderFile.close();
    }
    else
    {
        std::cerr << "Failed to open fragment shader: " << fragPath << std::endl;
    }

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    // Compile vertex shader
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    // Check vertex compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
    }

    // Compile fragment shader
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    // Check fragment compile errors
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    }

    // Link program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    // Check link errors
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
    }

    // Delete shaders
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::Use() const
{
    glUseProgram(ID);
}

void Shader::SetMat4(const std::string &name, const glm::mat4 &val) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1)
    {
        std::cerr << "Uniform not found: " << name << std::endl;
    }
    else
    {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
    }
}

void Shader::SetMat3(const std::string &name, const glm::mat3 &val) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::SetVec3(const std::string &name, const glm::vec3 &val) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1)
    {
        std::cerr << "Uniform not found: " << name << std::endl;
    }
    else
    {
        glUniform3f(loc, val.x, val.y, val.z);
    }
}

void Shader::SetFloat(const std::string &name, float val) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1)
    {
        std::cerr << "Uniform not found: " << name << std::endl;
    }
    else
    {
        glUniform1f(loc, val);
    }
}

void Shader::SetInt(const std::string &name, int val) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform1i(loc, val);
}
