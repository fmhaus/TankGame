#pragma once

#include "engine/Asset.h"

const static u32 PIXEL_SCALE = 128;

struct Font;
struct Texture;
struct HullData;
struct TurretData;
struct ParticleTextures;

struct AssetManager : NoCopy
{
	static AssetManager& get_instance();

	AssetManager();

	Asset<Font> font_sans_black;
	Asset<Texture> projectile_textures[8];

	Asset<Texture> hull_textures[4][8];
	Asset<Texture> turret_textures[4][8];
	Asset<Texture> track_textures[4][2];

	Asset<HullData> hull_data[8];
	Asset<TurretData> turret_data[8];

	Asset<ParticleTextures> particle_exhaust[2];
	Asset<ParticleTextures> particle_explosion[4];
	Asset<ParticleTextures> particle_flame;
	Asset<ParticleTextures> particle_flash[2];
	Asset<ParticleTextures> particle_impact[2];
	Asset<ParticleTextures> particle_smoke;
};