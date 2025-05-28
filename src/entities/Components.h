#pragma once

#include "engine/Types.h"
#include "engine/Graphics.h"
#include "engine/Asset.h"

#include "entt/entt.hpp"
#include "box2d/box2d.h"
#include "glm/glm.hpp"

/// Component for Box2D Physics for collision detection
struct Physics
{
	Physics(bool dynamic);

	void create_polygon_shape(const b2ShapeDef& shape_def, const b2Polygon& polygon);
	void create_box_shape(const b2ShapeDef& shape_def, f32 width, f32 height, f32 radius = 0.0f);

	bool dynamic;
	b2BodyId body;

	friend struct World;
	
	static entt::entity get_entity(b2BodyId body);
	static bool is_in_contact(entt::registry&, entt::entity e1, entt::entity e2);

private:
	static void update_components(entt::registry& registry);
	static void on_create_physics(entt::registry& registry, b2WorldId world, entt::entity entity);
	static void on_destroy_physics(entt::registry& registry, entt::entity entity);
};

/// Stores position and rotation
struct Transform
{
	glm::vec2 pos;
	f32 rot;

	void update_physics(entt::registry& registry, entt::entity);

	static void set_and_update_physics(entt::registry& registry, entt::entity entity, glm::vec2 pos, f32 rot);
};

/// Stores linear and angular velocity
struct Velocity
{
	glm::vec2 linear;
	f32 angular;

	void update_physics(entt::registry& registry, entt::entity);
	
	static void set_and_update_physics(entt::registry& registry, entt::entity entity, glm::vec2 lin, f32 ang);
};

struct SimpleSpriteRenderable
{
	AssetRef<Texture> texture;
	f32 scale;

	void render(Graphics& graphics, Transform& transform);
};