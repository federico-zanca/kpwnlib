/**
 * kpwn/spray.h — Kernel heap spray primitives
 *
 * Provides helpers for the common spray objects:
 *   - msg_msg / msg_msgseg   (msgsnd/msgrcv)
 *   - pipe_buffer            (pipe splice)
 *   - sk_buff                (socket sendmsg)
 *   - pg_vec                 (packet mmap)
 *   - add_key / keyctl
 *
 * Corresponding implementation: src/spray.c
 */
#ifndef KPWN_SPRAY_H
#define KPWN_SPRAY_H

#include "types.h"
#include "log.h"
#include "utils.h"


int resizePipeBuffer(int pipe[2], size_t kmalloc_target_size);


/* TODO: Add your declarations here.  Example stubs:
 *
 * int  spray_msg_alloc(int qid, const void *data, size_t size);
 * int  spray_msg_recv(int qid, void *buf, size_t size, int flags);
 *
 * void spray_pipes_open(int fds[][2], int count);
 * void spray_pipes_close(int fds[][2], int count);
 * void spray_pipes_write(int fds[][2], int count,
 *                        const void *data, size_t size);
 *
 * void spray_skbuff_alloc(int sk[][2], int count,
 *                         const void *data, size_t size);
 * void spray_skbuff_free(int sk[][2], int count);
 */

#endif /* KPWN_SPRAY_H */
