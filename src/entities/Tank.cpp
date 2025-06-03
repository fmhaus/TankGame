#include "Tank.h"

#include "World.h"
#include "Components.h"
#include "Projectile.h"
#include "Particle.h"
#include "AssetManager.h"
#include "engine/util/MathUtil.h"
#include "engine/util/StringUtil.h"

#include <cmath>
#include <fstream>

const f32 TANK_SCALE = 0.75f;

void HullData::load_from_file(std::unique_ptr<HullData>& data_ptr, const char* location, u32 pixel_scale)
{
	std::ifstream file(location);
	if (!file)
		throw std::runtime_error(std::string("Failed load file: ") + location);

	std::string line;

	std::getline(file, line);
	StringSplitter splitter(line, ';');
	f32 scale = std::stof(*splitter.next_word());
	f32 turret_pivot_y = std::stof(*splitter.next_word());

	std::getline(file, line);
	splitter = StringSplitter(line, ';');
	f32 tracks_scale = std::stof(*splitter.next_word());
	f32 tracks_x = std::stof(*splitter.next_word());
	f32 tracks_y = std::stof(*splitter.next_word());

	f32 inv_pixel_scale = 1.0 / (f32)pixel_scale;

	data_ptr = std::make_unique<HullData>(
		scale,
		turret_pivot_y * inv_pixel_scale,
		tracks_scale,
		tracks_x * inv_pixel_scale,
		tracks_y * inv_pixel_scale
	);
}

void TurretData::load_from_file(std::unique_ptr<TurretData>& data_ptr, const char* location, u32 pixel_scale)
{
	std::ifstream file(location);
	if (!file)
		throw std::runtime_error(std::string("Failed load file: ") + location);

	std::string line;

	std::getline(file, line);
	StringSplitter splitter(line, ';');
	f32 scale = std::stof(*splitter.next_word());
	f32 pivot_y = std::stof(*splitter.next_word());

	f32 inv_pixel_scale = 1.0 / (f32)pixel_scale;
	data_ptr = std::make_unique<TurretData>(scale, pivot_y * inv_pixel_scale, std::vector<glm::vec2>());

	while (std::getline(file, line))
	{
		splitter = StringSplitter(line, ';');
		f32 barrel_x = std::stof(*splitter.next_word());
		f32 barrel_y = std::stof(*splitter.next_word());
		data_ptr->barrel_points.emplace_back(barrel_x * inv_pixel_scale, barrel_y * inv_pixel_scale);
	}
}

static u32 id_generator = 1;

Tank::Tank(u32 id, entt::entity entity, const TankDesign& design)
	: id(id), entity(entity), design(design), turret_orientation(0.0f), shoot_barrel_index(0),
	hull_data(AssetManager::get_instance().hull_data[design.hull].loaded()),
	turret_data(AssetManager::get_instance().turret_data[design.turret].loaded())
{

}

glm::vec2 Tank::get_shoot_point(entt::registry& registry)
{
	Transform& tank_transform = registry.get<Transform>(entity);
	glm::vec2 hull_pivot = MathUtil::rotate({ 0.0f, hull_data.get().turret_pivot_y }, tank_transform.rot);

	shoot_barrel_index %= turret_data.get().barrel_points.size();
	glm::vec2 barrel_offset = MathUtil::rotate(turret_data.get().barrel_points[shoot_barrel_index] - glm::vec2(0.0f, turret_data.get().pivot_y), turret_orientation); // use both barrels
	shoot_barrel_index++;

	glm::vec2 shoot_point = tank_transform.pos + (hull_pivot + barrel_offset) * TANK_SCALE;

	return shoot_point;
}

entt::entity Tank::shoot_projectile(entt::registry& registry, const ProjectileType& type)
{
	glm::vec2 shoot_point = get_shoot_point(registry);
	entt::entity projectile = Projectile::create_projectile(registry, entity, type, shoot_point, turret_orientation);
	if (registry.all_of<TankRenderable>(entity))
	{
		Projectile::create_projectile_renderable(registry, projectile, type);

		Particle::create(registry, AssetManager::get_instance().particle_flash[type.particle_type], { shoot_point, turret_orientation }, 10.0f, 0.5f);
	}
	return entity;
}

entt::entity Tank::create_tank(entt::registry& registry, const TankDesign& design, glm::vec2 pos, bool player_control)
{
	entt::entity entity = registry.create();
	registry.emplace<Tank>(entity, id_generator++, entity, design);
	registry.emplace<Transform>(entity, pos, 0.0f);
	registry.emplace<Velocity>(entity);
	registry.emplace<TankRenderable>(entity, design);
	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.density = 868.0;
	shape_def.material.friction = 0.3f;
	shape_def.filter.groupIndex = -registry.get<Tank>(entity).id;
	registry.emplace<Physics>(entity, true).create_box_shape(shape_def, 1.32f * TANK_SCALE, 1.88f * TANK_SCALE);
	if (player_control)
		registry.emplace<TankPlayerController>(entity, TankMovementSettings{ 2.0f, 2.0f, 8.0f, glm::radians(80.0f), glm::radians(200.0f), 5.0f });
	return entity;
}

void Tank::update_tank_design(entt::registry& registry, entt::entity tank_entity, const TankDesign& new_design)
{
	Tank& tank = registry.get<Tank>(tank_entity);
	TankRenderable* renderable = nullptr;
		
	bool load_hull_texture = false;
	bool load_turret_texture = false;
	bool load_track_texture = false;

	if (tank.design.color != new_design.color)
	{
		load_hull_texture = true;
		load_turret_texture = true;
	}

	if (tank.design.hull != new_design.hull)
	{
		tank.hull_data = AssetManager::get_instance().hull_data[new_design.hull].loaded();
		load_hull_texture = true;
	}

	if (tank.design.turret != new_design.turret)
	{
		tank.turret_data = AssetManager::get_instance().turret_data[new_design.turret].loaded();
		load_turret_texture = true;
	}

	if (tank.design.tracks != new_design.tracks)
		load_track_texture = true;

	if (registry.all_of<TankRenderable>(tank_entity))
	{
		TankRenderable& renderable = registry.get<TankRenderable>(tank_entity);
		if (load_hull_texture)
			renderable.hull_texture = AssetManager::get_instance().hull_textures[new_design.color][new_design.hull].loaded();
		if (load_turret_texture)
			renderable.turret_texture = AssetManager::get_instance().turret_textures[new_design.color][new_design.turret].loaded();
		if (load_track_texture)
		{
			renderable.track_textures[0] = AssetManager::get_instance().track_textures[new_design.tracks][0].loaded();
			renderable.track_textures[1] = AssetManager::get_instance().track_textures[new_design.tracks][1].loaded();
		}
	}

	tank.design = new_design;
}


f32 Tank::get_scale()
{
	return TANK_SCALE;
}

TankRenderable::TankRenderable(const TankDesign& design)
	: hull_texture(AssetManager::get_instance().hull_textures[design.color][design.hull].loaded()),
	turret_texture(AssetManager::get_instance().turret_textures[design.color][design.turret].loaded()),
	track_textures{ AssetManager::get_instance().track_textures[design.tracks][0].loaded(), AssetManager::get_instance().track_textures[design.tracks][1].loaded() },
	turret_rotation(0.0),
	track_animation_1(0.0),
	track_animation_2(0.0)
{
}

void TankRenderable::update_track_animation(entt::registry& registry, f32 frame_time)
{
	for (auto [entity, tank, renderable, transform, velocity] : registry.view<Tank, TankRenderable, Transform, Velocity>().each())
	{
		// TODO: fix unfortunately
		f32 speed = glm::dot(velocity.linear, -glm::vec2(std::sin(transform.rot), std::cos(transform.rot)));
		if (speed == 0 && velocity.angular == 0)
			continue;
		 
		f32 track_1_speed, track_2_speed;

		f32 track_offset = tank.hull_data.get().tracks_off_x * tank.hull_data.get().scale * TANK_SCALE;

		if (velocity.angular != 0)
		{
			f32 turn_radius = speed / velocity.angular;

			if (speed != 0)
			{
				track_1_speed = speed * (turn_radius - track_offset) / turn_radius;
				track_2_speed = speed * (turn_radius + track_offset) / turn_radius;
			}
			else
			{
				track_1_speed = -track_offset * velocity.angular;
				track_2_speed = track_offset * velocity.angular;
			}
		}
		else
		{
			track_1_speed = track_2_speed = speed;
		}
		
		f32 track_step = 20.0f / 256.0f * tank.hull_data.get().tracks_scale * TANK_SCALE;

		renderable.track_animation_1 += track_1_speed / track_step * frame_time;
		renderable.track_animation_2 += track_2_speed / track_step * frame_time;
	}
}

void TankRenderable::render_tanks(entt::registry& registry, Graphics& graphics)
{
	for (auto [entity, tank, renderable, transform] : registry.view<Tank, TankRenderable, Transform>().each())
	{
		auto hull_transform = graphics.create_transform();
		hull_transform.translate(transform.pos.x, transform.pos.y);
		hull_transform.scale(TANK_SCALE * tank.hull_data.get().scale, TANK_SCALE * tank.hull_data.get().scale);
		hull_transform.rotate(transform.rot);

		auto track_transform = hull_transform;
		track_transform.scale(tank.hull_data.get().tracks_scale);
		track_transform.translate(-tank.hull_data.get().tracks_off_x, tank.hull_data.get().tracks_off_y);

		graphics.draw_image(renderable.track_textures[((u32)renderable.track_animation_1) % 2].get(), track_transform);
		track_transform.translate(2.0f * tank.hull_data.get().tracks_off_x, 0.0f);
		graphics.draw_image(renderable.track_textures[((u32)renderable.track_animation_2) % 2].get(), track_transform);

		graphics.draw_image(renderable.hull_texture.get(), hull_transform);

		auto turret_transform = graphics.create_transform();
		turret_transform.translate(transform.pos.x, transform.pos.y);
		turret_transform.scale(TANK_SCALE * tank.turret_data.get().scale, TANK_SCALE * tank.turret_data.get().scale);
		turret_transform.rotate(transform.rot);
		turret_transform.translate(0.0f, tank.hull_data.get().turret_pivot_y);
		turret_transform.rotate(renderable.turret_rotation - transform.rot);
		turret_transform.translate(0.0f, -tank.turret_data.get().pivot_y);

		graphics.draw_image(renderable.turret_texture.get(), turret_transform);
	}
}

TankPlayerController::TankPlayerController(const TankMovementSettings& settings)
	: movement_settings(settings),
	rel_turret_rotation(0.0f)
{
}


void TankPlayerController::update_tank(entt::registry& registry, const Window& player_input, const Graphics& graphics, f32 frame_time)
{
	for (auto [entity, controller, physics, tank] : registry.view<TankPlayerController, Physics, Tank>().each())
	{
		// Make sure tank is always moving in a straight line
		auto b2_transform = b2Body_GetTransform(physics.body);
		auto b2_pos = b2_transform.p;
		auto b2_rot = b2_transform.q;
		auto b2_vel = b2Body_GetLinearVelocity(physics.body);
		glm::vec2 forward_dir = { b2_rot.s, -b2_rot.c };
		f32 forwards_mag = glm::dot(glm::vec2(b2_vel.x, b2_vel.y), forward_dir);
		glm::vec2 new_vel = forward_dir * forwards_mag;

		f32 forwards_force = 0.0;
		f32 torque = 0.0;

		f32 turning_speed = b2Body_GetAngularVelocity(physics.body);

		bool input_forwards = player_input.is_key_pressed(GLFW_KEY_W);
		bool input_backwards = player_input.is_key_pressed(GLFW_KEY_S);
		bool input_left = player_input.is_key_pressed(GLFW_KEY_A);
		bool input_right = player_input.is_key_pressed(GLFW_KEY_D);

		// forwards and backwards
		if (input_forwards && !input_backwards)
		{
			if (forwards_mag > controller.movement_settings.max_speed)
				new_vel = forward_dir * controller.movement_settings.max_speed;
			else if (forwards_mag >= 0)
				forwards_force = controller.movement_settings.acceleration_force;
			else
				forwards_force = controller.movement_settings.braking_force;
		}
		else if (!input_forwards && input_backwards)
		{
			if (forwards_mag < -controller.movement_settings.max_speed)
				new_vel = forward_dir * -controller.movement_settings.max_speed;
			else if (forwards_mag <= 0)
				forwards_force = -controller.movement_settings.acceleration_force;
			else
				forwards_force = -controller.movement_settings.braking_force;
		}
		else
		{
			if (abs(forwards_mag) < 0.1f)
				new_vel = glm::vec2(0);
			else if (forwards_mag < 0)
				forwards_force = controller.movement_settings.braking_force;
			else
				forwards_force = -controller.movement_settings.braking_force;
		}

		// Reverse left and right when driving backwards
		if (forwards_mag < 0)
			std::swap(input_left, input_right);

		// left and right
		if (input_left && !input_right)
		{
			if (turning_speed < -controller.movement_settings.max_turning_speed)
				turning_speed = -controller.movement_settings.max_turning_speed;
			else
				torque = -controller.movement_settings.turning_torque;
		}
		else if (!input_left && input_right)
		{
			if (turning_speed > controller.movement_settings.max_turning_speed)
				turning_speed = controller.movement_settings.max_turning_speed;
			else
				torque = controller.movement_settings.turning_torque;

		}
		else
		{
			if (abs(turning_speed) < 0.5f)
				b2Body_SetAngularVelocity(physics.body, 0.0f);
			else if (turning_speed < 0)
				torque = controller.movement_settings.turning_torque;
			else
				torque = -controller.movement_settings.turning_torque;
		}

		b2Body_SetLinearVelocity(physics.body, b2Vec2(new_vel.x, new_vel.y));
		if (torque != 0.0)
			b2Body_ApplyTorque(physics.body, torque * b2Body_GetMass(physics.body), true);
		if (forwards_force != 0.0)
		{
			auto force_vector = b2Body_GetMass(physics.body) * forwards_force * forward_dir;
			b2Body_ApplyForceToCenter(physics.body, b2Vec2(force_vector.x, force_vector.y), true);
		}

		glm::vec2 mouse_world = graphics.to_world_space(player_input.get_cursor_pos());
		glm::vec2 to_mouse = mouse_world - glm::vec2(b2_pos.x, b2_pos.y);
		f32 angle_target = std::atan2(to_mouse.x, -to_mouse.y);
		f32 tank_rot = b2Rot_GetAngle(b2_rot);
		f32 turret_rotation_speed = MathUtil::normalize_angle_difference(tank_rot + controller.rel_turret_rotation, angle_target) * controller.movement_settings.gun_rotation_speed;
		controller.rel_turret_rotation += turret_rotation_speed * frame_time;

		tank.turret_orientation = tank_rot + controller.rel_turret_rotation;

		if (registry.all_of<TankRenderable>(entity))
			registry.get<TankRenderable>(entity).turret_rotation = tank_rot + controller.rel_turret_rotation;
	}
}