#pragma once

#include "Components.h"

#include "entt/entt.hpp"
#include "glm/glm.hpp"

enum class ProjectileSpriteType : u8
{
	GrenadeShell = 0,
	HeavyShell = 1,
	Laser = 2,
	LightShell = 3,
	MediumShell = 4,
	Plasma = 5,
	ShotgunShells = 6,
	Sniper = 7
};

struct ProjectileType
{
	ProjectileSpriteType sprite_type = ProjectileSpriteType::MediumShell;
	f32 velocity = 10.0f;
	f32 restitution = 0.5f;
	f32 density = 1000;
	f32 scale = 1.0f;
	u16 max_collisons = 0;
	u8 particle_type = 0;
	bool fix_orientation = false;
	bool fix_velocity = true;
};

struct Projectile
{
	Projectile(entt::entity entity, entt::entity shooter_entity, const ProjectileType& type);

	entt::entity entity;
	entt::entity shooter_entity;
	f32 initial_velocity;
	bool fix_orientation;
	bool fix_velocity;
	bool just_spawned;
	bool in_tank_spawn;
	u16 max_collisions;
	u16 collision_count;
	u8 particle_type;

	static entt::entity create_projectile(entt::registry& registry, entt::entity shooter_entity, const ProjectileType& type, glm::vec2 pos, f32 rot);
	static void create_projectile_renderable(entt::registry& registry, entt::entity projectile, const ProjectileType& type);

	static void update_projectiles(entt::registry& registry);
	static void on_collision_begin(entt::registry&, entt::entity projectile, entt::entity other, glm::vec2 pos, glm::vec2 normal);
	static void on_collision_end(entt::registry&, entt::entity projectile, entt::entity other);
};

struct ProjectileRenderable : SimpleSpriteRenderable
{
	static void render_projectiles(entt::registry& registry, Graphics& graphics);
};