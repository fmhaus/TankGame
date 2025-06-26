#pragma once

#include "Types.h"
#include "Input.h"

#include "glm/glm.hpp"

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
	glm::vec2 mouse_delta = { 0.0, 0.0 };
	glm::vec2 wheel_delta = { 0.0, 0.0 };
	std::function<void(u32, u32)> on_resize;
	std::vector<InputUser*> input_users;
};

struct GLFWwindow;

struct Window
{
	static Window& create_window(const WindowCreation& creation_data);
	static Window& get_instance();
	static void destroy_window();

	Window(const WindowCreation& creation_data);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(Window&&) = delete;

	/// Pools events; Returns if close is requested
	bool poll_events();
	void swap_buffers();

	void set_title(const char* title);
	u32 get_width() const;
	u32 get_height() const;
	f64 get_last_frame_time() const;

	glm::vec2 get_cursor_pos() const;
	glm::vec2 get_cursor_delta() const;
	glm::vec2 get_wheel_delta() const;
	bool is_key_pressed(InputKey key) const;
	bool is_button_pressed(InputButton button) const;

	friend InputUser;
	InputUser create_input_user();

	void set_on_resize(std::function<void(u32, u32)> on_resize);

private:
	static std::unique_ptr<Window> s_instance;

	void add_input_user(InputUser* user);
	void remove_input_user(InputUser* user);

	u32 width, height;
	GLFWwindow* handle;
	std::unique_ptr<WindowUserData> user_data;
	f64 last_time;
	f64 last_frame_time;
};