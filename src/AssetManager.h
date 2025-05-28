#pragma once

#include "engine/Asset.h"
#include "engine/Texture.h"
#include "engine/Font.h"

#include "entities/Tank.h"
#include "entities/Particle.h"

struct AssetManager : NoCopy
{
	Asset<Font> font_sans_black;
	Asset<Texture> projectile_textures[8];
	TankAssets tank_assets;
	Asset<ParticleTextures> particle_exhaust[2];
	Asset<ParticleTextures> particle_explosion[4];
	Asset<ParticleTextures> particle_flame;
	Asset<ParticleTextures> particle_flash[2];
	Asset<ParticleTextures> particle_impact[2];
	Asset<ParticleTextures> particle_smoke;

	AssetManager(u32 pixel_scale);
};