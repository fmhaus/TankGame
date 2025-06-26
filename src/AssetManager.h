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

	void preload_assets();

	Asset<Font> font_sans_black;
	Array<Asset<Texture>, 8> projectile_textures;

	Array2D<Asset<Texture>, 4, 8> hull_textures;
	Array2D<Asset<Texture>, 4, 8> turret_textures;
	Array2D<Asset<Texture>, 4, 2> track_textures;

	Array<Asset<HullData>, 8> hull_data;
	Array<Asset<TurretData>, 8> turret_data;

	Array<Asset<ParticleTextures>, 2> particle_exhaust;
	Array<Asset<ParticleTextures>, 4> particle_explosion;
	Asset<ParticleTextures> particle_flame;
	Array<Asset<ParticleTextures>, 2> particle_flash;
	Array<Asset<ParticleTextures>, 2> particle_impact;
	Asset<ParticleTextures> particle_smoke;

	AssetPreloader preloader;
};