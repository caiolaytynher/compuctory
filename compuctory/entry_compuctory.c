typedef enum Sprite_Tag {
    SPRITE_UNKNOWN,

    SPRITE_PLAYER,
    SPRITE_TREE_1,
    SPRITE_TREE_2,
    SPRITE_TREE_3,
    SPRITE_ROCK_1,
    SPRITE_ROCK_2,
    SPRITE_ROCK_3,

    SPRITE_COUNT,
} Sprite_Tag;

typedef struct Sprite {
    Vector2 dimentions;
    Gfx_Image* image;
} Sprite;

void sprite_load_all(Sprite sprites[SPRITE_COUNT], Allocator allocator) {
    sprites[SPRITE_UNKNOWN] = (Sprite){
        .image
        = load_image_from_disk(STR("assets/sprites/unknown.png"), allocator),
        .dimentions = v2(5.0f, 7.0f),
    };
    sprites[SPRITE_PLAYER] = (Sprite){
        .image
        = load_image_from_disk(STR("assets/sprites/player.png"), allocator),
        .dimentions = v2(7.0f, 6.0f),
    };
    sprites[SPRITE_TREE_1] = (Sprite){
        .image
        = load_image_from_disk(STR("assets/sprites/tree_1.png"), allocator),
        .dimentions = v2(9.0f, 14.0f),
    };
    sprites[SPRITE_TREE_2] = (Sprite){
        .image
        = load_image_from_disk(STR("assets/sprites/tree_2.png"), allocator),
        .dimentions = v2(9.0f, 13.0f),
    };
    sprites[SPRITE_TREE_3] = (Sprite){
        .image
        = load_image_from_disk(STR("assets/sprites/tree_3.png"), allocator),
        .dimentions = v2(9.0f, 14.0f),
    };
    sprites[SPRITE_ROCK_1] = (Sprite){
        .image
        = load_image_from_disk(STR("assets/sprites/rock_1.png"), allocator),
        .dimentions = v2(5.0f, 2.0f),
    };
    sprites[SPRITE_ROCK_2] = (Sprite){
        .image
        = load_image_from_disk(STR("assets/sprites/rock_2.png"), allocator),
        .dimentions = v2(5.0f, 3.0f),
    };
    sprites[SPRITE_ROCK_3] = (Sprite){
        .image
        = load_image_from_disk(STR("assets/sprites/rock_3.png"), allocator),
        .dimentions = v2(5.0f, 3.0f),
    };
}

typedef enum Entity_Tag {
    ENTITY_UNKNOWN,

    ENTITY_PLAYER,
    ENTITY_TREE,
    ENTITY_ROCK,

    ENTITY_COUNT,
} Entity_Tag;

typedef enum Entity_Flags {
    FLAG_ENTITY_IS_INITIALIZED = 1 << 0,
} Entity_Flags;

typedef struct Entity {
    Vector2 position;
    Sprite_Tag sprite_tag;
    Entity_Tag entity_tag;
    u8 flags;
} Entity;

void player_init(Entity* player) {
    player->entity_tag = ENTITY_PLAYER;
    player->sprite_tag = SPRITE_PLAYER;
    player->flags |= FLAG_ENTITY_IS_INITIALIZED;
}

int entry(int argc, char** argv) {
    window.title = STR("Compuctory");
    window.scaled_width = 1280; // We need to set the scaled size if we want to
                                // handle system scaling (DPI)
    window.scaled_height = 720;
    window.x = 200;
    window.y = 90;
    window.clear_color = hex_to_rgba(0x1f262dff);

    Sprite sprites[SPRITE_COUNT];
    sprite_load_all(sprites, get_heap_allocator());

    f32 player_speed = 100.0f;
    Entity player = {0};
    player_init(&player);
    // Centered on x axis
    player.position.x = -sprites[player.sprite_tag].dimentions.x / 2.0f;

    // Make it 1:1 to pixel size
    Matrix4 projection = m4_make_orthographic_projection(
        -(f32)window.width / 2.0f, (f32)window.width / 2.0f,
        -(f32)window.height / 2.0f, (f32)window.height / 2.0f, -1, -10
    );

    // Zoom to the appropriate viewport
    f32 viewport = 240.0f;
    Matrix4 view = m4_make_scale(
        v3(viewport / window.width, viewport / window.width, 1.0f)
    );

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
        player.position = v2_add(
            player.position, v2_mulf(player_move_direction, player_speed * dt)
        );

        Matrix4 player_xform = m4_scalar(1.0f);
        player_xform = m4_translate(
            player_xform, v3(player.position.x, player.position.y, 0.0f)
        );

        draw_image_xform(
            sprites[player.sprite_tag].image, player_xform,
            sprites[player.sprite_tag].dimentions, COLOR_WHITE
        );

        os_update();
        gfx_update();
    }

    return 0;
}