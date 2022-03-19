/**
 * Copyright (c) 2016 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#ifndef SANDBIRD_H
#define SANDBIRD_H

#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SB_VERSION "0.1.4"

typedef struct sb_Server  sb_Server;
typedef struct sb_Stream  sb_Stream;
typedef struct sb_Event   sb_Event;
typedef struct sb_Options sb_Options;
typedef struct sb_Buffer sb_Buffer;
typedef int (*sb_Handler)(sb_Event*);
typedef int sb_Socket;

struct sb_Buffer { char *s; size_t len, cap; };

struct sb_Server {
  sb_Stream *streams;         /* Linked list of all streams */
  sb_Handler handler;         /* Event handler callback function */
  sb_Socket sockfd;           /* Listeneing server socket */
  void *udata;                /* User data value passed to all events */
  time_t now;                 /* The current time */
  time_t timeout;             /* Stream no-activity timeout */
  time_t max_lifetime;        /* Maximum time a stream can exist */
  size_t max_request_size;    /* Maximum request size in bytes */
  pthread_mutex_t stream_mtx; /* Mutex to lock stream access */
};

struct sb_Stream {
  int state;                  /* Current state of the stream */
  sb_Server *server;          /* The server object which owns this stream */
  char address[46];           /* Remote IP address */
  time_t init_time;           /* Time the stream was created */
  time_t last_activity;       /* Time of Last I/O activity on the stream */
  size_t expected_recv_len;   /* Expected length of the stream's request */
  size_t data_idx;            /* Index of data section in recv_buf */
  sb_Socket sockfd;           /* Socket for this streams connection */
  sb_Buffer recv_buf;         /* Data received from client */
  sb_Buffer send_buf;         /* Data waiting to be sent to client */
  int send_fd;                /* File descriptor currently being sent to client */
  pthread_t thr;              /* Processing thread */
  sb_Stream *next;            /* Next stream in linked list */
  sb_Stream *prev;            /* Previous stream in linked list */
  FILE *send_fp;              /* File currently being sent to client */
};

struct sb_Event {
  int type;
  void *udata;
  sb_Server *server;
  sb_Stream *stream;
  const char *address;
  const char *method;
  const char *path;
};

struct sb_Options {
  sb_Handler handler;
  void *udata;
  const char *host;
  const char *port;
  const char *timeout;
  const char *max_lifetime;
  const char *max_request_size;
};

enum {
  SB_ESUCCESS     =  0,
  SB_EFAILURE     = -1,
  SB_EOUTOFMEM    = -2,
  SB_ETRUNCATED   = -3,
  SB_EBADSTATE    = -4,
  SB_EBADRESULT   = -5,
  SB_ECANTOPEN    = -6,
  SB_ENOTFOUND    = -7,
  SB_EFDTOOBIG    = -8
};

enum {
  SB_EV_CONNECT,
  SB_EV_CLOSE,
  SB_EV_REQUEST
};

enum {
  SB_RES_OK,
  SB_RES_CLOSE
};

const char *sb_error_str(int code);
sb_Server *sb_new_server(const sb_Options *opt);
void sb_close_server(sb_Server *srv);
int sb_poll_server(sb_Server *srv);
int sb_send_status(sb_Stream *st, int code, const char *msg);
int sb_send_header(sb_Stream *st, const char *field, const char *val);
int sb_send_file(sb_Stream *st, const char *filename);
int sb_write(sb_Stream *st, const void *data, size_t len);
int sb_vwritef(sb_Stream *st, const char *fmt, va_list args);
int sb_writef(sb_Stream *st, const char *fmt, ...);
int sb_get_header(sb_Stream *st, const char *field, char *dst, size_t len);
int sb_get_var(sb_Stream *st, const char *name, char *dst, size_t len);
int sb_get_cookie(sb_Stream *st, const char *name, char *dst, size_t len);
const void *sb_get_multipart(sb_Stream *st, const char *name, size_t *len);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
