#include "Components.h"

Physics::Physics(bool dynamic)
	: dynamic(dynamic), body(b2_nullBodyId)
{
}

void Physics::create_polygon_shape(const b2ShapeDef& shape_def, const b2Polygon& polygon)
{
	b2CreatePolygonShape(body, &shape_def, &polygon);
}

void Physics::create_box_shape(const b2ShapeDef& shape_def, f32 width, f32 height, f32 radius)
{
	b2Polygon box = b2MakeBox(width * 0.5f, height * 0.5f);
	box.radius = radius;
	b2CreatePolygonShape(body, &shape_def, &box);
}

entt::entity Physics::get_entity(b2BodyId body)
{
	return static_cast<entt::entity>(reinterpret_cast<usz>(b2Body_GetUserData(body)));
}

bool Physics::is_in_contact(entt::registry& registry, entt::entity e1, entt::entity e2)
{
	Physics& p1 = registry.get<Physics>(e1);
	Physics& p2 = registry.get<Physics>(e2);

	u32 contacts_cap_1 = b2Body_GetContactCapacity(p1.body);
	u32 contacts_cap_2 = b2Body_GetContactCapacity(p2.body);

	static const u32 MAX_CONTACTS = 32;
	static b2ContactData contacts_array[MAX_CONTACTS];

	u32 min_contacts_cap = std::min(std::min(contacts_cap_1, contacts_cap_2), MAX_CONTACTS);
	b2BodyId query_body, other_body;

	if (contacts_cap_1 <= contacts_cap_2)
	{
		b2Body_GetContactData(p1.body, contacts_array, min_contacts_cap);
		query_body = p1.body;
		other_body = p2.body;
	}
	else
	{
		b2Body_GetContactData(p2.body, contacts_array, min_contacts_cap);
		query_body = p2.body;
		other_body = p1.body;
	}

	for (u32 i = 0; i < min_contacts_cap; i++)
	{
		b2BodyId b1 = b2Shape_GetBody(contacts_array[i].shapeIdA);
		b2BodyId b2 = b2Shape_GetBody(contacts_array[i].shapeIdB);

		if (B2_ID_EQUALS(b1, query_body))
		{
			if (B2_ID_EQUALS(b2, other_body))
				return true;
		}
		else // b2 == query_body
		{	
			if (B2_ID_EQUALS(b1, other_body))
				return true;
		}
	}

	return false;

}

void Physics::update_components(entt::registry& registry)
{
	for (auto [entity, physics, transform] : registry.view<Physics, Transform>().each())
	{
		auto pos = b2Body_GetPosition(physics.body);
		transform.pos.x = pos.x;
		transform.pos.y = pos.y;
		transform.rot = b2Rot_GetAngle(b2Body_GetRotation(physics.body));
	}

	for (auto [entity, physics, velocity] : registry.view<Physics, Velocity>().each())
	{
		auto linear = b2Body_GetLinearVelocity(physics.body);
		velocity.linear.x = linear.x;
		velocity.linear.y = linear.y;
		velocity.angular = b2Body_GetAngularVelocity(physics.body);
	}
}

void Transform::update_physics(entt::registry& registry, entt::entity entity)
{
	if (registry.any_of<Physics>(entity))
	{
		b2Body_SetTransform(registry.get<Physics>(entity).body, b2Vec2(pos.x, pos.y), b2MakeRot(rot));
	}
}

void Transform::set_and_update_physics(entt::registry& registry, entt::entity entity, glm::vec2 pos, f32 rot)
{
	auto [transform, physics] = registry.get<Transform, Physics>(entity);
	transform.pos = pos;
	transform.rot = rot;
	b2Body_SetTransform(physics.body, b2Vec2(pos.x, pos.y), b2MakeRot(rot));
}

void Velocity::update_physics(entt::registry& registry, entt::entity entity)
{
	if (registry.any_of<Physics>(entity))
	{
		b2BodyId body = registry.get<Physics>(entity).body;
		b2Body_SetLinearVelocity(body, b2Vec2(linear.x, linear.y));
		b2Body_SetAngularVelocity(body, angular);
	}
}

void Velocity::set_and_update_physics(entt::registry& registry, entt::entity entity, glm::vec2 lin, f32 ang)
{
	auto [velocity, physics] = registry.get<Velocity, Physics>(entity);
	velocity.linear = lin;
	velocity.angular = ang;
	b2Body_SetLinearVelocity(physics.body, b2Vec2(lin.x, lin.y));
	b2Body_SetAngularVelocity(physics.body, ang);
}

void SimpleSpriteRenderable::render(Graphics& graphics, Transform& transform)
{
	auto image_transform = graphics.create_transform();
	image_transform.translate(transform.pos.x, transform.pos.y);
	image_transform.scale(scale, scale);
	image_transform.rotate(transform.rot);
	graphics.draw_image(texture.get(), image_transform);
}

void Physics::on_create_physics(entt::registry& registry, b2WorldId world, entt::entity entity)
{
	Physics& physics = registry.get<Physics>(entity);

	b2BodyDef body_def = b2DefaultBodyDef();
	body_def.type = physics.dynamic ? b2_dynamicBody : b2_staticBody;
	body_def.userData = reinterpret_cast<void*>((usz)entity);

	if (registry.all_of<Transform>(entity))
	{
		Transform& transform = registry.get<Transform>(entity);
		body_def.position = b2Vec2(transform.pos.x, transform.pos.y);
		body_def.rotation = b2MakeRot(transform.rot);
	}
	if (registry.all_of<Velocity>(entity))
	{
		Velocity& movement = registry.get<Velocity>(entity);
		body_def.linearVelocity = b2Vec2(movement.linear.x, movement.linear.y);
		body_def.angularVelocity = movement.angular;
	}

	physics.body = b2CreateBody(world, &body_def);
}

void Physics::on_destroy_physics(entt::registry& registry, entt::entity entity)
{
	Physics& physics = registry.get<Physics>(entity);

	b2DestroyBody(physics.body);
	physics.body = b2_nullBodyId;
}
