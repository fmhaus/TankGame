#include "Projectile.h"

#include "engine/util/MathUtil.h"

const static f32 PROJECTILE_SCALE = 1.0f;

const static glm::vec2 PROJECTILE_HITBOXES[8] =
{
	{ 0.078f, 0.156f },
	{ 0.078f, 0.242f },
	{ 0.047f, 0.29f },
	{ 0.04f, 0.1f },
	{ 0.063f, 0.15f },
	{ 0.063f, 0.29f },
	{ 0.21f, 0.125f },
	{ 0.04f, 0.2f }
};

#include <iostream>
entt::entity Projectile::create_projectile(entt::registry& registry, AssetManager& assets, Tank& shooter, ProjectileType type, f32 speed)
{
	u8 type_index = static_cast<u8>(type);

	glm::vec2 shoot_point = shooter.get_shoot_point(registry);
	glm::vec2 velocity = MathUtil::rotate({ 0.0f, -speed }, shooter.turret_orientation);

	entt::entity projectile = registry.create();
	registry.emplace<Transform>(projectile, shoot_point, shooter.turret_orientation);
	registry.emplace<Velocity>(projectile, velocity, 0.0f);

	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.density = 20000;	// TODO: test densities, varying densitites would also be fun
	// shape_def.filter.groupIndex = - shooter.id;
	shape_def.material.friction = 0.3f;
	shape_def.material.restitution = 0.5f;
	registry.emplace<Physics>(projectile, true).create_box_shape(shape_def,
		PROJECTILE_HITBOXES[type_index].x * PROJECTILE_SCALE,
		PROJECTILE_HITBOXES[type_index].y * PROJECTILE_SCALE,
		0.0f);

	registry.emplace<ProjectileRenderable>(projectile, assets.projectile_textures[type_index].loaded(), PROJECTILE_SCALE);
	registry.emplace<Projectile>(projectile, projectile);

	return projectile;
}

void Projectile::update_projectiles(entt::registry& registry)
{
	for (auto [entity, projectile, transform, velocity] : registry.view<Projectile, Transform, Velocity>().each())
	{
		/*
		transform.rot = std::atan2(velocity.linear.x, -velocity.linear.y);
		transform.update_physics(registry, entity);
		velocity.angular = 0.0f;
		velocity.update_physics(registry, entity);
		*/
	}
}

void ProjectileRenderable::render_projectiles(entt::registry& registry, Graphics& graphics)
{
	for (auto [entity, transform, renderable] : registry.view<Transform, ProjectileRenderable>().each())
	{
		renderable.render(graphics, transform);
	}
}