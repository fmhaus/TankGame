#include "engine/Window.h"
#include "engine/Camera.h"
#include "engine/Graphics.h"
#include "engine/Font.h"
#include "entities/World.h"
#include "entities/World.h"
#include "entities/Tank.h"
#include "entities/Components.h"
#include "entities/Map.h"
#include "entities/Projectile.h"
#include "entities/Particle.h"
#include "AssetManager.h"

#include <iostream>
#include <sstream>
#include <entt/entt.hpp>

const static u32 PIXEL_SCALE = 128;

const static f32 CAMERA_MOVE_SPEED = 1.0;
const static f32 CAMERA_ZOOM_SPEED = 0.1;

static void player_control_camera(Window& window, Camera& camera)
{
    f32 frame_time = window.get_last_frame_time();
    camera.h_scope *= 1.0 - CAMERA_ZOOM_SPEED * window.get_wheel_delta().y;
    camera.h_scope = std::max(camera.h_scope, 1.0f);
    if (window.is_key_pressed(GLFW_KEY_A))
        camera.center.x -= CAMERA_MOVE_SPEED * camera.h_scope * frame_time;
    if (window.is_key_pressed(GLFW_KEY_D))
        camera.center.x += CAMERA_MOVE_SPEED * camera.h_scope * frame_time;
    if (window.is_key_pressed(GLFW_KEY_S))
        camera.center.y += CAMERA_MOVE_SPEED * camera.h_scope * frame_time;
    if (window.is_key_pressed(GLFW_KEY_W))
        camera.center.y -= CAMERA_MOVE_SPEED * camera.h_scope * frame_time;
    camera.update_matrix();
}

struct InputButton
{
    GLFWKey key;
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

void set_text(TextMesh& text, TankDesign design)
{
    std::ostringstream oss;
    oss << "Color:" << (design.color + 1) << " Hull:" << (design.hull + 1) << " Turret:" << (design.turret + 1) << " Tracks:" << (design.tracks + 1);
    TextBuildSettings settings;
    settings.horizontal_align = HorizontalAlign::Left;
    text.load_text(oss.str(), settings);
}

void open_client()
{
    AssetManager asset_manager(PIXEL_SCALE);

    WindowCreation window_data{ 1280, 720, "TankGame", FullscreenMode::Windowed, true, true };
    Window window(window_data);

    World world;

    Graphics graphics(window.get_width(), window.get_height(), PIXEL_SCALE);

    entt::entity map_entity = Map::create_map_entity(world.registry, RESOURCES_PATH "maps/map1.tmx", PIXEL_SCALE);
    world.registry.get<Map>(map_entity).set_full_screen_camera(graphics.camera);
    graphics.camera.update_matrix();

    Tileset tileset(RESOURCES_PATH "images/map/Tileset.tsx", PIXEL_SCALE);
    Map::create_map_renderable(world.registry, map_entity, tileset);
    Map::create_map_physics(world.registry, map_entity, tileset);

    window.set_on_resize([&](u32 width, u32 height)
        {
            world.registry.get<Map>(map_entity).set_full_screen_camera(graphics.camera);
            graphics.update_window_dimensions(width, height);
        });

    auto tank = Tank::create_tank(world.registry, asset_manager.tank_assets, TankDesign{ 2, 0, 0, 0 }, glm::vec2(3.0f, 8.0f), true);
  
    std::vector<glm::vec2> polygon;
    polygon.push_back(glm::vec2(5.0f, 5.0f));
    polygon.push_back(glm::vec2(5.0f, 7.0f));
    polygon.push_back(glm::vec2(6.0f, 6.0f));
    polygon.push_back(glm::vec2(6.0f, 5.0f));

    bool is_space_down = false;

    InputButton shoot(GLFW_KEY_SPACE);
    InputButton next_color(GLFW_KEY_H);
    InputButton next_hull(GLFW_KEY_J);
    InputButton next_turret(GLFW_KEY_K);
    InputButton next_tracks(GLFW_KEY_L);
    InputButton prev_color(GLFW_KEY_B);
    InputButton prev_hull(GLFW_KEY_N);
    InputButton prev_turret(GLFW_KEY_M);
    InputButton prev_tracks(GLFW_KEY_SEMICOLON);
    InputButton reload(GLFW_KEY_R);

    TankDesign design(0, 0, 0, 0);

    TextMesh text(asset_manager.font_sans_black);
    set_text(text, design);


    ProjectileType projectile_type;
    projectile_type.sprite_type = ProjectileSpriteType::HeavyShell;
    projectile_type.max_collisons = 0;

    while (!window.poll_events())
    {
        //player_control_camera(window, camera);

        world.set_physics_debug_draw_enabled(window.is_key_pressed(GLFW_KEY_F6));
        TankPlayerController::update_tank(world.registry, window, graphics, window.get_last_frame_time());
      
        if (shoot.is_released(window))
        {
            auto [tank_transform, controller] = world.registry.get<Transform,TankPlayerController>(tank);
            Projectile::create_projectile(world.registry, asset_manager, world.registry.get<Tank>(tank), projectile_type, 10.0f);

            Particle::create(world.registry, asset_manager.particle_explosion[0], world.registry.get<Transform>(tank), 10.0f);
        }

        if (next_color.is_released(window))
        {
            design.color = (design.color + 1) % 4;
            Tank::update_tank_design(world.registry, tank, design, asset_manager.tank_assets);
            set_text(text, design);
        }

        if (next_hull.is_released(window))
        {
            design.hull = (design.hull + 1) % 8;
            Tank::update_tank_design(world.registry, tank, design, asset_manager.tank_assets);
            set_text(text, design);
        }

        if (next_turret.is_released(window))
        {
            design.turret = (design.turret + 1) % 8;
            Tank::update_tank_design(world.registry, tank, design, asset_manager.tank_assets);
            set_text(text, design);
        }

        if (next_tracks.is_released(window))
        {
            design.tracks = (design.tracks + 1) % 4;
            Tank::update_tank_design(world.registry, tank, design, asset_manager.tank_assets);
            set_text(text, design);
        }

        if (prev_color.is_released(window))
        {
            design.color = (design.color + 3) % 4;
            Tank::update_tank_design(world.registry, tank, design, asset_manager.tank_assets);
            set_text(text, design);
        }

        if (prev_hull.is_released(window))
        {
            design.hull = (design.hull + 7) % 8;
            Tank::update_tank_design(world.registry, tank, design, asset_manager.tank_assets);
            set_text(text, design);
        }

        if (prev_turret.is_released(window))
        {
            design.turret = (design.turret + 7) % 8;
            Tank::update_tank_design(world.registry, tank, design, asset_manager.tank_assets);
            set_text(text, design);
        }

        if (prev_tracks.is_released(window))
        {
            design.tracks = (design.tracks + 3) % 4;
            Tank::update_tank_design(world.registry, tank, design, asset_manager.tank_assets);
            set_text(text, design);
        }

        if (reload.is_released(window))
        {
            world.registry.remove<TankRenderable>(tank);
            world.registry.emplace<TankRenderable>(tank, asset_manager.tank_assets, design);
        }


        world.update((f32)window.get_last_frame_time());

        world.render(graphics);

        TextStyleSettings settings;
        settings.thickness = 0.495f;
        graphics.draw_text(text, 50, 50, settings);

        std::ostringstream oss;
        oss << "TankGame " << (1.0 / window.get_last_frame_time());
        window.set_title(oss.str().c_str());

        window.swap_buffers();
    }

    // This will release all asset refs stored inside entities
    world.registry.clear();
}

int main()
{
    try {
        open_client();
    } 
    catch (std::exception& e)
    {
        std::cerr << "Exception in open_client(): " << e.what() << std::endl;
    }
}