#pragma once

#include "engine/Asset.h"
#include "engine/Texture.h"
#include "engine/Font.h"

#include "entities/Tank.h"

struct AssetManager : NoCopy
{
	Asset<Font> font_sans_black;
	Asset<Texture> projectile_textures[8];
	TankAssets tank_assets;

	AssetManager(u32 pixel_scale);
};