#include "Camera.h"

Camera::Camera(f32 aspect_ratio)
	: center(0.0f), h_scope(10.0), aspect_ratio(aspect_ratio), transform(1)
{
	update_matrix();
}

void Camera::update_matrix()
{
	f32 h_scale = 2.0 / this->h_scope;
	f32 v_scale = h_scale * this->aspect_ratio;
	this->transform[0][0] = h_scale;
	this->transform[1][1] = -v_scale;
	this->transform[2][0] = -this->center.x * h_scale;
	this->transform[2][1] = this->center.y * v_scale;
}

void Camera::set_vertical_scope(f32 v_scope)
{
	h_scope = v_scope * aspect_ratio;
}

f32 Camera::calculate_vertical_scope()
{
	return h_scope / aspect_ratio;
}

Rect Camera::get_bounding_rect()
{
	f32 v_scope = calculate_vertical_scope();
	return { center.x - h_scope * 0.5f, center.y - v_scope * 0.5f, h_scope, v_scope };
}
