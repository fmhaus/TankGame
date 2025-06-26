#include "UI.h"

#include "glad/glad.h"

QuadMesh::QuadMesh()
{
	glGenVertexArrays(1, &this->vao);
	glGenBuffers(1, &this->vbo);

	glBindVertexArray(this->vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	
	const static f32 quad_vertices[12] = {
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.5f,  0.5f,
		-0.5f, -0.5f,
		0.5f,  0.5f,
		-0.5f,  0.5f 
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), (void*)quad_vertices, GL_STATIC_DRAW);
	glEnableVertexArrayAttrib(this->vao, 0);
}

QuadMesh::~QuadMesh()
{
	glDeleteVertexArrays(1, &this->vao);
	glDeleteBuffers(1, &this->vbo);
}

void QuadMesh::draw() const
{
	glBindVertexArray(this->vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

RectRenderer::RectRenderer()
	: quad_mesh(),
	shader(RESOURCES_PATH "shaders/rect/vertex.glsl", RESOURCES_PATH "shaders/rect/fragment.glsl"),
	window_size(shader.get_uniform<const glm::vec2&>("window_size")),
	transform(shader.get_uniform<const glm::vec4&>("transform")),
	color(shader.get_uniform<const glm::vec4&>("color")),
	outline_color(shader.get_uniform<const glm::vec4&>("outline_color")),
	outline_width(shader.get_uniform<f32>("outline_width")),
	round_corners(shader.get_uniform<f32>("round_corners"))
{
	
}

void RectRenderer::fill_rect(const Window& window, f32 x, f32 y, f32 width, f32 height, const Color& color, const Color& outline_color, f32 outline_width, f32 round_corners)
{
	this->shader.use();
	this->window_size.load(glm::vec2(window.get_width(), window.get_height()));

	this->transform.load({ x, y, width, height });
	this->color.load(color.to_vec());
	this->outline_color.load(outline_color.to_vec());
	this->outline_width.load(outline_width);
	this->round_corners.load(round_corners);
	this->quad_mesh.draw();

	Shader::use_default();
}