#include "Map.h"

#include "CollisionCategory.h"
#include "engine/util/MathUtil.h"

#include "pugixml.hpp"

#include <stdexcept>
#include <string>
#include <ranges>
#include <sstream>
#include <iostream>

const u32 TILE_ID_MASK = ~(0b1111 << 28);

MapGridLayer::MapGridLayer(u32 h_tiles, u32 v_tiles)
	: h_tiles(h_tiles), 
	v_tiles(v_tiles), 
	tile_ids(std::make_unique<u32[]>(h_tiles * v_tiles))
{
}


Map::Map(const char* location, u32 pixel_scale)
	: first_gid(1)
{
	f32 rec_pixel_scale = 1.0f / (f32)pixel_scale;

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(location);

	if (!result)
		throw std::runtime_error("Failed to load xml: " + std::string(location) + "\n" + result.description());

	pugi::xml_node map_element = doc.child("map");
	
	if (strcmp(map_element.attribute("orientation").value(), "orthogonal") != 0)
		throw std::runtime_error("Invalid orientation in map xml: " + std::string(location));

	u32 width = map_element.attribute("width").as_uint((u32) - 1);
	u32 height = map_element.attribute("height").as_uint((u32) -1);
	if (width == (u32)-1 || height == (u32)-1)
		throw std::runtime_error("Invalid width or height in map xml: " + std::string(location));

	u32 tile_width = map_element.attribute("tilewidth").as_uint((u32)-1);
	u32 tile_height = map_element.attribute("tileheight").as_uint((u32)-1);
	if (tile_width != tile_height || tile_width == -1)
		throw std::runtime_error("Invalid tile width or height in map xml: " + std::string(location));

	for (pugi::xml_node& child : map_element.children())
	{
		if (strcmp(child.name(), "tileset") == 0)
		{
			this->first_gid = child.attribute("firstgid").as_uint();
		} 
		else if (strcmp(child.name(), "layer") == 0)
		{
			u32 id = child.attribute("id").as_uint();
			u32 w = child.attribute("width").as_uint();
			u32 h = child.attribute("height").as_uint();
			if (w != width && h != height)
			{
				std::cerr << "Skip layer " << id << " because dimensions are not matching" << std::endl;
				continue;
			}

			pugi::xml_node data = child.child("data");
			std::string text = data.text().as_string();

			MapGridLayer& layer = std::get<MapGridLayer>(this->layers.emplace_back(MapGridLayer(w, h)));

			u32 x, y = 0;

			std::istringstream line_stream(text);
			std::string line;
			
			while (std::getline(line_stream, line, '\n'))
			{
				if (line.empty())
					continue;

				x = 0;
				std::istringstream word_stream(line);
				std::string word;

				while (std::getline(word_stream, word, ',')) {
					if (word.empty()) 
						continue;
					u32 gid = std::stoul(word);
					layer.tile_ids[y * w + x] = gid;
					x++;
				}
				y++;
			}
		}
		else if (strcmp(child.name(), "objectgroup") == 0)
		{
			MapObjectLayer& layer = std::get<MapObjectLayer>(this->layers.emplace_back(MapObjectLayer()));

			for (pugi::xml_node& object : child.children())
			{
				if (strcmp(object.name(), "object") == 0)
				{
					layer.objects.emplace_back(
						object.attribute("gid").as_uint(),
						object.attribute("x").as_float() * rec_pixel_scale,
						object.attribute("y").as_float() * rec_pixel_scale,
						object.attribute("width").as_float(),
						object.attribute("height").as_float(),
						object.attribute("rotation").as_float());
				}
			}
		}
	}

	this->h_tiles = width;
	this->v_tiles = height;
	this->tile_size = tile_width * rec_pixel_scale;
	this->world_width = h_tiles * this->tile_size;
	this->world_height = v_tiles * this->tile_size;
}

void Map::set_full_screen_camera(entt::registry& registry, entt::entity entity, Camera& camera)
{
	Map& map = registry.get<Map>(entity);
	camera.center = glm::vec2(map.world_width / 2.0f, map.world_height / 2.0f);
	f32 map_aspect_ratio = map.world_width / map.world_height;
	if (map_aspect_ratio < camera.window_width / camera.window_height)
		camera.set_vertical_scope(map.world_height);
	else
		camera.h_scope = map.world_width;
	camera.update_matrix();
}

entt::entity Map::create_map_entity(entt::registry& registry, const char* map_location, u32 pixel_scale)
{
	auto entity = registry.create();
	Map& map = registry.emplace<Map>(entity, map_location, pixel_scale);
	return entity;
}

void Map::create_map_renderable(entt::registry& registry, entt::entity entity, Tileset& tileset)
{
	registry.emplace<MapRenderable>(entity, registry.get<Map>(entity), tileset);
}

void add_collision_objects(Physics& physics, TilesetTile& tile, f32 x, f32 y, bool flip_diag, f32 rot)
{
	for (auto& collision_variant : tile.collision_objects)
	{
		if (std::holds_alternative<CollisionBox>(collision_variant))
		{
			CollisionBox& box = std::get<CollisionBox>(collision_variant);
			b2Polygon polygon;
			if (flip_diag)
			{
				f32 cx = x + tile.tile_height * 0.5f;
				f32 cy = y + tile.tile_width * 0.5f - 1;
				polygon = b2MakeOffsetBox(box.height * 0.5f, box.width * 0.5f, b2Vec2(cx, cy), b2MakeRot(rot));
			}
			else
			{
				f32 cx = x + tile.tile_width * 0.5f;
				f32 cy = y + tile.tile_height * 0.5f;
				polygon = b2MakeOffsetBox(box.width * 0.5f, box.height * 0.5f, b2Vec2(cx, cy), b2MakeRot(rot));
			}
			b2ShapeDef shape_def = b2DefaultShapeDef();
			shape_def.filter.categoryBits = CATEGORY_MAP;
			physics.create_polygon_shape(shape_def, polygon);
		}
	}
}

void Map::create_map_physics(entt::registry& registry, entt::entity entity, Tileset& tileset)
{
	Map& map = registry.get<Map>(entity);
	Physics& physics = registry.emplace<Physics>(entity, false);

	static const f32 HALF_BORDER_WIDTH = 0.2f;

	f32 half_width = map.world_width * 0.5f;
	f32 half_height = map.world_height * 0.5f;
	b2Polygon p1 = b2MakeOffsetBox(half_width, HALF_BORDER_WIDTH, b2Vec2(half_width, 0.0f), b2Rot_identity);
	b2Polygon p2 = b2MakeOffsetBox(half_width, HALF_BORDER_WIDTH, b2Vec2(half_width, map.world_height), b2Rot_identity);
	b2Polygon p3 = b2MakeOffsetBox(HALF_BORDER_WIDTH, half_height, b2Vec2(0.0f, half_height), b2Rot_identity);
	b2Polygon p4 = b2MakeOffsetBox(HALF_BORDER_WIDTH, half_height, b2Vec2(map.world_width, half_height), b2Rot_identity);
	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.filter.categoryBits = CATEGORY_MAP;

	physics.create_polygon_shape(shape_def, p1);
	physics.create_polygon_shape(shape_def, p2);
	physics.create_polygon_shape(shape_def, p3);
	physics.create_polygon_shape(shape_def, p4);

	for (auto& variant : map.layers)
	{
		if (std::holds_alternative<MapGridLayer>(variant))
		{
			MapGridLayer& layer = std::get<MapGridLayer>(variant);
			for (u32 y = 0; y < layer.v_tiles; y++)
			{
				for (u32 x = 0; x < layer.h_tiles; x++)
				{
					u32 gid = layer.tile_ids[y * layer.h_tiles + x];
					if (gid < map.first_gid)
						continue;
					u32 tile_id = (gid & TILE_ID_MASK) - map.first_gid;

					add_collision_objects(physics, tileset.tiles[tile_id], x, y, gid & (1 << 29), 0.0f);
				}
			}
		}
		else if (std::holds_alternative<MapObjectLayer>(variant))
		{
			MapObjectLayer& layer = std::get<MapObjectLayer>(variant);
			for (auto& o : layer.objects)
			{
				if (o.gid < map.first_gid)
					continue;
				u32 tile_id = (o.gid & TILE_ID_MASK) - map.first_gid;

				add_collision_objects(physics, tileset.tiles[tile_id], o.x, o.y, o.gid & (1 << 29), o.rotation);
			}
		}
	}
}

void load_texture(std::unique_ptr<Texture>& data_ptr, const std::string location)
{
	data_ptr = std::make_unique<Texture>(location.c_str());
}

Tileset::Tileset(const char* location, u32 pixel_scale)
{
	f32 rec_pixel_scale = 1.0f / (f32) pixel_scale;

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(location);

	if (!result)
		throw std::runtime_error("Failed to load xml: " + std::string(location));

	pugi::xml_node tileset_element = doc.child("tileset");
	pugi::xml_node grid_node = tileset_element.child("grid");

	u32 width = grid_node.attribute("width").as_uint();
	u32 height = grid_node.attribute("height").as_uint();

	if (width != height || width == 0)
		throw std::runtime_error("Invalid width or height in tileset xml: " + std::string(location));

	f32 world_grid_size = width * rec_pixel_scale;
	this->asset_count = tileset_element.attribute("tilecount").as_uint();

	std::string location_str = location;
	std::string path = location_str.substr(0, location_str.find_last_of('/') + 1);

	this->tiles = std::make_unique<TilesetTile[]>(asset_count);

	for (pugi::xml_node& tile : tileset_element.children())
	{
		if (strcmp(tile.name(), "tile") == 0)
		{
			u32 id = tile.attribute("id").as_uint();
			if (id >= asset_count)
				continue;

			for (pugi::xml_node& tile_child : tile.children())
			{
				if (strcmp(tile_child.name(), "image") == 0)
				{
					std::string source = tile_child.attribute("source").as_string();
					u32 width = tile_child.attribute("width").as_uint();
					u32 height = tile_child.attribute("height").as_uint();
					this->tiles[id].asset.set(path + source, &load_texture);
					this->tiles[id].tile_width = width * rec_pixel_scale;
					this->tiles[id].tile_height = height * rec_pixel_scale;
				}
				else if (strcmp(tile_child.name(), "collision_box") == 0)
				{
					this->tiles[id].collision_objects.emplace_back(
						CollisionBox(
							tile_child.attribute("width").as_float() * world_grid_size,
							tile_child.attribute("height").as_float() * world_grid_size,
							tile_child.attribute("radius").as_float() * world_grid_size
						));
				}
				else if (strcmp(tile_child.name(), "collision_circle") == 0)
				{
					this->tiles[id].collision_objects.emplace_back(
						CollisionCircle(tile_child.attribute("radius").as_float() * world_grid_size)
					);
				}
			}
		}
	}
}

MapRenderable::MapRenderable(Map& map, Tileset& tileset)
{
	asset_refs.resize(tileset.asset_count);

	for (auto& variant : map.layers)
	{
		if (std::holds_alternative<MapGridLayer>(variant))
		{
			MapGridLayer& layer = std::get<MapGridLayer>(variant);
			for (u32 i = 0; i < layer.v_tiles * layer.h_tiles; i++)
			{
				u32 gid = layer.tile_ids[i];
				if (gid < map.first_gid)
					continue;
				u32 tile_id = (gid & TILE_ID_MASK) - map.first_gid;

				if (!this->asset_refs[tile_id])
					this->asset_refs[tile_id] = std::make_unique<AssetRef<Texture>>(std::move(tileset.tiles[tile_id].asset.loaded()));
			}
		}
		else if (std::holds_alternative<MapObjectLayer>(variant))
		{
			MapObjectLayer& layer = std::get<MapObjectLayer>(variant);
			for (auto& o : layer.objects)
			{
				if (o.gid < map.first_gid)
					continue;
				u32 tile_id = (o.gid & TILE_ID_MASK) - map.first_gid;

				if (!this->asset_refs[tile_id])
					this->asset_refs[tile_id] = std::make_unique<AssetRef<Texture>>(std::move(tileset.tiles[tile_id].asset.loaded()));
			}
		}
	}
}

void MapRenderable::render_map(entt::registry& registry, Graphics& graphics)
{
	for (auto [entity, map, renderable] : registry.view<Map, MapRenderable>().each())
	{
		for (auto& variant : map.layers)
		{
			if (std::holds_alternative<MapGridLayer>(variant))
			{
				MapGridLayer& layer = std::get<MapGridLayer>(variant);
				for (u32 y = 0; y < layer.v_tiles; y++)
				{
					for (u32 x = 0; x < layer.h_tiles; x++)
					{
						u32 index = y * layer.h_tiles + x;
						u32 gid = layer.tile_ids[index];

						if (gid < map.first_gid)
							continue;

						u32 tile_id = (gid & TILE_ID_MASK) - map.first_gid;
						Texture& texture = renderable.asset_refs[tile_id]->get();

						ImageTransform transform = graphics.create_transform();
						transform.translate(x * map.tile_size, (y + 1) * map.tile_size);

						if (gid & (1 << 29))
							transform.translate_pixels(0.5f * texture.get_height(), -0.5f * texture.get_width());
						else
							transform.translate_pixels(0.5f * texture.get_width(), -0.5f * texture.get_height());

						if (gid & (1 << 30))
							transform.flip_y();
						if (gid & (1 << 31))
							transform.flip_x();

						if (gid & (1 << 29))
						{
							transform.flip_y();
							transform.rotate(-0.5f * MathUtil::PI_32);
						}

						graphics.draw_image(texture, transform);
					}
				}
			}
			else if (std::holds_alternative<MapObjectLayer>(variant))
			{
				MapObjectLayer& layer = std::get<MapObjectLayer>(variant);
				for (MapObject& o : layer.objects)
				{
					if (o.gid < map.first_gid)
						continue;

					u32 tile_id = (o.gid & TILE_ID_MASK) - map.first_gid;
					Texture& texture = renderable.asset_refs[tile_id]->get();

					ImageTransform transform = graphics.create_transform();
					transform.translate(o.x, o.y);

					transform.rotate(o.rotation);

					if (o.gid & (1 << 29))
						transform.translate_pixels(0.5f * o.height_pixel, -0.5f * o.width_pixel);
					else
						transform.translate_pixels(0.5f * o.width_pixel, -0.5f * o.height_pixel);

					if (o.gid & (1 << 30))
						transform.flip_y();
					if (o.gid & (1 << 31))
						transform.flip_x();

					if (o.gid & (1 << 29))
					{
						transform.flip_y();
						transform.rotate(-0.5f * MathUtil::PI_32);
					}

					transform.scale(o.width_pixel / (f32)texture.get_width(), o.height_pixel / (f32)texture.get_height());

					graphics.draw_image(texture, transform);
				}
			}
		}
	}
}