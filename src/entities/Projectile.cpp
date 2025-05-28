#include "Projectile.h"

#include "engine/util/MathUtil.h"
#include <stack>

const static glm::vec2 PROJECTILE_HITBOXES[8] =
{
	{ 0.156f, 0.312f },
	{ 0.156f, 0.484f },
	{ 0.094f, 0.58f },
	{ 0.08f, 0.2f },
	{ 0.126f, 0.3f },
	{ 0.126f, 0.58f },
	{ 0.42f, 0.250f },
	{ 0.08f, 0.4f }
};

static std::stack<entt::entity> remove_projectiles;

Projectile::Projectile(entt::entity entity, entt::entity shooter, bool fix_rotation, u16 max_collisions)
	: entity(entity), shooter(shooter), fix_rotation(fix_rotation), max_collisions(max_collisions), collision_count(0), just_spawned(true), in_tank_spawn(false)
{
}

entt::entity Projectile::create_projectile(entt::registry& registry, AssetManager& assets, Tank& shooter, ProjectileType type, f32 speed)
{
	u8 type_index = static_cast<u8>(type.sprite_type);

	glm::vec2 shoot_point = shooter.get_shoot_point(registry);
	glm::vec2 velocity = MathUtil::rotate({ 0.0f, -speed }, shooter.turret_orientation);

	entt::entity entity = registry.create();
	registry.emplace<Transform>(entity, shoot_point, shooter.turret_orientation);
	registry.emplace<Velocity>(entity, velocity, 0.0f);

	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.density = type.density;
	shape_def.material.friction = 0.3f;
	shape_def.material.restitution = type.bounciness;
	shape_def.enableContactEvents = true;

	b2Polygon hitbox = b2MakeBox(PROJECTILE_HITBOXES[type_index].x * type.scale * 0.5f, PROJECTILE_HITBOXES[type_index].y * type.scale * 0.5f);
	b2ShapeCastInput input = { 0 };

	Physics& physics = registry.emplace<Physics>(entity, true);
	physics.create_polygon_shape(shape_def, hitbox);
	b2Body_SetBullet(physics.body, true);
	b2Body_SetAngularDamping(physics.body, 0.2f);

	registry.emplace<ProjectileRenderable>(entity, assets.projectile_textures[type_index].loaded(), type.scale);
	Projectile& projectile = registry.emplace<Projectile>(entity, entity, shooter.entity, type.fix_orientation, type.max_collisons);

	return entity;
}

void Projectile::update_projectiles(entt::registry& registry)
{
	while (!remove_projectiles.empty())
	{
		registry.destroy(remove_projectiles.top());
		remove_projectiles.pop();
	}

	for (auto [entity, projectile, transform, velocity] : registry.view<Projectile, Transform, Velocity>().each())
	{
		projectile.just_spawned = false;
		if (projectile.fix_rotation)
		{
			transform.rot = std::atan2(velocity.linear.x, -velocity.linear.y);
			transform.update_physics(registry, entity);
			velocity.angular = 0.0f;
			velocity.update_physics(registry, entity);
		}
	}
}

void Projectile::on_collision_begin(entt::registry& registry, entt::entity projectile_entity, entt::entity other)
{
	Projectile& projectile = registry.get<Projectile>(projectile_entity);
	if (projectile.just_spawned && projectile.shooter == other)
	{
		projectile.in_tank_spawn = true;
	}
	else
	{
		projectile.collision_count++;
		if (projectile.collision_count > projectile.max_collisions)
		{
			remove_projectiles.push(projectile_entity);
		}
	}
}

void Projectile::on_collision_end(entt::registry& registry, entt::entity projectile_entity, entt::entity other)
{
	Projectile& projectile = registry.get<Projectile>(projectile_entity);
	if (projectile.in_tank_spawn)
	{
		if (projectile.shooter == other)
		{
			projectile.in_tank_spawn = false;
			return;
		}
	}
}

void ProjectileRenderable::render_projectiles(entt::registry& registry, Graphics& graphics)
{
	for (auto [entity, transform, renderable] : registry.view<Transform, ProjectileRenderable>().each())
	{
		renderable.render(graphics, transform);
	}
}