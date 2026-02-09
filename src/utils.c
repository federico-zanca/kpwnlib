/**
 * src/utils.c — Common utility implementations
 */
#define _GNU_SOURCE
#include <kpwn/utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>

/* Private state */
static int g_kpwn_debug = 0;

/* Debug control */
void utils_init(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "DEBUG") || !strcmp(argv[i], "debug")) {
            utils_set_debug(1);
            return;
        }
    }
}

void utils_set_debug(int enabled)
{
    g_kpwn_debug = !!enabled;
    if (g_kpwn_debug)
        WARN("Debug mode ON — DEBUG() will pause after each message");
}

void utils_debug_pause(void)
{
    dprintf(2, _CLR_YELLOW "    [press Enter to continue]" _CLR_RESET);
    char buf[16];
    (void)read(0, buf, sizeof(buf));
}


/* Device interaction  */
int open_dev(const char *path, int flags)
{
    int fd = open(path, flags);
    if (fd < 0)
        DIE("open(%s) failed", path);
    OK("Opened %s  →  fd %d", path, fd);
    return fd;
}



void hexdump(const void *addr, size_t len)
{
    int more = len % 0x10;
    u64 i;
    printf("\n------------------------HexDump------------------------\n");
    for(i = 0 ; i < (len / 0x10); i++){
        printf("0x%016lx:\t0x%016lx\t0x%016lx\n", i*0x10, *(u64 *)(addr + i*0x10), *(u64 *)(addr + i*0x10 + 8));
    }

    if(more != 0){
        printf("0x%016lx:\t", i*0x10);
        int offset = 0;
        if(more > 8){
            printf("%016lx", *(u64 *)(addr + i*0x10));
            offset = 8;
        }

        printf("0x");
        for(int j=0; j<8-more; j++){
            printf("  ");
        }
        for(int j=8-more; j<8; j++){
            printf("%02x", *(u8 *)(addr + i*0x10 + offset + (8-1-j)));
        }
    }
    puts("\n-------------------------------------------------------\n");

}



int pin_cpu(int cpu)
{
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu, &set);
    if (sched_setaffinity(0, sizeof(set), &set) < 0) {
        ERR("pin_cpu(%d) failed", cpu);
        return -1;
    }
    INFO("Pinned to CPU %d", cpu);
    return 0;
}


void get_shell(void)
{
    INFO("uid = %d", getuid());
    if (getuid() == 0)
        OK("Got root!");
    else
        ERR("Not root... privesc may have failed");

    char *argv[] = { "/bin/sh", NULL };
    execve(argv[0], argv, NULL);
}

void save_state(user_state_t *state)
{
    __asm__ volatile(
        "mov %0, cs;"
        "mov %1, ss;"
        "mov %2, rsp;"
        "pushfq;"
        "pop %3;"
        : "=r"(state->cs),
          "=r"(state->ss),
          "=r"(state->rsp),
          "=r"(state->rflags)
        :
        : "memory"
    );
    INFO("Saved state");
}

void restore_state(user_state_t *state)
{
    __asm__ volatile(
        "mov rsp, %0;"
        "push %1;"       // SS
        "push %0;"       // RSP
        "push %2;"       // RFLAGS
        "push %3;"       // CS
        "iretq;"
        :
        : "r"(state->rsp),
          "r"(state->ss),
          "r"(state->rflags),
          "r"(state->cs)
        : "memory"
    );
}

void test_user_space_func(void)
{
    OK("test_user_space_func executed");
}
