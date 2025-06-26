#include "AssetManager.h"

#include "engine/Texture.h"
#include "engine/Font.h"
#include "entities/Tank.h"
#include "entities/Particle.h"

static const char* PROJECTILE_NAMES[8] = { "Grenade_Shell.png", "Heavy_Shell.png", "Laser.png", "Light_Shell.png",
		"Medium_Shell.png", "Plasma.png", "Shotgun_Shells.png", "Sniper_Shell.png" };

template <typename T>
void load_with_location(std::unique_ptr<T>& data_ptr, const std::string& location)
{
	data_ptr = std::make_unique<T>(location.c_str());
}

AssetManager& AssetManager::get_instance()
{
	static AssetManager asset_manager;
	return asset_manager;
}

AssetManager::AssetManager()
{
	font_sans_black.set(std::string(RESOURCES_PATH "font/SansBlack.ttf"), load_with_location<Font>);

	for (u32 var1 = 0; var1 < 4; var1++)
	{
		for (u32 var2 = 0; var2 < 8; var2++)
		{
			hull_textures[var1][var2].set(std::string(RESOURCES_PATH "images/tank/hulls_") + std::to_string(var1 + 1) + "/Hull_0" + std::to_string(var2 + 1) + ".png", load_with_location<Texture>);
			turret_textures[var1][var2].set(std::string(RESOURCES_PATH "images/tank/guns_") + std::to_string(var1 + 1) + "/Gun_0" + std::to_string(var2 + 1) + ".png", load_with_location<Texture>);
		}
		for (u32 var2 = 0; var2 < 2; var2++)
		{
			static const char* var2_str[2] = { "A", "B" };
			track_textures[var1][var2].set(std::string(RESOURCES_PATH "images/tank/tracks/Track_") + std::to_string(var1 + 1) + "_" + var2_str[var2] + ".png", load_with_location<Texture>);
		}
	}

	for (u32 i = 0; i < 8; i++)
	{
		hull_data[i].set(std::string(RESOURCES_PATH "images/tank/hulls_data/Hull_0") + std::to_string(i + 1) + ".txt", [](auto& data_ptr, const auto& location)
			{
				HullData::load_from_file(data_ptr, location.c_str(), PIXEL_SCALE);
			});

		turret_data[i].set(std::string(RESOURCES_PATH "images/tank/guns_data/Gun_0") + std::to_string(i + 1) + ".txt", [](auto& data_ptr, const auto& location)
			{
				TurretData::load_from_file(data_ptr, location.c_str(), PIXEL_SCALE);
			});
	}

	for (u32 i = 0; i < projectile_textures.size(); i++)
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

void AssetManager::preload_assets()
{
	preloader.clear();

	// TODO: only load assets that are needed
	preloader.preload(font_sans_black);
	preloader.preload_array(projectile_textures);

	preloader.preload_array(particle_exhaust);
	preloader.preload_array(particle_explosion);
	preloader.preload(particle_flame);
	preloader.preload_array(particle_flash);
	preloader.preload_array(particle_impact);
	preloader.preload(particle_smoke);
}
