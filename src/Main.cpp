#include "engine/Window.h"
#include "engine/Camera.h"
#include "engine/Graphics.h"
#include "engine/Font.h"
#include "engine/UI.h"
#include "entities/World.h"
#include "entities/Tank.h"
#include "entities/Components.h"
#include "entities/Map.h"
#include "entities/Projectile.h"
#include "entities/Particle.h"

#include <iostream>
#include <sstream>
#include <entt/entt.hpp>

const static f32 CAMERA_MOVE_SPEED = 1.0;
const static f32 CAMERA_ZOOM_SPEED = 0.1;

static void player_control_camera(Window& window, Camera& camera)
{
    f32 frame_time = window.get_last_frame_time();
    camera.h_scope *= 1.0 - CAMERA_ZOOM_SPEED * window.get_wheel_delta().y;
    camera.h_scope = std::max(camera.h_scope, 1.0f);
    if (window.is_key_pressed(KEY_A))
        camera.center.x -= CAMERA_MOVE_SPEED * camera.h_scope * frame_time;
    if (window.is_key_pressed(KEY_D))
        camera.center.x += CAMERA_MOVE_SPEED * camera.h_scope * frame_time;
    if (window.is_key_pressed(KEY_S))
        camera.center.y += CAMERA_MOVE_SPEED * camera.h_scope * frame_time;
    if (window.is_key_pressed(KEY_W))
        camera.center.y -= CAMERA_MOVE_SPEED * camera.h_scope * frame_time;
    camera.update_matrix();
}

struct ReleaseKey
{
    InputKey key;
    bool state = false;

    bool is_released(Window& window)
    {
        bool now = window.is_key_pressed(key);
        if (state && !now)
        {
            state = now;
            return true;
        }
        state = now;
        return false;
    }
};

void open_client()
{
    WindowCreation window_data{ 1280, 720, "TankGame", FullscreenMode::Windowed, true, true };
    Window& window = Window::create_window(window_data);

    World world;

    Graphics graphics(window.get_width(), window.get_height(), PIXEL_SCALE);

    entt::entity map_entity = Map::create_map_entity(world.registry, RESOURCES_PATH "maps/map1.tmx", PIXEL_SCALE);
    Map::set_full_screen_camera(world.registry, map_entity, graphics.camera);
    graphics.camera.update_matrix();

    Tileset tileset(RESOURCES_PATH "images/map/Tileset.tsx", PIXEL_SCALE);
    Map::create_map_renderable(world.registry, map_entity, tileset);
    Map::create_map_physics(world.registry, map_entity, tileset);

    window.set_on_resize([&](u32 width, u32 height)
        {
            Map::set_full_screen_camera(world.registry, map_entity, graphics.camera);
            graphics.update_window_dimensions(width, height);
        });

    auto tank_entity = Tank::create_tank(world.registry, TankDesign{ 2, 0, 0, 0 }, glm::vec2(3.0f, 8.0f), true);

    ReleaseKey shoot(KEY_SPACE);

    ProjectileType projectile_type;
    projectile_type.sprite_type = ProjectileSpriteType::Laser;
    projectile_type.velocity = 15.0f;
    projectile_type.max_collisons = 3;
    projectile_type.fix_orientation = true;
    projectile_type.fix_velocity = true;
    projectile_type.restitution = 1.0f;
    projectile_type.density = 200;
    projectile_type.particle_type = 1;
    projectile_type.allow_projectile_collision = false;

    ProjectileType projectile_type_2;
    projectile_type.sprite_type = ProjectileSpriteType::HeavyShell;
    projectile_type.velocity = 15.0f;
    projectile_type.max_collisons = 0;
    projectile_type.fix_orientation = true;
    projectile_type.fix_velocity = true;
    projectile_type.restitution = 1.0f;
    projectile_type.density = 200;
    projectile_type.particle_type = 0;
    projectile_type.allow_projectile_collision = true;

    world.registry.get<TankPlayerController>(tank_entity).projectile_type = projectile_type;

    while (!window.poll_events())
    {
		world.handle_inputs(window.get_last_frame_time(), graphics.camera);

        world.update((f32)window.get_last_frame_time());

        world.set_physics_debug_draw_enabled(window.is_key_pressed(KEY_F6));
        world.render(graphics);

        std::ostringstream oss;
        oss << "TankGame " << (1.0 / window.get_last_frame_time());
        window.set_title(oss.str().c_str());

        window.swap_buffers();
    }

    // This will release all asset refs stored inside entities
    world.registry.clear();

	Window::destroy_window();
}

int main()
{
    open_client();
}