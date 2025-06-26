#pragma once

#include "Types.h"

#include "glm/glm.hpp"

#include <vector>
#include <functional>
#include <memory>

typedef s32 InputKey;
typedef s32 InputButton;

enum class InputType
{
	Release = 0,
	Press = 1,
	Repeat = 2
};

struct Window;

using KeyListener = std::function<void(InputKey, InputType)>;
using ButtonListener = std::function<void(InputButton, InputType)>;
using MouseMoveListener = std::function<void(f32, f32)>;
using WheelMoveListener = std::function<void(f32, f32)>;

struct InputUser : NoCopy
{
	InputUser(Window* window);
	~InputUser();

	InputUser(InputUser&&) noexcept;
	InputUser& operator=(InputUser&&) noexcept;

	bool is_key_pressed(InputKey key) const;
	bool is_button_pressed(InputButton button) const;
	glm::vec2 get_cursor_pos() const;
	glm::vec2 get_cursor_delta() const;
	glm::vec2 get_wheel_delta() const;


	bool listener_enabled;
	std::unique_ptr<MouseMoveListener> mouse_move_listener;
	std::unique_ptr<WheelMoveListener> wheel_move_listener;
	std::unique_ptr<KeyListener> key_listener;
	std::unique_ptr<ButtonListener> button_listener;
private:
	friend struct Window;

	Window* window;
};

struct KeyState
{
	KeyState(Window& window, InputKey key);

	void update_state();

	bool is_pressed() const;
	bool is_released() const;
	bool is_held() const;

	Window* window;
	InputKey key;
	bool last;
	bool current;
};

struct ButtonState
{
	ButtonState(Window& window, InputButton button);

	void update_state();

	bool is_pressed() const;
	bool is_released() const;
	bool is_held() const;

	Window* window;
	InputButton button;
	bool last;
	bool current;
};

// Key codes are based on GLFW key codes, see: https://www.glfw.org/docs/latest/group__keys.html

/* The unknown key */
const static InputKey KEY_UNKNOWN = -1;

/* Printable keys */
const static InputKey KEY_SPACE = 32;
const static InputKey KEY_APOSTROPHE = 39;  /* ' */
const static InputKey KEY_COMMA = 44;  /* , */
const static InputKey KEY_MINUS = 45;  /* - */
const static InputKey KEY_PERIOD = 46;  /* . */
const static InputKey KEY_SLASH = 47;  /* / */
const static InputKey KEY_0 = 48;
const static InputKey KEY_1 = 49;
const static InputKey KEY_2 = 50;
const static InputKey KEY_3 = 51;
const static InputKey KEY_4 = 52;
const static InputKey KEY_5 = 53;
const static InputKey KEY_6 = 54;
const static InputKey KEY_7 = 55;
const static InputKey KEY_8 = 56;
const static InputKey KEY_9 = 57;
const static InputKey KEY_SEMICOLON = 59;  /* ; */
const static InputKey KEY_EQUAL = 61;  /* = */
const static InputKey KEY_A = 65;
const static InputKey KEY_B = 66;
const static InputKey KEY_C = 67;
const static InputKey KEY_D = 68;
const static InputKey KEY_E = 69;
const static InputKey KEY_F = 70;
const static InputKey KEY_G = 71;
const static InputKey KEY_H = 72;
const static InputKey KEY_I = 73;
const static InputKey KEY_J = 74;
const static InputKey KEY_K = 75;
const static InputKey KEY_L = 76;
const static InputKey KEY_M = 77;
const static InputKey KEY_N = 78;
const static InputKey KEY_O = 79;
const static InputKey KEY_P = 80;
const static InputKey KEY_Q = 81;
const static InputKey KEY_R = 82;
const static InputKey KEY_S = 83;
const static InputKey KEY_T = 84;
const static InputKey KEY_U = 85;
const static InputKey KEY_V = 86;
const static InputKey KEY_W = 87;
const static InputKey KEY_X = 88;
const static InputKey KEY_Y = 89;
const static InputKey KEY_Z = 90;
const static InputKey KEY_LEFT_BRACKET = 91;  /* [ */
const static InputKey KEY_BACKSLASH = 92;  /* \ */
const static InputKey KEY_RIGHT_BRACKET = 93;  /* ] */
const static InputKey KEY_GRAVE_ACCENT = 96;  /* ` */
const static InputKey KEY_WORLD_1 = 161; /* non-US #1 */
const static InputKey KEY_WORLD_2 = 162; /* non-US #2 */

/* Function keys */
const static InputKey KEY_ESCAPE = 256;
const static InputKey KEY_ENTER = 257;
const static InputKey KEY_TAB = 258;
const static InputKey KEY_BACKSPACE = 259;
const static InputKey KEY_INSERT = 260;
const static InputKey KEY_DELETE = 261;
const static InputKey KEY_RIGHT = 262;
const static InputKey KEY_LEFT = 263;
const static InputKey KEY_DOWN = 264;
const static InputKey KEY_UP = 265;
const static InputKey KEY_PAGE_UP = 266;
const static InputKey KEY_PAGE_DOWN = 267;
const static InputKey KEY_HOME = 268;
const static InputKey KEY_END = 269;
const static InputKey KEY_CAPS_LOCK = 280;
const static InputKey KEY_SCROLL_LOCK = 281;
const static InputKey KEY_NUM_LOCK = 282;
const static InputKey KEY_PRINT_SCREEN = 283;
const static InputKey KEY_PAUSE = 284;
const static InputKey KEY_F1 = 290;
const static InputKey KEY_F2 = 291;
const static InputKey KEY_F3 = 292;
const static InputKey KEY_F4 = 293;
const static InputKey KEY_F5 = 294;
const static InputKey KEY_F6 = 295;
const static InputKey KEY_F7 = 296;
const static InputKey KEY_F8 = 297;
const static InputKey KEY_F9 = 298;
const static InputKey KEY_F10 = 299;
const static InputKey KEY_F11 = 300;
const static InputKey KEY_F12 = 301;
const static InputKey KEY_F13 = 302;
const static InputKey KEY_F14 = 303;
const static InputKey KEY_F15 = 304;
const static InputKey KEY_F16 = 305;
const static InputKey KEY_F17 = 306;
const static InputKey KEY_F18 = 307;
const static InputKey KEY_F19 = 308;
const static InputKey KEY_F20 = 309;
const static InputKey KEY_F21 = 310;
const static InputKey KEY_F22 = 311;
const static InputKey KEY_F23 = 312;
const static InputKey KEY_F24 = 313;
const static InputKey KEY_F25 = 314;
const static InputKey KEY_KP_0 = 320;
const static InputKey KEY_KP_1 = 321;
const static InputKey KEY_KP_2 = 322;
const static InputKey KEY_KP_3 = 323;
const static InputKey KEY_KP_4 = 324;
const static InputKey KEY_KP_5 = 325;
const static InputKey KEY_KP_6 = 326;
const static InputKey KEY_KP_7 = 327;
const static InputKey KEY_KP_8 = 328;
const static InputKey KEY_KP_9 = 329;
const static InputKey KEY_KP_DECIMAL = 330;
const static InputKey KEY_KP_DIVIDE = 331;
const static InputKey KEY_KP_MULTIPLY = 332;
const static InputKey KEY_KP_SUBTRACT = 333;
const static InputKey KEY_KP_ADD = 334;
const static InputKey KEY_KP_ENTER = 335;
const static InputKey KEY_KP_EQUAL = 336;
const static InputKey KEY_LEFT_SHIFT = 340;
const static InputKey KEY_LEFT_CONTROL = 341;
const static InputKey KEY_LEFT_ALT = 342;
const static InputKey KEY_LEFT_SUPER = 343;
const static InputKey KEY_RIGHT_SHIFT = 344;
const static InputKey KEY_RIGHT_CONTROL = 345;
const static InputKey KEY_RIGHT_ALT = 346;
const static InputKey KEY_RIGHT_SUPER = 347;
const static InputKey KEY_MENU = 348;

const static InputKey KEY_LAST = KEY_MENU;

/* Mouse buttons */
const static InputButton MOUSE_BUTTON_1 = 0;
const static InputButton MOUSE_BUTTON_2 = 1;
const static InputButton MOUSE_BUTTON_3 = 2;
const static InputButton MOUSE_BUTTON_4 = 3;
const static InputButton MOUSE_BUTTON_5 = 4;
const static InputButton MOUSE_BUTTON_6 = 5;
const static InputButton MOUSE_BUTTON_7 = 6;
const static InputButton MOUSE_BUTTON_8 = 7;
const static InputButton MOUSE_BUTTON_LAST = MOUSE_BUTTON_8;
const static InputButton MOUSE_BUTTON_LEFT = MOUSE_BUTTON_1;
const static InputButton MOUSE_BUTTON_RIGHT = MOUSE_BUTTON_2;
const static InputButton MOUSE_BUTTON_MIDDLE = MOUSE_BUTTON_3;