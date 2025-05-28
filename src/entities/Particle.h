#pragma once

#include "Components.h"
#include "engine/Asset.h"
#include "entt/entt.hpp"

struct ParticleTextures
{
	ParticleTextures(u32 count);

	std::unique_ptr<Texture[]> textures;
	u32 texture_count;
};

struct Particle
{
	Particle(Asset<ParticleTextures>& asset, f32 scale, f32 frames_per_second);

	AssetRef<ParticleTextures> textures_asset;
	f32 scale;
	f32 frames_per_second;
	f32 animation_time;

	static entt::entity create(entt::registry& registry, Asset<ParticleTextures>& asset, Transform transform, f32 frames_per_second, f32 scale = 1.0f);
	static void load_textures(std::unique_ptr<ParticleTextures>& data_ptr, const std::string& location);
	static void update_animations(entt::registry& registry, f32 delta_time);
	static void render_particles(entt::registry& registry, Graphics& graphics);

};