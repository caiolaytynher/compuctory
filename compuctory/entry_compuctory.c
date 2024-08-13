int entry(int argc, char** argv) {
    window.title = STR("Compuctory");
    window.scaled_width = 1280; // We need to set the scaled size if we want to
                                // handle system scaling (DPI)
    window.scaled_height = 720;
    window.x = 200;
    window.y = 90;
    window.clear_color = hex_to_rgba(0x1f262dff);

    Gfx_Image* player = load_image_from_disk(
        STR("assets/sprites/player.png"), get_heap_allocator()
    );
    Vector2 player_position = v2(0.0f, 0.0f);
    f32 player_speed = 1.0f;
    Vector2 sprite_size = v2(0.5f, 0.5f);

    f64 dt = 0.0;
    f64 last_time = os_get_current_time_in_seconds();
    while (!window.should_close) {
        // Display FPS on the console
        f64 now = os_get_current_time_in_seconds();
        if ((int)now != (int)last_time) {
            log("%.2f FPS\n%.2f ms", 1.0 / (now - last_time),
                (now - last_time) * 1000);
        }
        dt = now - last_time;
        last_time = now;

        reset_temporary_storage();

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
        player_position = v2_add(
            player_position, v2_mulf(player_move_direction, player_speed * dt)
        );

        Matrix4 player_xform = m4_scalar(1.0f);
        player_xform = m4_translate(
            player_xform, v3(player_position.x, player_position.y, 0.0f)
        );

        draw_image_xform(player, player_xform, sprite_size, COLOR_WHITE);

        os_update();
        gfx_update();
    }

    return 0;
}