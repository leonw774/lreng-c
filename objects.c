#include "objects.h"

const object_t const RESERVED_OBJS[RESERVED_ID_NUM] = {
    (object_t) {.type = OBJ_NULL, .data = {.null = 0}},
    (object_t) {.type = OBJ_BUILTIN_FUNC, .data = {
        .builtin_func = {.name = RESERVED_ID_INPUT}
    }},
    (object_t) { .type = OBJ_BUILTIN_FUNC, .data = {
        .builtin_func = {.name = RESERVED_ID_OUTPUT}
    }}
};

extern void free_object(object_t* obj) {
    if (obj->type == OBJ_NUMBER) {
        bi_free(&obj->data.number.numer);
        bi_free(&obj->data.number.denom);
    }
    else if (obj->type == OBJ_PAIR) {
        free_object(obj->data.pair.left);
        free_object(obj->data.pair.right);
    }
}

