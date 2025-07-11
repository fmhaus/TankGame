#pragma once

#include "entt/entt.hpp"
#include "box2d/box2d.h"

#include "engine/Types.h"
#include "engine/Graphics.h"

#include <functional>

enum class CollisionListenerType : u32
{
	OnContactBegin = 0,
	OnContactEnd = 1
};

/// Used to track registered (collision) listeners to a World.
struct CollisionListenerID
{
	CollisionListenerType get_type();

	u32 value;
};

/// Stores the information for a collision listener. The id has one bit that stores the type (contact begin or contact end)
/// has_component stores the type information for what component types this listener should be invoked
struct BeginContactListener
{
	CollisionListenerID id;
	std::function<bool(entt::registry&, entt::entity)> has_component;
	std::function<void(entt::registry&, entt::entity, entt::entity, glm::vec2, glm::vec2)> callback;
};

struct EndContactListener
{
	CollisionListenerID id;
	std::function<bool(entt::registry&, entt::entity)> has_component;
	std::function<void(entt::registry&, entt::entity, entt::entity)> callback;
};


struct World : NoCopy 
{
	World();
	~World();

	/// Handles player input
	void handle_inputs(f32 delta_time, const Camera& camera);

	/// Updates the world with the delta time step (last frame time)
	void update(f32 delta_time);

	/// Draws the entities using the Graphics instance
	void render(Graphics& graphics);

	/// Enables or disables the physics debug draw. The debug is drawn on render(Graphics&) after the entities have been rendered
	void set_physics_debug_draw_enabled(bool enabled);

	/// Registers a listener for receiving collision callbacks
	/// The returned ListenerID can be used to unregister the callback
	/// type_contact_begin specified whether the callback should be invoked on contact begin or on contact end.
	/// The template Types specify which components an entity need to have for the callback to be invoked.
	/// The second callback argument (first entity) always has all template types as components
	/// If both entities in contact have all component types, the callback is called twice where both entities are the first (entity) argument once.
	template <typename... Types>
	CollisionListenerID add_begin_contact_listener(std::function<void(entt::registry&, entt::entity component_entity, entt::entity other_entity, glm::vec2 pos, glm::vec2 normal)> listener)
	{
		CollisionListenerID id = next_listener_ID(CollisionListenerType::OnContactBegin);
		begin_contact_listeners.emplace_back(
				id,
				[](entt::registry& registry, entt::entity entity)
				{
					return registry.all_of<Types...>(entity);
				}, listener);
		return id;
	}

	template <typename... Types>
	CollisionListenerID add_end_contact_listener(std::function<void(entt::registry&, entt::entity component_entity, entt::entity other_entity)> listener)
	{
		CollisionListenerID id = next_listener_ID(CollisionListenerType::OnContactEnd);
		end_contact_listeners.emplace_back(
			id,
			[](entt::registry& registry, entt::entity entity)
			{
				return registry.all_of<Types...>(entity);
			}, listener);
		return id;
	}

	/// Removes a registered collision listener
	void remove_collision_listener(CollisionListenerID id)
	{
		if (id.get_type() == CollisionListenerType::OnContactBegin)
			std::erase_if(this->begin_contact_listeners, [=](auto& l) { return l.id.value == id.value; });
		else
			std::erase_if(this->end_contact_listeners, [=](auto& l) { return l.id.value == id.value; });
	}

	entt::registry registry;
	b2WorldId physics_world;

private:
	static CollisionListenerID next_listener_ID(CollisionListenerType type);

	void on_create_physics(entt::registry& registry, entt::entity entity);
	void on_destroy_physics(entt::registry& registry, entt::entity entity);

	std::unique_ptr<b2DebugDraw> physics_debug_draw;
	std::vector<BeginContactListener> begin_contact_listeners;
	std::vector<EndContactListener> end_contact_listeners;
};