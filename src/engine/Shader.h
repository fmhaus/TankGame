#pragma once

#include "Types.h"

#include <glm/glm.hpp>

// TODO: implement more as needed
void load_uniform_impl(s32 location, f32 value);
void load_uniform_impl(s32 location, s32 value);
void load_uniform_impl(s32 location, const glm::mat3x3& value);
void load_uniform_impl(s32 location, const glm::mat4x4& value);
void load_uniform_impl(s32 location, const glm::vec2& value);
void load_uniform_impl(s32 location, const glm::vec3& value);
void load_uniform_impl(s32 location, const glm::vec4& value);

template <typename T>
struct Uniform
{
	Uniform(s32 location) : location(location) {}

	inline void load(T value)
	{
		if (location != -1)
			load_uniform_impl(location, value);
	}

private:
	s32 location;
};

struct Shader : NoCopy
{
	Shader(const char* vertex_file, const char* fragment_file);
	~Shader();
	void use();
	static void use_default();

	template <typename T>
	inline Uniform<T> get_uniform(const char* name)
	{
		return Uniform<T> { get_uniform_location(name) };
	}

private:
	s32 get_uniform_location(const char* name);

	GLResource program;
};
