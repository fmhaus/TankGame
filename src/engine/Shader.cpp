#include "Shader.h"

#include <glad/glad.h>
#include <stdexcept>
#include <fstream>
#include <iostream>

void load_uniform(s32 location, f32 value)
{
	glUniform1f(location, value);
}

void load_uniform(s32 location, s32 value)
{
	glUniform1i(location, value);
}

void load_uniform(s32 location, const glm::mat3x3& value)
{
	glUniformMatrix3fv(location, 1, GL_FALSE, (GLfloat*)&value);
}

void load_uniform(s32 location, const glm::vec4& value)
{
	glUniform4f(location, value.x, value.y, value.z, value.w);
}

u32 create_shader(GLenum type, const char* file)
{
	std::ifstream f(file);

	if (!f.is_open())
		throw std::runtime_error("Failed to read file: " + std::string(file));

	std::string str;

	f.seekg(0, std::ios::end);
	str.reserve(f.tellg());
	f.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

	s32 str_len = str.length();
	const char* str_ptr = str.c_str();

	u32 shader = glCreateShader(type);
	glShaderSource(shader, 1, &str_ptr, &str_len);
	glCompileShader(shader);
	
	s32 success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		char info_log[512];
		glGetShaderInfoLog(shader, 512, nullptr, info_log);
		throw std::runtime_error("Failed to compile shader " + std::string(file) + ":\n" + info_log);
	}

	return shader;
}

Shader::Shader(const char* vertex_file, const char* fragment_file)
{
	program = glCreateProgram();

	u32 vertex = create_shader(GL_VERTEX_SHADER, vertex_file);
	u32 fragment = create_shader(GL_FRAGMENT_SHADER, fragment_file);

	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	s32 success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		char buffer[512];
		glGetProgramInfoLog(program, 512, nullptr, buffer);
		throw std::runtime_error("Failed to link shader " + std::string(vertex_file) + ", " + fragment_file + ":\n" + buffer);
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader()
{
	glDeleteProgram(this->program);
}

void Shader::use()
{
	glUseProgram(this->program);
}

void Shader::use_default()
{
	glUseProgram(0);
}

s32 Shader::get_uniform_location(const char* name)
{
	s32 location = glGetUniformLocation(this->program, name);
	if (location == -1)
		std::cerr << "Uniform " << name << " not found!" << std::endl;
	return location;
}
