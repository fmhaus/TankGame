#pragma once

#include "Window.h"
#include "Types.h"
#include "Shader.h"
#include "Mesh.h"
#include "entt/entt.hpp"

struct UINode
{

};

struct QuadMesh : NoCopy
{
	QuadMesh();
	~QuadMesh();

	void draw() const;

	GLResource vao, vbo;
};

struct RectRenderer : NoCopy
{
	RectRenderer();

	void fill_rect(const Window& window, f32 x, f32 y, f32 width, f32 height, const Color& color = { 1.0f, 1.0f, 1.0f, 1.0f }, const Color& outline_color = {0.0f, 0.0f, 0.0f, 1.0f}, f32 outline_width = 0.0f, f32 round_corners = 0.0f);

private:
	QuadMesh quad_mesh;
	Shader shader;
	Uniform<const glm::vec2&> window_size;
	Uniform<const glm::vec4&> transform;
	Uniform<const glm::vec4&> color;
	Uniform<const glm::vec4&> outline_color;
	Uniform<f32> outline_width;
	Uniform<f32> round_corners;
};