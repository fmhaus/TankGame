#include "Graphics.h"

#include "engine/util/MathUtil.h"

#include <glad/glad.h>

void ImageTransform::translate(f32 x, f32 y)
{
    matrix[2][0] += x * matrix[0][0] + y * matrix[1][0];
    matrix[2][1] += x * matrix[0][1] + y * matrix[1][1];
    matrix[2][2] += x * matrix[0][2] + y * matrix[1][2];
}

void ImageTransform::scale(f32 sx, f32 sy)
{
    matrix[0][0] *= sx;
    matrix[0][1] *= sx;
    matrix[0][2] *= sx;

    matrix[1][0] *= sy;
    matrix[1][1] *= sy;
    matrix[1][2] *= sy;
}

void ImageTransform::rotate(f32 angle_rad)
{
	f32 cos = std::cos(angle_rad);
	f32 sin = std::sin(angle_rad);

	f32 nm00 = matrix[0][0] * cos + matrix[1][0] * sin;
	f32 nm01 = matrix[0][1] * cos + matrix[1][1] * sin;
	f32 nm02 = matrix[0][2] * cos + matrix[1][2] * sin;
	matrix[1][0] = -matrix[0][0] * sin + matrix[1][0] * cos;
	matrix[1][1] = -matrix[0][1] * sin + matrix[1][1] * cos;
	matrix[1][2] = -matrix[0][2] * sin + matrix[1][2] * cos;
	matrix[0][0] = nm00;
	matrix[0][1] = nm01;
	matrix[0][2] = nm02;
}

glm::vec2 ImageTransform::transform_point(glm::vec2 point) const
{
	glm::vec3 transformed = this->matrix * glm::vec3(point.x, point.y, 0.0f);
	return { transformed.x, transformed.y };
}

ImageTransform::ImageTransform(u32 pixel_scale)
	: inv_pixel_scale(1.0f / (f32) pixel_scale),
	matrix(1.0f)
{
}

ImageRenderer::ImageRenderer()
	: shader(RESOURCES_PATH "shaders/image/vert.glsl", RESOURCES_PATH "shaders/image/frag.glsl"),
	uniform_transform(shader.get_uniform<const glm::mat3x3&>("transform")),
	uniform_sampler(shader.get_uniform<s32>("sampler")),
	uniform_opacity(shader.get_uniform<float>("opacity"))
{
	shader.use();
	uniform_sampler.load((s32)0);
	Shader::use_default();
}

void ImageRenderer::render_image(const Mesh& quad_mesh, const Texture& texture, const Camera& camera, f32 pixel_scale, const ImageTransform& transform, f32 opacity)
{
	glEnable(GL_MULTISAMPLE);

	glm::mat3x3 res_matrix = transform.matrix;
	f32 sx = texture.get_width() / pixel_scale;
	f32 sy = texture.get_height() / pixel_scale;

	res_matrix[0][0] *= sx;
	res_matrix[0][1] *= sx;
	res_matrix[0][2] *= sx;

	res_matrix[1][0] *= sy;
	res_matrix[1][1] *= sy;
	res_matrix[1][2] *= sy;

	res_matrix = camera.transform * res_matrix;

	shader.use();
	uniform_transform.load(res_matrix);
	uniform_opacity.load(std::clamp(opacity, 0.0f, 1.0f));
	texture.bind_to_tex_unit(0);

	quad_mesh.render();

	Shader::use_default();

	glDisable(GL_MULTISAMPLE);
}

Graphics::Graphics(u32 width, u32 height, u32 pixel_scale)
	: camera((f32) width, (f32) height),
	f_width((f32) width),
	f_height((f32) height),
	pixel_scale(pixel_scale),
	quad_mesh(),
	image_renderer(),
	text_renderer()
{
	MeshBuilder mesh_builder;
	mesh_builder.reserve_size(1);
	mesh_builder.push_quad(-0.5, -0.5, 1.0, 0.0, 0.0, 1.0, QuadTransform{});
	quad_mesh.load_mesh(mesh_builder);
}

void Graphics::update_window_dimensions(u32 width, u32 height)
{
	this->f_width = (f32)width;
	this->f_height = (f32)height;
	this->camera.window_width = this->f_width;
	this->camera.window_height = this->f_height;
	this->camera.update_matrix();
}

void send_vertex(Camera& camera, f32 x, f32 y)
{
	glm::vec3 transformed = camera.transform * glm::vec3(x, y, 1.0f);
	glVertex2f(transformed.x, transformed.y);
}

void Graphics::draw_polygon(const std::vector<glm::vec2>& points, const Color& color)
{
	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_LINE_LOOP);

	for (auto point : points)
		send_vertex(camera, point.x, point.y);

	glEnd();
}

void Graphics::fill_polygon(const std::vector<glm::vec2>& points, const Color& color)
{
	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_POLYGON);

	for (auto point : points)
		send_vertex(camera, point.x, point.y);

	glEnd();
}

void Graphics::draw_rect(const Rect& rect, const Color& color)
{
	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_LINE_LOOP);
	send_vertex(camera, rect.x, rect.y + rect.height);
	send_vertex(camera, rect.x, rect.y);
	send_vertex(camera, rect.x + rect.width, rect.y);
	send_vertex(camera, rect.x + rect.width, rect.y + rect.height);
	glEnd();
}

void Graphics::fill_rect(const Rect& rect, const Color& color)
{
	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_TRIANGLE_STRIP);
	send_vertex(camera, rect.x, rect.y + rect.height);
	send_vertex(camera, rect.x + rect.width, rect.y + rect.height);
	send_vertex(camera, rect.x, rect.y);
	send_vertex(camera, rect.x + rect.width, rect.y);
	glEnd();
}

const static u32 CIRCLE_SUBDIVISIONS = 50;

void Graphics::draw_circle(const glm::vec2& pos, f32 radius, const Color& color)
{
	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_LINE_LOOP);
	for (u32 i = 0; i < CIRCLE_SUBDIVISIONS; i++)
	{
		f32 angle = 2.0f * MathUtil::PI_32 * i / (f32)CIRCLE_SUBDIVISIONS;
		f32 x = std::sin(angle) * radius;
		f32 y = std::cos(angle) * radius;
		send_vertex(camera, pos.x + x, pos.y + y);
	}
	glEnd();
}

void Graphics::fill_circle(const glm::vec2& pos, f32 radius, const Color& color)
{
	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_POLYGON);
	for (u32 i = 0; i < CIRCLE_SUBDIVISIONS; i++)
	{
		f32 angle = 2.0f * MathUtil::PI_32 * i / (f32)CIRCLE_SUBDIVISIONS;
		f32 x = std::sin(angle) * radius;
		f32 y = std::cos(angle) * radius;
		send_vertex(camera, pos.x + x, pos.y + y);
	}
	glEnd();
	
}

void Graphics::draw_line(const glm::vec2& p1, const glm::vec2& p2, const Color& color)
{
	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_LINES);
	send_vertex(camera, p1.x, p1.y);
	send_vertex(camera, p2.x, p2.y);
	glEnd();
}
