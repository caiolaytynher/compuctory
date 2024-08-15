#include "sprite.c"
#include "entity.c"
#include "world.c"

int entry(int argc, char** argv) {
    window.title = STR("Compuctory");
    window.scaled_width = 1280; // We need to set the scaled size if we want to
                                // handle system scaling (DPI)
    window.scaled_height = 720;
    window.x = 200;
    window.y = 90;
    window.clear_color = hex_to_rgba(0x1f262dff);

    sprite_load_all(get_heap_allocator());
    world_init(1024, get_heap_allocator());

    f32 player_speed = 100.0f;
    Entity* player = world_create_player();
    // Centered in x axis
    player->position.x = -entity_get_sprite(player)->dimentions.x / 2.0f;

    for (u8 i = 0; i < 100; i++) {
        world_create_tree();
        world_create_rock();
    }

    // Make it 1:1 to pixel size
    Matrix4 projection = m4_make_orthographic_projection(
        -(f32)window.width / 2.0f, (f32)window.width / 2.0f,
        -(f32)window.height / 2.0f, (f32)window.height / 2.0f, -1, -10
    );

    // Zoom to the appropriate viewport
    f32 viewport_width = 240.0f;
    // It's flipped because of ooga booga
    f32 view_scale_factor = viewport_width / window.width;
    Matrix4 view
        = m4_make_scale(v3(view_scale_factor, view_scale_factor, 1.0f));

    f64 dt = 0.0;
    f64 last_time = os_get_current_time_in_seconds();
    while (!window.should_close) {
        reset_temporary_storage();

        // Display FPS on the console
        f64 now = os_get_current_time_in_seconds();
        if ((int)now != (int)last_time) {
            log("%.2f FPS\n%.2f ms", 1.0 / (now - last_time),
                (now - last_time) * 1000);
        }
        dt = now - last_time;
        last_time = now;

        draw_frame.projection = projection;
        draw_frame.view = view;

        if (is_key_just_pressed(KEY_ESCAPE)) {
            window.should_close = true;
        }

        Vector2 player_move_direction = v2(0.0f, 0.0f);
        if (is_key_down('W')) {
            player_move_direction.y = 1;
        }
        if (is_key_down('A')) {
            player_move_direction.x = -1;
        }
        if (is_key_down('S')) {
            player_move_direction.y = -1;
        }
        if (is_key_down('D')) {
            player_move_direction.x = 1;
        }

        player_move_direction = v2_normalize(player_move_direction);
        player->position = v2_add(
            player->position, v2_mulf(player_move_direction, player_speed * dt)
        );

        for (u64 i = 0; i < world.entities->length; i++) {
            Entity* entity = &world.entities->buffer[i];
            entity_render(entity);
        }

        os_update();
        gfx_update();
    }

    sprite_unload_all();
    world_deinit();

    return 0;
}