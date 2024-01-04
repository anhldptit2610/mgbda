#include "cpu.h"
#include "rom.h"

int main(int argc, char *argv[])
{
    struct gb *gb = gb_create();

    if (argc < 2) {
        printf("Need to supply a ROM file to run\n");
        exit(EXIT_SUCCESS);
    }

    if (!gb)
        exit(EXIT_FAILURE);
    cpu_init(gb);
    rom_load(gb, argv[1]);
    while (1) {
        cpu_step(gb);
    }
    gb_destroy(gb);
    return 0;
}