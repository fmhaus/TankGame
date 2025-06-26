#include "Input.h"

#include "Window.h"

InputUser::InputUser(Window* window)
	: window(window),
	listener_enabled(true),
	mouse_move_listener(nullptr),
	wheel_move_listener(nullptr),
	key_listener(nullptr),
	button_listener(nullptr)
{
	window->add_input_user(this);
}

InputUser::~InputUser()
{
	window->remove_input_user(this);
}

InputUser::InputUser(InputUser&& other) noexcept
{
	window->remove_input_user(&other);
	window->add_input_user(this);
}

InputUser& InputUser::operator=(InputUser&& other) noexcept
{
	window->remove_input_user(&other);
	return *this;
}

bool InputUser::is_key_pressed(InputKey key) const
{
	return window->is_key_pressed(key);
}

bool InputUser::is_button_pressed(InputButton button) const
{
	return window->is_button_pressed(button);
}

glm::vec2 InputUser::get_cursor_pos() const
{
	return window->get_cursor_pos();
}

glm::vec2 InputUser::get_cursor_delta() const
{
	return window->get_cursor_delta();
}

glm::vec2 InputUser::get_wheel_delta() const
{
	return window->get_wheel_delta();
}

KeyState::KeyState(Window& window, InputKey key)
	: window(&window), key(key), last(false), current(false)
{
}

void KeyState::update_state()
{
	last = current;
	current = window->is_key_pressed(key);
}

bool KeyState::is_pressed() const
{
	return !last && current;
}

bool KeyState::is_released() const
{
	return last && !current;
}

bool KeyState::is_held() const
{
	return last && current;
}

ButtonState::ButtonState(Window& window, InputButton button)
	: window(&window), button(button), last(false), current(false)
{
}

void ButtonState::update_state()
{
	last = current;
	current = window->is_button_pressed(button);
}

bool ButtonState::is_pressed() const
{
	return !last && current;
}

bool ButtonState::is_released() const
{
	return last && !current;
}

bool ButtonState::is_held() const
{
	return last && current;
}