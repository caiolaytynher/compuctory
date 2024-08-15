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
    Allocator allocator;
} Sprite;

// #Global
Sprite sprites[SPRITE_COUNT] = {0};

inline Sprite* sprite_get(Sprite_Tag tag) { return &sprites[tag]; }

void sprite_load(
    Sprite* sprite, string path, Vector2 dimentions, Allocator allocator
) {
    sprite->image = load_image_from_disk(path, allocator);
    sprite->dimentions = dimentions;
    sprite->allocator = allocator;
}

void sprite_unload(Sprite* sprite) {
    dealloc(sprite->allocator, sprite->image);
    sprite->image = NULL;
}

void sprite_load_all(Allocator allocator) {
    sprite_load(
        &sprites[SPRITE_UNKNOWN], STR("assets/sprites/unknown.png"),
        v2(5.0f, 7.0f), allocator
    );
    sprite_load(
        &sprites[SPRITE_PLAYER], STR("assets/sprites/player.png"),
        v2(7.0f, 6.0f), allocator
    );
    sprite_load(
        &sprites[SPRITE_TREE_1], STR("assets/sprites/tree_1.png"),
        v2(9.0f, 14.0f), allocator
    );
    sprite_load(
        &sprites[SPRITE_TREE_2], STR("assets/sprites/tree_2.png"),
        v2(9.0f, 13.0f), allocator
    );
    sprite_load(
        &sprites[SPRITE_TREE_3], STR("assets/sprites/tree_3.png"),
        v2(9.0f, 14.0f), allocator
    );
    sprite_load(
        &sprites[SPRITE_ROCK_1], STR("assets/sprites/rock_1.png"),
        v2(5.0f, 2.0f), allocator
    );
    sprite_load(
        &sprites[SPRITE_ROCK_2], STR("assets/sprites/rock_2.png"),
        v2(5.0f, 3.0f), allocator
    );
    sprite_load(
        &sprites[SPRITE_ROCK_3], STR("assets/sprites/rock_3.png"),
        v2(5.0f, 3.0f), allocator
    );
}

void sprite_unload_all() {
    for (u64 i = 0; i < SPRITE_COUNT; i++) {
        sprite_unload(&sprites[i]);
    }
}

typedef enum Entity_Tag {
    ENTITY_UNKNOWN,

    ENTITY_PLAYER,
    ENTITY_TREE,
    ENTITY_ROCK,

    ENTITY_COUNT,
} Entity_Tag;

typedef struct Entity {
    Vector2 position;
    Sprite_Tag sprite_tag;
    Entity_Tag entity_tag;
} Entity;

typedef struct Entity_Pool {
    Entity* buffer;
    u64 capacity;
    u64 length;
    Allocator allocator;
} Entity_Pool;

typedef struct World {
    Entity_Pool* entity_pool;
} World;

// #Global
World world = {0};

void world_init(u64 entity_pool_capacity, Allocator allocator) {
    u64 allocation_size
        = sizeof(Entity_Pool) + entity_pool_capacity * sizeof(Entity);
    world.entity_pool = (Entity_Pool*)alloc(allocator, allocation_size);
    memset(world.entity_pool, 0, allocation_size);

    world.entity_pool->buffer = (Entity*)(world.entity_pool + 1);
    world.entity_pool->capacity = entity_pool_capacity;
    world.entity_pool->length = 0;
    world.entity_pool->allocator = allocator;
}

void world_deinit() {
    dealloc(world.entity_pool->allocator, world.entity_pool);
    world.entity_pool = NULL;
}

Entity* entity_fetch() {
    assert(
        world.entity_pool->length < world.entity_pool->capacity,
        "Not enought space in the entity pool."
    );

    Entity* entity = &world.entity_pool->buffer[world.entity_pool->length + 1];
    world.entity_pool->length++;

    return entity;
}

Entity* player_init() {
    Entity* player = entity_fetch();

    player->entity_tag = ENTITY_PLAYER;
    player->sprite_tag = SPRITE_PLAYER;

    return player;
}

Entity* tree_init() {
    Entity* tree = entity_fetch();

    tree->entity_tag = ENTITY_TREE;
    tree->sprite_tag = get_random_int_in_range(SPRITE_TREE_1, SPRITE_TREE_3);
    tree->position = v2(
        get_random_float32_in_range(
            -(f32)window.width / 2.0f, (f32)window.width / 2.0f
        ),
        get_random_float32_in_range(
            -(f32)window.height / 2.0f, (f32)window.height / 2.0f
        )
    );

    return tree;
}

Entity* rock_init() {
    Entity* rock = entity_fetch();

    rock->entity_tag = ENTITY_ROCK;
    rock->sprite_tag = get_random_int_in_range(SPRITE_ROCK_1, SPRITE_ROCK_3);
    rock->position = v2(
        get_random_float32_in_range(
            -(f32)window.width / 2.0f, (f32)window.width / 2.0f
        ),
        get_random_float32_in_range(
            -(f32)window.height / 2.0f, (f32)window.height / 2.0f
        )
    );

    return rock;
}

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
    Entity* player = player_init();
    // Centered on x axis
    player->position.x = -sprite_get(player->sprite_tag)->dimentions.x / 2.0f;

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
        player->position = v2_add(
            player->position, v2_mulf(player_move_direction, player_speed * dt)
        );

        Matrix4 player_xform = m4_scalar(1.0f);
        player_xform = m4_translate(
            player_xform, v3(player->position.x, player->position.y, 0.0f)
        );

        draw_image_xform(
            sprite_get(player->sprite_tag)->image, player_xform,
            sprite_get(player->sprite_tag)->dimentions, COLOR_WHITE
        );

        os_update();
        gfx_update();
    }

    sprite_unload_all();
    world_deinit();

    return 0;
}