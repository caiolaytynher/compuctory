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

inline Sprite* entity_get_sprite(Entity* entity) {
    return &sprites[entity->sprite_tag];
}

void entity_render(Entity* entity) {
    Matrix4 xform = m4_scalar(1.0f);
    xform = m4_translate(xform, v3(v2_expand(entity->position), 0.0f));
    Sprite* sprite = entity_get_sprite(entity);
    draw_image_xform(sprite->image, xform, sprite->dimentions, COLOR_WHITE);
}