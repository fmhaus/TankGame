#include "Projectile.h"

#include "CollisionCategory.h"
#include "Particle.h"
#include "AssetManager.h"
#include "engine/util/MathUtil.h"
#include <stack>

const static glm::vec2 PROJECTILE_HITBOXES[8] =
{
	{ 0.156f, 0.312f },
	{ 0.156f, 0.484f },
	{ 0.064f, 0.58f },
	{ 0.08f, 0.2f },
	{ 0.126f, 0.3f },
	{ 0.126f, 0.58f },
	{ 0.42f, 0.250f },
	{ 0.08f, 0.4f }
};

static std::stack<entt::entity> remove_projectiles;

Projectile::Projectile(entt::entity entity, entt::entity shooter_entity, const ProjectileType& type)
	: entity(entity), shooter_entity(shooter_entity), fix_orientation(type.fix_orientation), max_collisions(type.max_collisons), collision_count(0), 
	just_spawned(true), in_tank_spawn(false), fix_velocity(type.fix_velocity), initial_velocity(type.velocity), particle_type(type.particle_type)
{
}

entt::entity Projectile::create_projectile(entt::registry& registry, entt::entity shooter_entity, const ProjectileType& type, glm::vec2 pos, f32 rot)
{
	u8 type_index = static_cast<u8>(type.sprite_type);

	glm::vec2 velocity = MathUtil::rotate({ 0.0f, -type.velocity }, rot);

	entt::entity entity = registry.create();
	registry.emplace<Transform>(entity, pos, rot);
	registry.emplace<Velocity>(entity, velocity, 0.0f);

	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.density = type.density;
	shape_def.material.friction = 0.3f;
	shape_def.material.restitution = type.restitution;
	shape_def.enableContactEvents = true;
	shape_def.filter.categoryBits = CATEGORY_PROJECTILE;
	if (!type.allow_projectile_collision)
		shape_def.filter.maskBits &= ~CATEGORY_PROJECTILE;

	b2Polygon hitbox = b2MakeBox(PROJECTILE_HITBOXES[type_index].x * type.scale * 0.5f, PROJECTILE_HITBOXES[type_index].y * type.scale * 0.5f);
	b2ShapeCastInput input = { 0 };

	Physics& physics = registry.emplace<Physics>(entity, true);
	physics.create_polygon_shape(shape_def, hitbox);
	b2Body_SetBullet(physics.body, true);
	b2Body_SetAngularDamping(physics.body, 0.2f);

	// registry.emplace<ProjectileRenderable>(entity, assets.projectile_textures[type_index].loaded(), type.scale);
	Projectile& projectile = registry.emplace<Projectile>(entity, entity, shooter_entity, type);

	return entity;
}

void Projectile::create_projectile_renderable(entt::registry& registry, entt::entity projectile, const ProjectileType& type)
{
	registry.emplace<ProjectileRenderable>(projectile, AssetManager::get_instance().projectile_textures[static_cast<usz>(type.sprite_type)].loaded(), type.scale);
}

void Projectile::update_projectiles(entt::registry& registry)
{
	while (!remove_projectiles.empty())
	{
		entt::entity projectile = remove_projectiles.top();
		remove_projectiles.pop();

		registry.destroy(projectile);
	}

	for (auto [entity, projectile, transform, velocity] : registry.view<Projectile, Transform, Velocity>().each())
	{
		projectile.just_spawned = false;
		if (projectile.fix_orientation)
		{
			transform.rot = std::atan2(velocity.linear.x, -velocity.linear.y);
			transform.update_physics(registry, entity);
			velocity.angular = 0.0f;
			velocity.update_physics(registry, entity);
		}
		if (projectile.fix_velocity)
		{
			velocity.linear = glm::normalize(velocity.linear) * projectile.initial_velocity;
			velocity.update_physics(registry, entity);
		}
	}
}

void Projectile::on_collision_begin(entt::registry& registry, entt::entity projectile_entity, entt::entity other, glm::vec2 pos, glm::vec2 normal)
{
	Projectile& projectile = registry.get<Projectile>(projectile_entity);
	if (projectile.just_spawned && projectile.shooter_entity == other)
	{
		projectile.in_tank_spawn = true;
	}
	else
	{
		projectile.collision_count++;
		if (projectile.collision_count > projectile.max_collisions)
		{
			remove_projectiles.push(projectile_entity);

			if (registry.all_of<ProjectileRenderable>(projectile_entity))
			{
				f32 rot = registry.get<Transform>(projectile_entity).rot;
				Particle::create(registry, AssetManager::get_instance().particle_impact[projectile.particle_type], Transform { pos, rot }, 10.0f, 0.5f);
			}
		}
	}
}

void Projectile::on_collision_end(entt::registry& registry, entt::entity projectile_entity, entt::entity other)
{
	Projectile& projectile = registry.get<Projectile>(projectile_entity);
	if (projectile.in_tank_spawn)
	{
		if (projectile.shooter_entity == other)
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