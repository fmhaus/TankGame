#pragma once

#include "Types.h"

#include "glm/glm.hpp"
#include "GLFW/glfw3.h"

#include <functional>
#include <memory>

enum class FullscreenMode
{
	Windowed, Fullscreen, Borderless
};

struct WindowCreation
{
	u32 width, height;
	const char* title;
	FullscreenMode fullscreen_mode;
	bool vsync;
	bool resizable;
};

struct WindowUserData
{
	glm::vec2 mouse_delta;
	glm::vec2 wheel_delta;
	std::function<void(u32, u32)> on_resize;
};

typedef s32 GLFWKey;
typedef s32 GLFWButton;

struct Window : NoCopy
{
	Window(const WindowCreation& creation_data);
	~Window();

	/// Pools events; Returns if close is requested
	bool poll_events();
	void swap_buffers();

	void set_title(const char* title);
	u32 get_width() const;
	u32 get_height() const;
	f64 get_last_frame_time() const; 
	glm::vec2 get_cursor_pos() const;
	const glm::vec2& get_cursor_delta() const;
	const glm::vec2& get_wheel_delta() const;
	bool is_key_pressed(GLFWKey key) const;
	bool is_mouse_button_pressed(GLFWButton button) const;

	void set_on_resize(std::function<void(u32, u32)> on_resize);

private:
	u32 width, height;
	GLFWwindow* handle;
	std::unique_ptr<WindowUserData> user_data;
	f64 last_time;
	f64 last_frame_time;
};