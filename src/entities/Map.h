#pragma once

#include "engine/Types.h"
#include "engine/Asset.h"
#include "engine/Texture.h"
#include "engine/Mesh.h"
#include "engine/Graphics.h"
#include "Components.h"

#include "entt/entt.hpp"

#include <vector>
#include <variant>

struct MapGridLayer : NoCopy
{
	MapGridLayer(u32 h_tiles, u32 v_tiles);

	u32 h_tiles, v_tiles;
	std::unique_ptr<u32[]> tile_ids;
};

struct MapObject
{
	u32 gid;
	f32 x, y, width_pixel, height_pixel, rotation;
};

struct MapObjectLayer : NoCopy
{
	std::vector<MapObject> objects;
};

struct CollisionBox
{
	f32 width, height, radius;
};

struct CollisionCircle
{
	f32 radius;
};

struct TilesetTile : NoCopy
{
	Asset<Texture> asset;
	f32 tile_width;
	f32 tile_height;
	std::vector<std::variant<CollisionBox, CollisionCircle>> collision_objects;
};

struct Tileset : NoCopy
{
	Tileset(const char* location, u32 pixel_scale);

private:
	std::unique_ptr<TilesetTile[]> tiles;
	u32 asset_count;

	friend struct MapRenderable;
	friend struct Map;
};

struct Map : NoCopy
{
	Map(const char* location, u32 pixel_scale);

	void set_full_screen_camera(Camera& camera);

	static entt::entity create_map_entity(entt::registry& registry, const char* map_location, u32 pixel_scale);
	static void create_map_renderable(entt::registry& registry, entt::entity entity, Tileset& tileset);
	static void create_map_physics(entt::registry& registry, entt::entity entity, Tileset& tileset);

	u32 h_tiles, v_tiles;
	f32 tile_size;
	f32 world_width, world_height;
	u32 first_gid;
	std::vector<std::variant<MapGridLayer, MapObjectLayer>> layers;
};

struct MapRenderable : NoCopy
{
	MapRenderable(Map& map, Tileset& tileset);

	static void render_map(entt::registry& registry, Graphics& graphics);

	friend MapGridLayer;
	friend MapObjectLayer;

private:
	std::vector<std::unique_ptr<AssetRef<Texture>>> asset_refs;
};