#include "sprite.c"

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

typedef struct Entity_Pool_Header {
    u64 capacity;
    u64 length;
    Allocator allocator;
} Entity_Pool_Header;

typedef struct World {
    Entity* entity_pool;
} World;

// #Global
World world = {0};

#define ENTITY_POOL_HEADER ((Entity_Pool_Header*)world.entity_pool - 1)
inline u64 entity_pool_length() { return ENTITY_POOL_HEADER->length; }
inline u64 entity_pool_capacity() { return ENTITY_POOL_HEADER->capacity; }

void world_init(u64 entity_pool_capacity, Allocator allocator) {
    u64 allocation_size
        = sizeof(Entity_Pool_Header) + entity_pool_capacity * sizeof(Entity);
    Entity_Pool_Header* entity_pool_header = alloc(allocator, allocation_size);
    memset(entity_pool_header, 0, allocation_size);

    entity_pool_header->allocator = allocator;
    entity_pool_header->capacity = entity_pool_capacity;
    entity_pool_header->length = 0;

    world.entity_pool = (Entity*)(entity_pool_header + 1);
}

void world_deinit() {
    dealloc(ENTITY_POOL_HEADER->allocator, ENTITY_POOL_HEADER);
    world.entity_pool = NULL;
}

Entity* entity_fetch() {
    assert(
        entity_pool_length() < entity_pool_capacity(),
        "Not enought space in the entity pool."
    );

    Entity* entity = &world.entity_pool[entity_pool_length()];
    ENTITY_POOL_HEADER->length++;

    return entity;
}

inline Sprite* entity_sprite(Entity* entity) {
    return &sprites[entity->sprite_tag];
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

void entity_render(Entity* entity) {
    Matrix4 xform = m4_scalar(1.0f);
    xform = m4_translate(xform, v3(v2_expand(entity->position), 0.0f));
    Sprite* sprite = entity_sprite(entity);
    draw_image_xform(sprite->image, xform, sprite->dimentions, COLOR_WHITE);
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
    // Centered in x axis
    player->position.x = -entity_sprite(player)->dimentions.x / 2.0f;

    for (u8 i = 0; i < 100; i++) {
        tree_init();
        rock_init();
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

        for (u64 i = 0; i < entity_pool_length(); i++) {
            Entity* entity = &world.entity_pool[i];
            entity_render(entity);
        }

        os_update();
        gfx_update();
    }

    sprite_unload_all();
    world_deinit();

    return 0;
}