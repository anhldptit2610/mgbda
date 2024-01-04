#include "common.h"
#include "gb.h"

struct gb *gb_create(void)
{
    struct gb *gb = malloc(sizeof(struct gb));

    if (!gb) {
        printf("[ERROR] Can't create the system\n");
    }

    gb->rom.data = NULL;
    return gb;
}

void gb_destroy(struct gb *gb)
{
    free(gb);
}

