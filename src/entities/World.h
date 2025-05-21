#pragma once

#include "entt/entt.hpp"
#include "box2d/box2d.h"

#include "engine/Types.h"
#include "engine/Graphics.h"

struct World : NoCopy 
{
	World();
	~World();

	void update(f32 delta_time);
	void render(Graphics& graphics);

	entt::registry registry;
	b2WorldId physics_world;

	void draw_physics_debug(Graphics& graphics);
	void set_physics_debug_draw_enabled(bool enabled);

private:
	void on_create_physics(entt::registry& registry, entt::entity entity);
	void on_destroy_physics(entt::registry& registry, entt::entity entity);

	std::unique_ptr<b2DebugDraw> physics_debug_draw;
};