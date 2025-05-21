#include "World.h"

#include "Components.h"

#include "Tank.h"
#include "Map.h"
#include "Projectile.h"

World::World()
{
	b2WorldDef world_def = b2DefaultWorldDef();
	world_def.gravity = b2Vec2_zero;
	physics_world = b2CreateWorld(&world_def);

	registry.on_construct<Physics>().connect<&World::on_create_physics>(*this);
	registry.on_destroy<Physics>().connect<&World::on_destroy_physics>(*this);
}

World::~World()
{
	registry.clear();
	b2DestroyWorld(physics_world);
}

void World::update(f32 delta_time)
{
	b2World_Step(physics_world, delta_time, 4);
	Physics::update_components(registry);

	Projectile::update_projectiles(registry);

	TankRenderable::update_track_animation(registry, delta_time);
}

void World::render(Graphics& graphics)
{
	MapRenderable::render_map(registry, graphics);
	TankRenderable::render_tanks(registry, graphics);
	ProjectileRenderable::render_projectiles(registry, graphics);
}

void World::draw_physics_debug(Graphics& graphics)
{
	if (physics_debug_draw)
	{
		Rect camera_box = graphics.camera.get_bounding_rect();
		/*
		physics_debug_draw->drawingBounds = b2AABB(
			b2Vec2(camera_box.x, camera_box.y),
			b2Vec2(camera_box.x + camera_box.width, camera_box.y + camera_box.height)
		);
		*/
		physics_debug_draw->context = (void*)&graphics;

		b2World_Draw(physics_world, physics_debug_draw.get());
	}
}

Color color_from_b2_hex(b2HexColor color)
{
	f32 r = ((color >> 16) & 0xFF) / 255.0f;
	f32 g = ((color >> 8) & 0xFF) / 255.0f;
	f32 b = (color & 0xFF) / 255.0f;
	return { r, g, b, 1.0f };
}
#include <iostream>
void World::set_physics_debug_draw_enabled(bool enabled)
{
	if ((physics_debug_draw.get() != nullptr) == enabled)
		return;
	if (enabled)
	{
		physics_debug_draw = std::make_unique<b2DebugDraw>(b2DefaultDebugDraw());
		physics_debug_draw->DrawCircleFcn = [](b2Vec2 center, f32 radius, b2HexColor color, void* context)
			{
				Graphics* graphics = static_cast<Graphics*>(context);
				graphics->draw_circle(glm::vec2(center.x, center.y), radius, color_from_b2_hex(color));
			};
		physics_debug_draw->DrawPointFcn = [](b2Vec2 p, f32 size, b2HexColor color, void* context)
			{
				Graphics* graphics = static_cast<Graphics*>(context);
				graphics->fill_circle(glm::vec2(p.x, p.y), size * 0.5f, color_from_b2_hex(color));
			};
		physics_debug_draw->DrawPolygonFcn = [](const b2Vec2* vertices, s32 vertex_count, b2HexColor color, void* context)
			{
				std::vector<glm::vec2> polygon;
				for (s32 i = 0; i < vertex_count; i++)
					polygon.emplace_back(vertices[i].x, vertices[i].y);
				Graphics* graphics = static_cast<Graphics*>(context);
				graphics->draw_polygon(polygon, color_from_b2_hex(color));
			};
		physics_debug_draw->DrawSegmentFcn = [](b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* context)
			{
				Graphics* graphics = static_cast<Graphics*>(context);
				graphics->draw_line({ p1.x, p1.y }, { p2.x, p2.y }, color_from_b2_hex(color));
			};
		physics_debug_draw->DrawSolidCircleFcn = [](b2Transform transform, float radius, b2HexColor color, void* context)
			{
				Graphics* graphics = static_cast<Graphics*>(context);
				graphics->fill_circle(glm::vec2(transform.p.x, transform.p.y), radius, color_from_b2_hex(color));
			};
		physics_debug_draw->DrawSolidPolygonFcn = [](b2Transform transform, const b2Vec2* vertices, int vertex_count, float radius, b2HexColor color, void* context)
			{
				std::vector<glm::vec2> polygon;
				for (s32 i = 0; i < vertex_count; i++)
				{
					polygon.emplace_back(
						transform.p.x + vertices[i].x * transform.q.c - vertices[i].y * transform.q.s, 
						transform.p.y + vertices[i].x * transform.q.s + vertices[i].y * transform.q.c);
				}
				Graphics* graphics = static_cast<Graphics*>(context);
				graphics->fill_polygon(polygon, color_from_b2_hex(color));
			};
		physics_debug_draw->drawShapes = true;
		/*
		* // TODO: maybe implement someday
		physics_debug_draw->DrawSolidCapsuleFcn = [](b2Vec2 p1, b2Vec2 p2, f32 radius, b2HexColor color, void* context)
			{
			};
		physics_debug_draw->DrawStringFcn = [](b2Vec2 p, const char* s, b2HexColor color, void* context)
			{
			};
		*/
	}
	else
	{
		physics_debug_draw.reset();
	}
}

void World::on_create_physics(entt::registry& registry, entt::entity entity)
{
	Physics& physics = registry.get<Physics>(entity);

	b2BodyDef body_def = b2DefaultBodyDef();
	body_def.type = physics.dynamic ? b2_dynamicBody : b2_staticBody;

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

	physics.body = b2CreateBody(physics_world, &body_def);
}


void World::on_destroy_physics(entt::registry& registry, entt::entity entity)
{
	Physics& physics = registry.get<Physics>(entity);

	b2DestroyBody(physics.body);
	physics.body = b2_nullBodyId;
}
