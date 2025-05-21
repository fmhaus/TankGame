#pragma once

#include "Components.h"
#include "AssetManager.h"
#include "Tank.h"

#include "entt/entt.hpp"
#include "glm/glm.hpp"

enum class ProjectileType : u8
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

struct Projectile
{
	entt::entity entity;

	static entt::entity create_projectile(entt::registry& registry, AssetManager& assets, Tank& shooter, ProjectileType type, f32 speed);

	static void update_projectiles(entt::registry& registry);
};

struct ProjectileRenderable : SimpleSpriteRenderable
{
	static void render_projectiles(entt::registry& registry, Graphics& graphics);
};