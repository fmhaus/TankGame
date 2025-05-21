#pragma once

#include "Types.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Camera.h"
#include "Font.h"

#include "glm/glm.hpp"

/// Used to translate/scale/rotate images in the world
struct ImageTransform {

public:
	/// Translates in world coordinates
	void translate(f32 x, f32 y);
	/// Translates the image in pixel coordinates instead of world coordinates
	void translate_pixels(f32 x, f32 y) { translate(x * inv_pixel_scale, y * inv_pixel_scale); }

	void scale(f32 sx, f32 sy);
	void scale(f32 s) { scale(s, s); }

	/// clockwise is positive
	void rotate(f32 angle_rad);
	/// flip over x axis; horizontal flip
	void flip_x() { scale(-1.0f, 1.0f); }
	/// flip over y axis; vertical flip
	void flip_y() { scale(1.0f, -1.0f); }


	glm::vec2 transform_point(glm::vec2 point) const;

private:
	ImageTransform(u32 pixel_scale);

	glm::mat3x3 matrix;
	f32 inv_pixel_scale;

	friend struct Graphics;
	friend struct ImageRenderer;
};

struct ImageRenderer
{
	ImageRenderer();

	void render_image(const Mesh& quad_mesh, const Texture& texture, const Camera& camera, f32 pixel_scale, const ImageTransform& transform, f32 opacity);

private:
	Shader shader;
	Uniform<const glm::mat3x3&> uniform_transform;
	Uniform<s32> uniform_sampler;
	Uniform<float> uniform_opacity;
};

struct Graphics {

	Graphics(u32 width, u32 height, u32 pixel_scale);

	Camera camera;

	/// Needs to be called whenever the window dimensions change
	void update_window_dimensions(u32 width, u32 height);

	ImageTransform create_transform() { return ImageTransform(pixel_scale); }

	/// Translates world coordinates to screen coordinates
	glm::vec2 to_screen_space(glm::vec2 world) const;
	/// Translates screen coordinates to world coordinates
	glm::vec2 to_world_space(glm::vec2 screen) const;

	inline void draw_image(const Texture& texture, const ImageTransform& transform, f32 opacity) 
	{ 
		image_renderer.render_image(quad_mesh, texture, camera, (f32)pixel_scale, transform, opacity);
	}

	inline void draw_image(const Texture& texture, const ImageTransform& transform)
	{
		image_renderer.render_image(quad_mesh, texture, camera, (f32)pixel_scale, transform, 1.0f);
	}

	inline void draw_text(const TextMesh& text, f32 x, f32 y, const TextStyleSettings& settings = TextStyleSettings())
	{
		text_renderer.render_text(text, settings, x, y, f_width, f_height);
	}

	// OpenGL immediate draws; better used only for debug
	void draw_polygon(const std::vector<glm::vec2>& points, const Color& color);
	void fill_polygon(const std::vector<glm::vec2>& points, const Color& color);
	void draw_rect(const Rect& rect, const Color& color);
	void fill_rect(const Rect& rect, const Color& color);
	void draw_circle(const glm::vec2& pos, f32 radius, const Color& color);
	void fill_circle(const glm::vec2& pos, f32 radius, const Color& color);
	void draw_line(const glm::vec2& p1, const glm::vec2& p2, const Color& color);
private:
	f32 f_width, f_height;
	Mesh quad_mesh;
	u32 pixel_scale;

	ImageRenderer image_renderer;
	TextRenderer text_renderer;
};