#pragma once

#include "Types.h"

#include <glm/glm.hpp>

struct Camera
{
	Camera(f32 aspect_ratio);

	/// Calculates the transform matrix; Need to call this every time after changing center/h_scope/aspect_ratio
	void update_matrix();

	/// Sets the camera to a vertical camera scope (height)
	void set_vertical_scope(f32 v_scope);
	/// Calculates the vertical scope of the camera
	f32 calculate_vertical_scope();

	/// Gets the rectangle of the camera bounds in world space
	Rect get_bounding_rect();

	/// The center of the camera in world space
	glm::vec2 center;
	/// The horizontal camera scope (width) in world space
	f32 h_scope;
	/// window_width / window_height
	f32 aspect_ratio;

	glm::mat3 transform;
};