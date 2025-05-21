#pragma once

#include "Types.h"
#include <memory>

/// Represents a texture in RAM memory (decompressed)
struct TextureBuffer : NoCopy
{
public:
	TextureBuffer(u32 width, u32 height, u8 n_channels);
	TextureBuffer(const char* file);

	/// Copies a single pixel from one buffer to another
	void copy_pixel(TextureBuffer& dst_buffer, u32 src_x, u32 src_y, u32 dst_x, u32 dst_y) const;
	/// Copies a (horizontal line of pixels)
	void copy_consecutive_pixels(TextureBuffer& dest, u32 src_x, u32 src_y, u32 dest_x, u32 dest_y, u32 count) const;
	u8* get_pointer_to_pixel(u32 x, u32 y);

	u8* data_ptr() const;
	u32 get_width() const;
	u32 get_height() const;
	u32 get_n_channels() const;

private:
	std::unique_ptr<u8[]> buffer_data;
	u32 width, height;
	u8 n_channels;
};

/// Represents a texture in VRAM
struct Texture : NoCopy
{
	Texture();
	Texture(const char* file);
	~Texture();

	/// Loads a buffer from memory to GPU
	void store_buffer(const TextureBuffer& buffer);
	void bind_to_tex_unit(u32 unit) const;

	u32 get_width() const;
	u32 get_height() const;

private:
	u32 width, height;
	GLResource id;
};