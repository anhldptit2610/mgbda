#include <common.h>
#include <gb.h>
#include <cpu.h>
#include <cjson/cJSON.h>

struct cpu_state {
    uint16_t pc;
    uint16_t sp;
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t f;
    uint8_t h;
    uint8_t l;
    uint8_t ime;
    uint8_t ei;
    struct mem {
        uint16_t addr;
        uint8_t val;
    } mem[100];
    int mem_index;
};

void read_json(char buffer[1000][1024], FILE *fp)
{
    for (int i = 0; i < 1000; i++) {
        int j = 0, times = 0, c;

        while (times < 3) {
            if ((c = fgetc(fp)) == '}')
                times++;
            buffer[i][j++] = c;
        }
        buffer[i][j++] = '\0';
        fgetc(fp);  // read the , character
    }
}

void setup_test(char *json_buffer, struct gb *gb, char *instr, struct cpu_state *initial_state, 
                                struct cpu_state *final_state)
{
    uint16_t state_buffer[12], mem_buffer[2];
    cJSON *element = NULL, *tmp = NULL, *j = NULL;
    int i, k;

    cJSON *test = cJSON_Parse(json_buffer);
    if (!test) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
            printf("Error: %s\n", error_ptr);
        cJSON_Delete(test);
    }

    cJSON *name = cJSON_GetObjectItemCaseSensitive(test, "name");
    if (cJSON_IsString(name)) {
        strcpy(instr, name->valuestring);
    }
    cJSON *initial = cJSON_GetObjectItemCaseSensitive(test, "initial");
    if (initial) {
        tmp = initial->child;
        i = 0;

        while (tmp->next) {
            state_buffer[i++] = tmp->valueint;
            tmp = tmp->next;
        }
        cJSON_ArrayForEach(element, tmp)
        {
            k = 0;
            cJSON_ArrayForEach(j, element)
            {
                mem_buffer[k++] = j->valueint;
            }
            gb->mem[mem_buffer[0]] = mem_buffer[1];
            initial_state->mem[initial_state->mem_index].addr = mem_buffer[0];
            initial_state->mem[initial_state->mem_index].val = mem_buffer[1];
            initial_state->mem_index++;
        }
    }
    initial_state->pc = gb->cpu.regs.pc = state_buffer[0];
    initial_state->sp = gb->cpu.regs.sp = state_buffer[1];
    initial_state->a = gb->cpu.regs.a = state_buffer[2];
    initial_state->b = gb->cpu.regs.b = state_buffer[3];
    initial_state->c = gb->cpu.regs.c = state_buffer[4];
    initial_state->d = gb->cpu.regs.d = state_buffer[5];
    initial_state->e = gb->cpu.regs.e = state_buffer[6];
    initial_state->f = gb->cpu.regs.f = state_buffer[7];
    initial_state->h = gb->cpu.regs.h = state_buffer[8];
    initial_state->l = gb->cpu.regs.l = state_buffer[9];

    cJSON *final = cJSON_GetObjectItemCaseSensitive(test, "final");
    if (final) {
        tmp = final->child;
        i = 0;

        while (tmp->next) {
            state_buffer[i++] = tmp->valueint;
            tmp = tmp->next;
        }
        cJSON_ArrayForEach(element, tmp)
        {
            k = 0;
            cJSON_ArrayForEach(j, element)
            {
                mem_buffer[k++] = j->valueint;
            }
            final_state->mem[final_state->mem_index].addr = mem_buffer[0];
            final_state->mem[final_state->mem_index].val = mem_buffer[1];
            final_state->mem_index++;
        }
        final_state->a = state_buffer[0];
        final_state->b = state_buffer[1];
        final_state->c = state_buffer[2];
        final_state->d = state_buffer[3];
        final_state->e = state_buffer[4];
        final_state->f = state_buffer[5];
        final_state->h = state_buffer[6];
        final_state->l = state_buffer[7];
        final_state->pc = state_buffer[8];
        final_state->sp = state_buffer[9];
    }
    cJSON_Delete(test);
}

int check_test(struct gb *gb, struct cpu_state *final_state)
{
    int ret = 0;

    // check register
    if (gb->cpu.regs.a != final_state->a || gb->cpu.regs.b != final_state->b || gb->cpu.regs.c != final_state->c ||
       gb->cpu.regs.d != final_state->d || gb->cpu.regs.e != final_state->e || gb->cpu.regs.f != final_state->f ||
       gb->cpu.regs.h != final_state->h || gb->cpu.regs.l != final_state->l)
        ret = 1;
    // check memory
    for (int i = 0; i < final_state->mem_index; i++) {
        uint16_t addr = final_state->mem[i].addr;
        if (gb->mem[addr] != final_state->mem[i].val)
            ret = 1;
    }
    return ret;
}

int main(int argc, char *argv[])
{
    char json_buffer[1000][1024];
    uint16_t cpu_state_buffer[12];
    struct gb *gb = NULL;
    struct cpu_state final_state, initial_state;

    if (argc < 2) {
        fprintf(stderr, "Missing json file path\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "testing file %s\n", argv[1]);
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "Can't open file %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    fgetc(fp);      // read [ character


    // read 1000 json tests per file
    final_state.mem_index = 0;
    initial_state.mem_index = 0;
    read_json(json_buffer, fp);

    // setup and run the test
    int ret = 0;
    char name[1000];
    for (int i = 0; i < 1000; i++) {
        gb = gb_create();
        final_state.mem_index = 0;
        initial_state.mem_index = 0;
        setup_test(json_buffer[i], gb, name, &initial_state, &final_state);
        cpu_step(gb); 

        // check if the result is ok or not
        if (check_test(gb, &final_state)) {
            printf("Test failed.\n");
            printf("Instructions: %s", name);
            printf("\n---------------------------------\n");
            printf("Initial state:\n");
            printf("pc: %04x sp: %04x a: %02x b: %02x c: %02x d: %02x e: %02x f: %02x h: %02x l: %02x\n", 
                        initial_state.pc, initial_state.sp, initial_state.a, initial_state.b,
                        initial_state.c, initial_state.d, initial_state.e, initial_state.f, 
                        initial_state.h, initial_state.l);
            printf("mem: ");
            for (int i = 0; i < final_state.mem_index; i++) {
                printf("%04x - %02x ", final_state.mem[i].addr, final_state.mem[i].val);
            }
            printf("\n---------------------------------\n");
            printf("CPU state:\n");
            printf("pc: %04x sp: %04x a: %02x b: %02x c: %02x d: %02x e: %02x f: %02x h: %02x l: %02x\n",
                        gb->cpu.regs.pc, gb->cpu.regs.sp, gb->cpu.regs.a, gb->cpu.regs.b,
                        gb->cpu.regs.c, gb->cpu.regs.d, gb->cpu.regs.e, gb->cpu.regs.f, 
                        gb->cpu.regs.h, gb->cpu.regs.l);
            printf("mem: ");
            for (int i = 0; i < final_state.mem_index; i++) {
                printf("%04x - %02x ", final_state.mem[i].addr, gb->mem[final_state.mem[i].addr]);
            }
            printf("\n---------------------------------\n");
            printf("final state:\n");
            printf("pc: %04x sp: %04x a: %02x b: %02x c: %02x d: %02x e: %02x f: %02x h: %02x l: %02x\n", 
                        final_state.pc, final_state.sp, final_state.a, final_state.b,
                        final_state.c, final_state.d, final_state.e, final_state.f, 
                        final_state.h, final_state.l);
            printf("mem: ");
            for (int i = 0; i < final_state.mem_index; i++) {
                printf("%04x - %02x ", final_state.mem[i].addr, final_state.mem[i].val);
            }
            printf("\n");
            ret = 1;
            exit(EXIT_FAILURE);
        }
        gb_destroy(gb);
    }
    if (!ret)
        printf("test %s ok.\n", argv[1]);
    return 0;
}