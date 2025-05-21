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
