#pragma once

#include "Types.h"

#include <vector>

struct Vertex
{
	f32 x, y, tx, ty;
};

struct QuadTransform
{
	bool flip_x, flip_y;
	u8 rotation;
};

struct MeshBuilder : NoCopy
{
	void push_quad(f32 x, f32 y, f32 size, f32 tex_x, f32 tex_y, f32 tex_size, QuadTransform transform);
	void push_rect(f32 x, f32 y, f32 width, f32 height, f32 tex_x, f32 tex_y, f32 tex_width, f32 tex_height, QuadTransform transform);
	void clear();

	void reserve_size(u32 quad_count);

	std::vector<Vertex> vertices;
};

struct Mesh : NoCopy
{
	Mesh();
	~Mesh();
	void load_mesh(MeshBuilder& builder);
	void render() const;

private:
	GLResource vao;
	GLResource vbo;
	u32 vertex_count;
};