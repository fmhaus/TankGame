#pragma once

#include "Types.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Asset.h"

#include <string>

enum class VerticalAlign
{
	Top, Middle, Bottom
};

enum class HorizontalAlign
{
	Left, Center, Right
};

/// Settings when building a TextMesh
struct TextBuildSettings
{
	f32 size = 20;
	VerticalAlign vertical_align = VerticalAlign::Middle;
	HorizontalAlign	horizontal_align = HorizontalAlign::Center;
};

/// Settings for rendering text
struct TextStyleSettings
{
	Color color = { 0.0f, 0.0f, 0.0f, 1.0f };
	Color outine_color = { 0.0f, 0.0f, 0.0f, 1.0f };
	f32 thickness = 0.5f;
	f32 sharpness = 0.9f;
	f32 outline_width = 0.0f;
	f32 outline_sharpness = 0.9f;
};

struct CharData
{
	f32 tex_x, tex_y, tex_width, tex_height;
	f32 x_off, y_off, x_advance;
};

struct Font : NoCopy
{
	Font(const char* file);

	friend struct TextMesh;

private:
	const CharData* find_char_data(char c) const;

	Texture texture;
	f32 line_height;
	f32 line_gap;
	CharData* fallback_char;
	std::vector<CharData> chars_data;

	friend struct TextRenderer;
};

/// Stores the Mesh of a text string
struct TextMesh : NoCopy
{
	TextMesh(Asset<Font>& font_asset);

	void load_text(const std::string& text, const TextBuildSettings& settings = TextBuildSettings());

private:
	Mesh mesh;
	AssetRef<Font> font;

	static MeshBuilder builder;

	friend struct TextRenderer;
};

struct TextRenderer : NoCopy
{
	TextRenderer();

	void render_text(const TextMesh& text, const TextStyleSettings& settings, f32 x, f32 y, f32 window_width, f32 window_height);
private:
	Shader shader;
	Uniform<s32> uniform_sampler;
	Uniform<const glm::mat3&> uniform_transform;
	Uniform<const glm::vec4&> uniform_color;
	Uniform<const glm::vec4&> uniform_edges;
	Uniform<const glm::vec4&> uniform_outline_color;
};