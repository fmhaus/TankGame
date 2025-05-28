#pragma once

#include "Components.h"
#include "AssetManager.h"
#include "Tank.h"

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
	f32 bounciness = 0.5f;
	f32 density = 1000;
	f32 scale = 1.0f;
	u32 max_collisons = 0;
	bool fix_orientation = false;
};

struct Projectile
{
	Projectile(entt::entity entity, entt::entity shooter, bool fix_rotation, u16 max_collisions);

	entt::entity entity;
	entt::entity shooter;
	bool fix_rotation;
	bool just_spawned;
	bool in_tank_spawn;
	u16 max_collisions;

	u16 collision_count;

	static entt::entity create_projectile(entt::registry& registry, AssetManager& assets, Tank& shooter, ProjectileType type, f32 speed);

	static void update_projectiles(entt::registry& registry);
	static void on_collision_begin(entt::registry&, entt::entity projectile, entt::entity other);
	static void on_collision_end(entt::registry&, entt::entity projectile, entt::entity other);
};

struct ProjectileRenderable : SimpleSpriteRenderable
{
	static void render_projectiles(entt::registry& registry, Graphics& graphics);
};