#include "Mesh.h"

#include <glad/glad.h>
#include <cassert>

void MeshBuilder::push_quad(f32 x, f32 y, f32 size, f32 tex_x, f32 tex_y, f32 tex_size, QuadTransform transform)
{
	this->push_rect(x, y, size, size, tex_x, tex_y, tex_size, tex_size, transform);
}

void MeshBuilder::push_rect(f32 x, f32 y, f32 width, f32 height, f32 tex_x, f32 tex_y, f32 tex_width, f32 tex_height, QuadTransform transform)
{
	f32 tx1 = tex_x + (transform.flip_x ? tex_width : 0.0);
	f32 tx2 = tex_x + (transform.flip_x ? 0.0 : tex_width);
	f32 ty1 = tex_y + (transform.flip_y ? tex_height : 0.0);
	f32 ty2 = tex_y + (transform.flip_y ? 0.0 : tex_height);

	Vertex vertices[4] = {
		Vertex { x, y },
		Vertex { x + width, y },
		Vertex { x + width, y + height },
		Vertex { x, y + height }
	};

	f32 tex_array[4][2] = {
		{ tx1, ty1 },
		{ tx2, ty1 },
		{ tx2, ty2 },
		{ tx1, ty2 }
	};

	assert(transform.rotation < 4);

	for (u32 i = 0; i < 4; i++)
	{
		u32 tex_index = (4 + i - transform.rotation) % 4;
		vertices[i].tx = tex_array[tex_index][0];
		vertices[i].ty = tex_array[tex_index][1];
	}

	const static u32 TRIANGLE_INDICES[6] = { 0, 1, 2, 0, 2, 3 };
	for (u32 i : TRIANGLE_INDICES)
	{
		this->vertices.push_back(vertices[i]);
	}
}

void MeshBuilder::clear()
{
	this->vertices.clear();
}

void MeshBuilder::reserve_size(u32 quad_count)
{
	this->vertices.reserve(quad_count * 4);
}

Mesh::Mesh()
	: vertex_count(0)
{
	glGenVertexArrays(1, &this->vao);
	glGenBuffers(1, &this->vbo);

	glBindVertexArray(this->vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glEnableVertexArrayAttrib(this->vao, 0);
	glEnableVertexArrayAttrib(this->vao, 1);
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &this->vao);
	glDeleteBuffers(1, &this->vbo);
}

void Mesh::load_mesh(MeshBuilder& builder)
{
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, builder.vertices.size() * sizeof(Vertex), (void*)&builder.vertices[0], GL_STATIC_DRAW);
	this->vertex_count = builder.vertices.size();
}

void Mesh::render() const
{
	glBindVertexArray(this->vao);
	glDrawArrays(GL_TRIANGLES, 0, this->vertex_count);
}