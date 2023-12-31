/*
 * Copyright (c) 2009-2011, Salvatore Sanfilippo <antirez at gmail dot com>
 * Copyright (c) 2010-2011, Pieter Noordhuis <pcnoordhuis at gmail dot com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __HIREDIS_H
#define __HIREDIS_H
#include <string.h>
#include <stdio.h> /* for size_t */
#include <stdarg.h> /* for va_list */
#include "sds.h"

#define HIREDIS_MAJOR 0
#define HIREDIS_MINOR 11
#define HIREDIS_PATCH 0

#define REDIS_ERR -1
#define REDIS_OK 0

/* When an error occurs, the err flag in a context is set to hold the type of
 * error that occured. REDIS_ERR_IO means there was an I/O error and you
 * should use the "errno" variable to find out what is wrong.
 * For other values, the "errstr" field will hold a description. */
#define REDIS_ERR_IO 1 /* Error in read or write */
#define REDIS_ERR_EOF 3 /* End of file */
#define REDIS_ERR_PROTOCOL 4 /* Protocol error */
#define REDIS_ERR_OOM 5 /* Out of memory */
#define REDIS_ERR_OTHER 2 /* Everything else... */

/* Connection type can be blocking or non-blocking and is set in the
 * least significant bit of the flags field in redisContext. */
#define REDIS_BLOCK 0x1

/* Connection may be disconnected before being free'd. The second bit
 * in the flags field is set when the context is connected. */
#define REDIS_CONNECTED 0x2

/* The async API might try to disconnect cleanly and flush the output
 * buffer and read all subsequent replies before disconnecting.
 * This flag means no new commands can come in and the connection
 * should be terminated once all replies have been read. */
#define REDIS_DISCONNECTING 0x4

/* Flag specific to the async API which means that the context should be clean
 * up as soon as possible. */
#define REDIS_FREEING 0x8

/* Flag that is set when an async callback is executed. */
#define REDIS_IN_CALLBACK 0x10

/* Flag that is set when the async context has one or more subscriptions. */
#define REDIS_SUBSCRIBED 0x20

/* Flag that is set when monitor mode is active */
#define REDIS_MONITORING 0x40

#define REDIS_REPLY_STRING 1
#define REDIS_REPLY_ARRAY 2
#define REDIS_REPLY_INTEGER 3
#define REDIS_REPLY_NIL 4
#define REDIS_REPLY_STATUS 5
#define REDIS_REPLY_ERROR 6

#define REDIS_READER_MAX_BUF (1024*16)  /* Default max unused reader buffer. */

#define REDIS_KEEPALIVE_INTERVAL 15 /* seconds */

#ifdef __cplusplus
extern "C" {
#endif

/* This is the reply object returned by redisCommand() */
typedef struct redisReply {
    int type; /* REDIS_REPLY_* */
    long long integer; /* The integer when type is REDIS_REPLY_INTEGER */
    sds str; /* Used for both REDIS_REPLY_ERROR and REDIS_REPLY_STRING */
    size_t elements; /* number of elements, for REDIS_REPLY_ARRAY */
    struct redisReply **element; /* elements vector for REDIS_REPLY_ARRAY */
} redisReply;

typedef struct redisReadTask {
    int type;
    int elements; /* number of elements in multibulk container */
    int idx; /* index in parent (array) object */
    void *obj; /* holds user-generated value for a read task */
    struct redisReadTask *parent; /* parent task */
    void *privdata; /* user-settable arbitrary field */
} redisReadTask;

typedef struct redisReplyObjectFunctions {
    void *(*createString)(const redisReadTask*, char*, size_t);
    void *(*createArray)(const redisReadTask*, int);
    void *(*createInteger)(const redisReadTask*, long long);
    void *(*createNil)(const redisReadTask*);
    void (*freeObject)(void*);
} redisReplyObjectFunctions;

/* State for the protocol parser */
typedef struct redisReader {
    int err; /* Error flags, 0 when there is no error */
    char errstr[128]; /* String representation of error when applicable */

    char *buf; /* Read buffer */
    size_t pos; /* Buffer cursor */
    size_t len; /* Buffer length */
    size_t maxbuf; /* Max length of unused buffer */
    size_t nextobj;

    redisReadTask rstack[9];
    int ridx; /* Index of current read task */
    void *reply; /* Temporary reply pointer */

    redisReplyObjectFunctions *fn;
    void *privdata;
} redisReader;

/* Public API for the protocol parser. */
int redisReaderInit(redisReader *r);
void redisReaderCleanup(redisReader *r);
int redisReaderFeed(redisReader *r, const char *buf, size_t len);
int redisReaderGetReply(redisReader *r, void **reply);
int redisReaderGetReplyBuffer(redisReader *r, const char **buf, size_t *len);

/* Function to free the reply objects hiredis returns by default. */
void freeReplyObject(void *reply);

void redisReaderShrinkBuffer(redisReader *r, size_t threshold);
int redisReaderCountObject(redisReader *r, const char **buf, size_t *len);
void dumpReplyObject(redisReply *reply, int indent, FILE *fp);

static __inline size_t
redisReaderPendingLength(redisReader *r) {
    return r->len - r->nextobj;
}

static __inline void
redisReaderReset(redisReader *r) {
    redisReplyObjectFunctions *fn = r->fn;
    size_t maxbuf = r->maxbuf;
    redisReaderCleanup(r);
    redisReaderInit(r);
    r->fn = fn;
    r->maxbuf = maxbuf;
}


static __inline sds redis_format_nil(sds out)
{
    return sdscatlen(out, "$-1\r\n", 5);
}

static __inline sds redis_format_prefix(sds out, char prefix, int num, int prealloc)
{
    char *p;
    out = sdsMakeRoomFor(out, SDS_LLSTR_SIZE + 3 + prealloc);
    if (!out)
        return NULL;
    p = out + sdslen(out);
    *p++ = prefix;
    p += sdsll2str(p, num);
    *p++ = '\r';
    *p++ = '\n';
    sdsIncrLen(out, p - out - sdslen(out));
    return out;
}

static __inline sds redis_format_integer(sds out, int n)
{
    return redis_format_prefix(out, ':', n, 0);
}

static __inline sds redis_format_array(sds out, size_t len)
{
    const int default_elemsize = 32;
    return redis_format_prefix(out, '*', (int)len, (int)len * default_elemsize);
}

static __inline sds redis_format_buf(sds out, const char *buf, size_t bufsize)
{
    char *p;
    out = redis_format_prefix(out, '$', (int)bufsize, (int)bufsize + 2);
    p = out + sdslen(out);
    memcpy(p, buf, bufsize);
    p[bufsize] = '\r';
    p[bufsize + 1] = '\n';
    sdsIncrLen(out, bufsize + 2);
    return out;
}

static __inline sds redis_format_str(sds out, const char *str)
{
    return redis_format_buf(out, str, strlen(str));
}

static __inline sds redis_format_sds(sds out, sds str)
{
    return redis_format_buf(out, str, sdslen(str));
}

static __inline sds redis_format_argv(sds out, int argc, char **argv)
{
    int i;
    out = redis_format_array(out, argc);
    for (i = 0; i < argc; i++) {
        out = redis_format_str(out, argv[i]);
    }
    return out;
}

static __inline redisReply *redis_parse(sds str)
{
    redisReply *obj;
    redisReader reader;
    int ret;

    redisReaderInit(&reader);

    ret = redisReaderFeed(&reader, str, sdslen(str));
    if (ret != REDIS_OK) {
        obj = NULL;
        goto out;
    }

    ret = redisReaderGetReply(&reader, (void **)&obj);
    if (ret != REDIS_OK) {
        obj = NULL;
        goto out;
    }
out:
    redisReaderCleanup(&reader);
    return obj;
}


#ifdef __cplusplus
}
#endif

#endif
