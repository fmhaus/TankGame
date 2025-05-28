#include "AssetManager.h"

template <typename T>
void load_with_location(std::unique_ptr<T>& data_ptr, const std::string& location)
{
	data_ptr = std::make_unique<T>(location.c_str());
}

AssetManager::AssetManager(u32 pixel_scale)
	: tank_assets(pixel_scale)
{
	font_sans_black.set(std::string(RESOURCES_PATH "font/SansBlack.ttf"), load_with_location<Font>);


	static const char* PROJECTILE_NAMES[8] = { "Grenade_Shell.png", "Heavy_Shell.png", "Laser.png", "Light_Shell.png",
		"Medium_Shell.png", "Plasma.png", "Shotgun_Shells.png", "Sniper_Shell.png"};

	for (u32 i = 0; i < 8; i++)
	{
		projectile_textures[i].set(std::string(RESOURCES_PATH "images/projectile/") + PROJECTILE_NAMES[i], load_with_location<Texture>);
	}

	particle_exhaust[0].set(RESOURCES_PATH "images/particle/Exhaust_1", Particle::load_textures);
	particle_exhaust[1].set(RESOURCES_PATH "images/particle/Exhaust_2", Particle::load_textures);
	particle_explosion[0].set(RESOURCES_PATH "images/particle/Explosion_1", Particle::load_textures);
	particle_explosion[1].set(RESOURCES_PATH "images/particle/Explosion_2", Particle::load_textures);
	particle_explosion[2].set(RESOURCES_PATH "images/particle/Explosion_3", Particle::load_textures);
	particle_explosion[3].set(RESOURCES_PATH "images/particle/Explosion_4", Particle::load_textures);
	particle_flame.set(RESOURCES_PATH "images/particle/Flame", Particle::load_textures);
	particle_flash[0].set(RESOURCES_PATH "images/particle/Flash_1", Particle::load_textures);
	particle_flash[1].set(RESOURCES_PATH "images/particle/Flash_2", Particle::load_textures);
	particle_impact[0].set(RESOURCES_PATH "images/particle/Shot_Impact_1", Particle::load_textures);
	particle_impact[1].set(RESOURCES_PATH "images/particle/Shot_Impact_2", Particle::load_textures);
	particle_smoke.set(RESOURCES_PATH "images/particle/Smoke", Particle::load_textures);
}
