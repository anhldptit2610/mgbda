#include "rom.h"

void rom_load(struct gb *gb, char *rom_path)
{
    FILE *fp = fopen(rom_path, "r");

    if (!fp) {
        printf("Can't open the rom file. Path: %s\n", rom_path);
        return;
    }
    fseek(fp, 0, SEEK_END);
    gb->rom.info.size = ftell(fp);
    rewind(fp);
    
    gb->rom.data = malloc(sizeof(uint8_t) * gb->rom.info.size);
    if (!gb->rom.data) {
        printf("Can't allocate memory for rom\n");
        exit(EXIT_FAILURE);
    }
    fread(gb->rom.data, sizeof(uint8_t), gb->rom.info.size, fp);
    gb->rom.info.loaded = true;
    fclose(fp);
}