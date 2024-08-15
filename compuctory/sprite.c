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