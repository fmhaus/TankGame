#include "Camera.h"

Camera::Camera(f32 window_width, f32 window_height)
	: center(0.0f), h_scope(10.0), window_width(window_width), window_height(window_height), transform(1)
{
	update_matrix();
}

void Camera::update_matrix()
{
	f32 h_scale = 2.0 / this->h_scope;
	f32 v_scale = h_scale * this->window_width / this->window_height;
	this->transform[0][0] = h_scale;
	this->transform[1][1] = -v_scale;
	this->transform[2][0] = -this->center.x * h_scale;
	this->transform[2][1] = this->center.y * v_scale;
}

void Camera::set_vertical_scope(f32 v_scope)
{
	h_scope = v_scope * this->window_width / this->window_height;
}

f32 Camera::calculate_vertical_scope()
{
	return h_scope * this->window_height / this->window_width;
}

Rect Camera::get_bounding_rect()
{
	f32 v_scope = calculate_vertical_scope();
	return { center.x - h_scope * 0.5f, center.y - v_scope * 0.5f, h_scope, v_scope };
}

glm::vec2 Camera::to_screen_space(glm::vec2 world) const
{
	glm::vec3 world_space = glm::vec3(world.x, world.y, 1.0f);
	glm::vec3 clip_space = transform * world_space;
	return { (clip_space.x + 1.0f) * 0.5f * window_width, (clip_space.y - 1.0f) * -0.5f * window_height };
}

glm::vec2 Camera::to_world_space(glm::vec2 screen) const
{
	glm::vec3 clip_space = glm::vec3(screen.x * 2.0 / window_width - 1.0f, screen.y * -2.0 / window_height + 1.0f, 1.0);
	glm::vec3 world_space = glm::inverse(transform) * clip_space;
	return { world_space.x, world_space.y };
}