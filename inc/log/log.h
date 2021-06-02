/**
 * Copyright (c) 2020 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

#define LOG_VERSION "0.1.0"

typedef struct LogEvent {
  va_list ap;
  const char *fmt;
  const char *file;
  struct tm *time;
  void *udata;
  int line;
  int level;
} LogEvent;

typedef void (*log_LogFn)(LogEvent *ev);
typedef void (*log_LockFn)(bool lock, void *udata);

typedef enum LogLevel{ LOG_LEVEL_TRACE = 0, LOG_LEVEL_DEBUG, LOG_LEVEL_INFO, LOG_LEVEL_WARN, LOG_LEVEL_ERROR, LOG_LEVEL_FATAL } LogLevel;

#define LOG_TRACE(...) log_log(LOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...) log_log(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)  log_log(LOG_LEVEL_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)  log_log(LOG_LEVEL_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) log_log(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) log_log(LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

const char* log_level_string(int level);
void log_set_lock(log_LockFn fn, void *udata);
void log_set_level(int level);
void log_set_quiet(bool enable);
void log_set_verbose(bool enable);
void log_set_non_intrusive_level(LogLevel level);
int log_add_callback(log_LogFn fn, void *udata, int level);
int log_add_fp(FILE *fp, int level);

void log_log(int level, const char *file, int line, const char *fmt, ...);

#endif
