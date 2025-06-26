#pragma once

#include "Projectile.h"

#include "engine/Graphics.h"
#include "engine/Asset.h"
#include "engine/Input.h"

#include "AssetManager.h"

#include "entt/entt.hpp"

struct HullData
{
	f32 scale;
	f32 turret_pivot_y;
	f32 tracks_scale;
	f32 tracks_off_x;
	f32 tracks_off_y;

	static void load_from_file(std::unique_ptr<HullData>& data_ptr, const char* location, u32 pixel_scale);
};

struct TurretData
{
	f32 scale;
	f32 pivot_y;
	std::vector<glm::vec2> barrel_points;

	static void load_from_file(std::unique_ptr<TurretData>& data_ptr, const char* location, u32 pixel_scale);
};

struct TankDesign
{
	u8 color;
	u8 hull;
	u8 turret;
	u8 tracks;
};

struct Tank
{
	Tank(u32 id, entt::entity entity, const TankDesign& design);

	glm::vec2 get_shoot_point(entt::registry& registry);
	entt::entity shoot_projectile(entt::registry& registry, const ProjectileType& type);

	u32 id;
	entt::entity entity;
	f32 turret_orientation;
	u8 shoot_barrel_index;
	AssetRef<HullData> hull_data;
	AssetRef<TurretData> turret_data;
	TankDesign design;
	// team data, stats, weapon data, name, ...


	static entt::entity create_tank(entt::registry& registry, const TankDesign& design, glm::vec2 pos, bool player_control);
	static void update_tank_design(entt::registry& registry, entt::entity tank_entity, const TankDesign& new_design);

	static f32 get_scale();
};

struct TankRenderable 
{
	TankRenderable(const TankDesign& design);

	AssetRef<Texture> hull_texture;
	AssetRef<Texture> turret_texture;
	AssetRef<Texture> track_textures[2];
	f32 turret_rotation;
	f32 track_animation_1, track_animation_2;

	static void update_track_animation(entt::registry& registry, f32 frame_time);
	static void render_tanks(entt::registry& registry, Graphics& graphics);
};

struct TankMovementSettings
{
	f32 max_speed;
	f32 acceleration_force;
	f32 braking_force;
	f32 max_turning_speed;
	f32 turning_torque;
	f32 gun_rotation_speed;
};

struct TankPlayerController
{
	TankPlayerController(const TankMovementSettings& settings = { 2.0f, 2.0f, 8.0f, glm::radians(80.0f), glm::radians(200.0f), 5.0f });

	TankMovementSettings movement_settings;
	ProjectileType projectile_type;
	f32 rel_turret_rotation;
	InputUser input_user;
	KeyState shoot_key_state;

	static void update_tank(entt::registry& registry, const Camera& camera, f32 frame_time);
};