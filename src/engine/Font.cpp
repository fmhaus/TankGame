#include "Font.h"

#include "util/FileUtil.h"
#include "util/MathUtil.h"

#include "stb_truetype/stb_truetype.h"
#include "glad/glad.h"

// DeB
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"
#include <iostream>

const char* CHARACTERS = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
const u32 CHARACTERS_COUNT = 95;
const f32 FONT_RESOLUTION = 64.0f;
const f32 SDF_PIXEL_DECREASE = 0.3f;
const u32 PADDING = 8;

Font::Font(const char* file)
	: fallback_char(nullptr)
{
	u64 file_size;
	auto file_buffer = FileUtil::load_file(file, file_size);

	stbtt_fontinfo info;
	stbtt_InitFont(&info, file_buffer.get(), 0);

	f32 scale = stbtt_ScaleForPixelHeight(&info, FONT_RESOLUTION);
	s32 ascent, descent, line_gap;
	stbtt_GetFontVMetrics(&info, &ascent, &descent, &line_gap);
	this->line_height = (ascent + descent) * scale;
	this->line_gap = line_gap * scale;

	u32 max_width = 0, max_height = 0;

	for (u32 i = 0; i < CHARACTERS_COUNT; i++)
	{
		s32 x1, y1, x2, y2;
		stbtt_GetCodepointBitmapBox(&info, CHARACTERS[i], scale, scale, &x1, &y1, &x2, &y2);

		u32 width = (x2 - x1) + 2 * PADDING;
		u32 height = (y2 - y1) + 2 * PADDING;

		if (width > max_width)
			max_width = width;
		if (height > max_height)
			max_height = height;
	}

	// This can be optimized to not use any pow2 sizes and pack sdf closer together
	// right now there is a low 

	u32 v_images = 0;
	u32 best_area = (u32) - 1;

	for (u32 i = 1; i < CHARACTERS_COUNT; i++)
	{
		u32 tex_height = MathUtil::get_bounding_pow2(i * max_height);
		u32 tex_width = MathUtil::get_bounding_pow2(MathUtil::divide_round_up(CHARACTERS_COUNT, i) * max_width);
		u32 area = tex_width * tex_height - (std::abs((s32) tex_height - (s32) tex_width));
		if (area < best_area)
		{
			best_area = area;
			v_images = i;
		}
	}

	u32 h_images = MathUtil::divide_round_up(CHARACTERS_COUNT, v_images);
	s32 tex_width = MathUtil::get_bounding_pow2(h_images * max_width);
	s32 tex_height = MathUtil::get_bounding_pow2(v_images * max_height);

	TextureBuffer buffer(tex_width, tex_height, 1);

	for (u32 i = 0; i < CHARACTERS_COUNT; i++)
	{
		s32 x_advance, left_side_bearing;
		stbtt_GetCodepointHMetrics(&info, CHARACTERS[i], &x_advance, &left_side_bearing);

		s32 x1, y1, x2, y2;
		stbtt_GetCodepointBitmapBox(&info, CHARACTERS[i], scale, scale, &x1, &y1, &x2, &y2);

		s32 tx = (i % h_images) * max_width;
		s32 ty = (i / h_images) * max_height;

		static const f32 pixel_dist_scale = 128.0f / (FONT_RESOLUTION * SDF_PIXEL_DECREASE);
		s32 sdf_width = 0, sdf_height = 0;
		u8* sdf = stbtt_GetCodepointSDF(&info, scale, CHARACTERS[i], PADDING, 0x7F, pixel_dist_scale, &sdf_width, &sdf_height, nullptr, nullptr);

		if (sdf)
		{
			for (u32 y = 0; y < sdf_height; y++)
				memcpy(buffer.get_pointer_to_pixel(tx, ty + y), &sdf[y * sdf_width], sdf_width);

			stbtt_FreeSDF(sdf, info.userdata);
		}

		this->chars_data.push_back(CharData {
			tx / (f32) tex_width,
			ty / (f32) tex_height,
			sdf_width / (f32) tex_width,
			sdf_height / (f32) tex_height,
			left_side_bearing * scale,
			(f32) y1 + descent * scale,
			x_advance * scale
			});

		if (CHARACTERS[i] == '?')
			this->fallback_char = &this->chars_data[this->chars_data.size() - 1];
	}

	this->texture.store_buffer(buffer);

	// stbi_write_png(RESOURCES_PATH "out1.png", tex_width, tex_height, 1, buffer.buffer_data.get(), tex_width);
}

const CharData* Font::find_char_data(char c) const
{
	// do binary search
	u32 left = 0;
	u32 right = CHARACTERS_COUNT - 1;
	while (left <= right)
	{
		u32 mid = (left + right) / 2;
		if (CHARACTERS[mid] == c)
		{
			return &this->chars_data[mid];
		}
		else if (CHARACTERS[mid] < c)
		{
			left = mid + 1;
		}
		else
		{
			right = mid - 1;
		}
	}

	std::cerr << "[FontBuilder] Character '" << c << "' not found" << std::endl;
	return this->fallback_char;
}

MeshBuilder TextMesh::builder;

TextMesh::TextMesh(Asset<Font>& font_asset)
	: font(font_asset.loaded())
{
}

void TextMesh::load_text(const std::string& text, const TextBuildSettings& settings)
{
	builder.clear();

	f32 x = 0.0f, y = 0.0f;

	f32 scale = std::max(1.0f, settings.size) / (FONT_RESOLUTION);

	f32 total_width = 0.0f;
	for (char c : text)
	{
		auto data = font.get().find_char_data(c);
		if (data)
			total_width += data->x_advance;
	}
	total_width *= scale;

	if (settings.horizontal_align == HorizontalAlign::Center)
		x -= total_width / 2.0f;
	else if (settings.horizontal_align == HorizontalAlign::Right)
		x -= total_width;

	if (settings.vertical_align == VerticalAlign::Middle)
		y += font.get().line_height * scale / 2.0f;
	else if (settings.vertical_align == VerticalAlign::Top)
		y += font.get().line_height * scale;

	for (char c : text)
	{
		auto data = font.get().find_char_data(c);
		if (!data)
			continue;

		x += data->x_off * scale;
		y += data->y_off * scale;

		f32 rect_width = font.get().texture.get_width() * data->tex_width * scale;
		f32 rect_height = font.get().texture.get_height() * data->tex_height * scale;

		builder.push_rect(x, y,
			font.get().texture.get_width() * data->tex_width * scale,
			font.get().texture.get_height() * data->tex_height * scale,
			data->tex_x, data->tex_y, data->tex_width, data->tex_height, {});

		x += (data->x_advance - data->x_off) * scale;
		y += -data->y_off * scale;
	}

	this->mesh.load_mesh(builder);
}

TextRenderer::TextRenderer()
	: shader(RESOURCES_PATH "shaders/font/vert.glsl", RESOURCES_PATH "shaders/font/frag.glsl"),
	uniform_sampler(shader.get_uniform<s32>("sampler")),
	uniform_transform(shader.get_uniform<const glm::mat3&>("transform")),
	uniform_color(shader.get_uniform<const glm::vec4&>("color")),
	uniform_edges(shader.get_uniform<const glm::vec4&>("edges")),
	uniform_outline_color(shader.get_uniform<const glm::vec4&>("outlineColor"))
{
	shader.use();
	uniform_sampler.load(0);
	Shader::use_default();
}

void TextRenderer::render_text(const TextMesh& text, const TextStyleSettings& settings, f32 x, f32 y, f32 window_width, f32 window_height)
{
	glEnable(GL_MULTISAMPLE);

	glm::mat3 transform(1);
	transform[0][0] = 2.0 / window_width;
	transform[1][1] = -2.0 / window_height;
	transform[2][0] = 2.0 * x / window_width - 1.0;
	transform[2][1] = -2.0 * y / window_height + 1.0;

	text.font.get().texture.bind_to_tex_unit(0);

	f32 text_border = std::clamp(settings.thickness + settings.outline_width, 0.0f, 1.0f);
	f32 text_border_width = 1.0f - std::clamp(settings.sharpness, 0.0f, 1.0f);
	f32 outline_border = std::clamp(settings.thickness, 0.0f, 1.0f);
	f32 outline_border_width = 1.0f - std::clamp(settings.outline_sharpness, 0.0f, 1.0f);

	shader.use();
	uniform_transform.load(transform);
	uniform_color.load(settings.color.to_vec());
	uniform_outline_color.load(settings.outine_color.to_vec());
	uniform_edges.load({
		text_border - text_border_width,
		text_border,
		outline_border,
		outline_border + outline_border_width
		});
	
	text.mesh.render();

	Shader::use_default();

	glDisable(GL_MULTISAMPLE);
}