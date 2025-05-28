#include "Particle.h"

#include <filesystem>

static const u32 MAX_TEXTURES = 256;

ParticleTextures::ParticleTextures(u32 count)
	: textures(std::make_unique<Texture[]>(count)), texture_count(count)
{
}


Particle::Particle(Asset<ParticleTextures>& asset, f32 scale, f32 frames_per_second)
	: textures_asset(asset.loaded()), scale(scale), frames_per_second(frames_per_second), animation_time(0.0f)
{
}

entt::entity Particle::create(entt::registry& registry, Asset<ParticleTextures>& asset, Transform transform, f32 frames_per_second, f32 scale)
{
	entt::entity entity = registry.create();
	registry.emplace<Transform>(entity, transform);
	registry.emplace<Particle>(entity, asset, scale, frames_per_second);

	return entity;
}

void Particle::load_textures(std::unique_ptr<ParticleTextures>& data_ptr, const std::string& location)
{
	u32 texture_count = 0;
	for (; texture_count < MAX_TEXTURES; texture_count++)
	{
		std::string texture_location = location + "/" + std::to_string(texture_count + 1) + ".png";
		if (!std::filesystem::exists(texture_location))
			break;
	}

	if (texture_count == 0)
		throw std::runtime_error("No textures found in: " + location);

	data_ptr.reset(new ParticleTextures(texture_count));

	for (u32 i = 0; i < texture_count; i++)
	{
		std::string texture_location = location + "/" + std::to_string(i + 1) + ".png";
		data_ptr->textures[i].store_buffer(TextureBuffer(texture_location.c_str()));
	}
}

void Particle::update_animations(entt::registry& registry, f32 delta_time)
{
	for (auto [entity, particle] : registry.view<Particle>().each())
	{
		particle.animation_time += particle.frames_per_second * delta_time;
		if ((u32)particle.animation_time >= particle.textures_asset.get().texture_count)
		{
			registry.destroy(entity);
		}
	}
}

void Particle::render_particles(entt::registry& registry, Graphics& graphics)
{
	for (auto [entity, particle_transform, particle] : registry.view<Transform, Particle>().each())
	{
		ImageTransform transform = graphics.create_transform();
		transform.translate(particle_transform.pos.x, particle_transform.pos.y);
		transform.rotate(particle_transform.rot);
		transform.scale(particle.scale);

		u32 image_index = (u32)particle.animation_time;
		if (image_index < particle.textures_asset.get().texture_count)
		{
			graphics.draw_image(particle.textures_asset.get().textures[image_index], transform);
		}
	}
}