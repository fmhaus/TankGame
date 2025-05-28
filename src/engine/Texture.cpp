#include "Texture.h"

#include <glad/glad.h>
#include <stb_image/stb_image.h>
#include <cassert>
#include <stdexcept>

TextureBuffer::TextureBuffer(u32 width, u32 height, u8 n_channels)
	: width(width), height(height), n_channels(n_channels), buffer_data(new u8[width * height * n_channels])
{
	assert(n_channels != 0 && n_channels <= 4);
}

TextureBuffer::TextureBuffer(const char* file)
{
	s32 w, h, c;
	buffer_data.reset(stbi_load(file, &w, &h, &c, 0));
	if (!buffer_data.get())
	{
		const char* failure = stbi_failure_reason();
		throw std::runtime_error("Failed to read image " + std::string(file) + ": " + failure);
	}

	width = w;
	height = h;
	n_channels = c;
}

void TextureBuffer::copy_pixel(TextureBuffer& dst_buffer, u32 src_x, u32 src_y, u32 dst_x, u32 dst_y) const
{
	copy_consecutive_pixels(dst_buffer, src_x, src_y, dst_x, dst_y, 1);
}

void TextureBuffer::copy_consecutive_pixels(TextureBuffer& dst_buffer, u32 src_x, u32 src_y, u32 dst_x, u32 dst_y, u32 count) const
{
	assert(n_channels == dst_buffer.n_channels);
	assert(src_x + count <= width);
	assert(src_y < height);
	assert(dst_x + count <= dst_buffer.width);
	assert(dst_y < dst_buffer.height);

	u32 dst_offset = n_channels * (dst_buffer.width * dst_y + dst_x);
	u32 src_offset = n_channels * (width * src_y + src_x);

	memcpy(dst_buffer.buffer_data.get() + dst_offset, this->buffer_data.get() + src_offset, (usz)(count * (u32)n_channels));
}

u8* TextureBuffer::get_pointer_to_pixel(u32 x, u32 y)
{
	return &buffer_data[n_channels * (width * y + x)];
}

u8* TextureBuffer::data_ptr() const
{
	return buffer_data.get();
}

u32 TextureBuffer::get_width() const
{
	return width;
}

u32 TextureBuffer::get_height() const
{
	return height;
}

u32 TextureBuffer::get_n_channels() const
{
	return n_channels;
}

Texture::Texture()
	: width(0), height(0)
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

Texture::Texture(const char* file)
	: Texture()
{
	store_buffer(TextureBuffer(file));
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

void Texture::store_buffer(const TextureBuffer& buffer)
{
	glBindTexture(GL_TEXTURE_2D, id);

	GLint format;
	if (buffer.get_n_channels() == 1)
		format = GL_RED;
	else if (buffer.get_n_channels() == 2)
		format = GL_RG;
	else if (buffer.get_n_channels() == 3)
		format = GL_RGB;
	else if (buffer.get_n_channels() == 4)
		format = GL_RGBA;
	else
		throw;

	glTexImage2D(GL_TEXTURE_2D, 0, format, buffer.get_width(), buffer.get_height(), 0, format, GL_UNSIGNED_BYTE, buffer.data_ptr());

	width = buffer.get_width();
	height = buffer.get_height();
}

void Texture::bind_to_tex_unit(u32 unit) const
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, id);
}

u32 Texture::get_width() const
{
	return width;
}

u32 Texture::get_height() const
{
	return height;
}