#pragma once

#include "Types.h"

#include <glm/glm.hpp>

struct Camera
{
	Camera(f32 window_width, f32 window_height);

	/// Calculates the transform matrix; Need to call this every time after changing center/h_scope/width/height
	void update_matrix();

	/// Sets the camera to a vertical camera scope (height)
	void set_vertical_scope(f32 v_scope);
	/// Calculates the vertical scope of the camera
	f32 calculate_vertical_scope();

	/// Gets the rectangle of the camera bounds in world space
	Rect get_bounding_rect();

	/// Translates world coordinates to screen coordinates
	glm::vec2 to_screen_space(glm::vec2 world) const;
	/// Translates screen coordinates to world coordinates
	glm::vec2 to_world_space(glm::vec2 screen) const;

	/// The center of the camera in world space
	glm::vec2 center;
	/// The horizontal camera scope (width) in world space
	f32 h_scope;

	f32 window_width;
	f32 window_height;

	glm::mat3 transform;
};