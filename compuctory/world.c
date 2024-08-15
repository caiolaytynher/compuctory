typedef struct World {
    Entity_Pool* entities;
} World;

// #Global
World world = {0};

void world_init(u64 entity_capacity, Allocator allocator) {
    u64 allocation_size
        = sizeof(Entity_Pool) + entity_capacity * sizeof(Entity);
    world.entities = alloc(allocator, allocation_size);
    memset(world.entities, 0, allocation_size);

    world.entities->allocator = allocator;
    world.entities->capacity = entity_capacity;
    world.entities->length = 0;

    world.entities->buffer = (Entity*)(world.entities + 1);
}

void world_deinit() {
    dealloc(world.entities->allocator, world.entities);
    world.entities = NULL;
}

Entity* world_request_entity() {
    assert(
        world.entities->length < world.entities->capacity,
        "Not enought space in the entity pool."
    );

    Entity* entity = &world.entities->buffer[world.entities->length];
    world.entities->length++;

    return entity;
}

Entity* world_create_player() {
    Entity* player = world_request_entity();

    player->entity_tag = ENTITY_PLAYER;
    player->sprite_tag = SPRITE_PLAYER;

    return player;
}

Entity* world_create_tree() {
    Entity* tree = world_request_entity();

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

Entity* world_create_rock() {
    Entity* rock = world_request_entity();

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