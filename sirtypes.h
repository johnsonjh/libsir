/*
 * SPDX-License-Identifier: MIT
 * scspell-id: d04c0528-2b0e-11ed-a16e-80ee73e9b8e7
 *
 * Copyright (c) 2018 Ryan M. Lederman
 * Copyright (c) 2022 Jeffrey H. Johnson <trnsz@pobox.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef _LOG_TYPES_H_INCLUDED
# define _LOG_TYPES_H_INCLUDED

# include "sirconfig.h"
# include "sirplatform.h"

/* The file identifier type. */

typedef const int *logfileid_t;

/* The error code type. */

typedef uint32_t logerror_t;

/* Defines the available levels (severity/priority) of logging output. */

typedef enum
{
  LOGL_NONE    = 0x0,  /* No output.                                           */
  LOGL_EMERG   = 0x1,  /* Nuclear war, Armageddon, etc.                        */
  LOGL_ALERT   = 0x2,  /* Action required ASAP.                                */
  LOGL_CRIT    = 0x4,  /* Critical errors.                                     */
  LOGL_ERROR   = 0x8,  /* Errors.                                              */
  LOGL_WARN    = 0x10, /* Warnings that could likely be ignored.               */
  LOGL_NOTICE  = 0x20, /* Normal but significant.                              */
  LOGL_INFO    = 0x40, /* Informational messages.                              */
  LOGL_DEBUG   = 0x80, /* Debugging/diagnostic output.                         */
  LOGL_ALL     = 0xff, /* Includes all logging levels.                         */
  LOGL_DEFAULT = 0x100 /* Use the default levels for this type of destination. */
} log_level;

/*
 * Used to differentiate between a single log_level and one or more
 * bitwise OR'd together.
 */

typedef uint16_t log_levels;

/* Formatting options for a destination. */

typedef enum
{
  /* Don't include time stamps in output. */

  LOGO_NOTIME   = 0x200,

  /* Don't include the human-readable logging level in output. */

  LOGO_NOLEVEL  = 0x400,

  /* Don't include the process/app name in output. */

  LOGO_NONAME   = 0x800,

  /*
   * Don't include milliseconds in time stamps. If not set, time stamps
   * in output to this destination will be appended with the current
   * millisecond in addition to the hour, minute, and second. If LOGO_NOTIME
   * is set, this has no effect.
   */

  LOGO_NOMSEC   = 0x1000,

  /* Don't include the process ID in output. */

  LOGO_NOPID    = 0x2000,

  /* Don't include the thread ID/name in output. */

  LOGO_NOTID    = 0x4000,

  /*
   * Don't write header messages when logging begins, or the file is rolled.
   * Only applicable to log files.
   */

  LOGO_NOHDR    = 0x10000,

  /*
   * Includes all other options; effectively disables all output formatting
   * except the original formatted message (does not include LOGO_NOHDR; set
   * that flag in addition to remove header messages).
   */

  LOGO_MSGONLY  = 0xeff00,

  /*
   * Use the default for this type of destination. See
   * logdefaults.h for specifics.
   */

  LOGO_DEFAULT  = 0x100000,
} log_option;

/*
 * Used to differentiate between a single log_option and one or more
 * bitwise OR'd together.
 */

typedef uint32_t log_options;

/* Available styles (colors, brightness) for console output. */

typedef enum
{
  LOGS_NONE         = 0,       /* Used internally; has no effect.                */
  LOGS_BRIGHT       = 0x1,     /* If set, the foreground color is 'intensified'. */
  LOGS_DIM          = 0x2,     /* If set, the foreground color is 'dimmed'.      */
  LOGS_FG_BLACK     = 0x10,    /* Black foreground.                              */
  LOGS_FG_RED       = 0x20,    /* Red foreground.                                */
  LOGS_FG_GREEN     = 0x30,    /* Green foreground.                              */
  LOGS_FG_YELLOW    = 0x40,    /* Yellow foreground.                             */
  LOGS_FG_BLUE      = 0x50,    /* Blue foreground.                               */
  LOGS_FG_MAGENTA   = 0x60,    /* Magenta foreground.                            */
  LOGS_FG_CYAN      = 0x70,    /* Cyan foreground.                               */
  LOGS_FG_WHITE     = 0x80,    /* White foreground.                              */
  LOGS_FG_LGRAY     = 0x90,    /* Light gray foreground.                         */
  LOGS_FG_DGRAY     = 0xa0,    /* Dark gray foreground.                          */
  LOGS_FG_LRED      = 0xb0,    /* Light red foreground.                          */
  LOGS_FG_LGREEN    = 0xc0,    /* Light green foreground.                        */
  LOGS_FG_LYELLOW   = 0xd0,    /* Light yellow foreground.                       */
  LOGS_FG_LBLUE     = 0xe0,    /* Light blue foreground.                         */
  LOGS_FG_LMAGENTA  = 0xf0,    /* Light magenta foreground.                      */
  LOGS_FG_LCYAN     = 0xf10,   /* Light cyan foreground.                         */
  LOGS_FG_DEFAULT   = 0xf20,   /* Use the default foreground color.              */
  LOGS_BG_BLACK     = 0x1000,  /* Black background.                              */
  LOGS_BG_RED       = 0x2000,  /* Red background.                                */
  LOGS_BG_GREEN     = 0x3000,  /* Green background.                              */
  LOGS_BG_YELLOW    = 0x4000,  /* Yellow background.                             */
  LOGS_BG_BLUE      = 0x5000,  /* Blue background.                               */
  LOGS_BG_MAGENTA   = 0x6000,  /* Magenta background.                            */
  LOGS_BG_CYAN      = 0x7000,  /* Cyan background.                               */
  LOGS_BG_WHITE     = 0x8000,  /* White background.                              */
  LOGS_BG_LGRAY     = 0x9000,  /* Light gray background.                         */
  LOGS_BG_DGRAY     = 0xa000,  /* Dark gray background.                          */
  LOGS_BG_LRED      = 0xb000,  /* Light red background.                          */
  LOGS_BG_LGREEN    = 0xc000,  /* Light green background.                        */
  LOGS_BG_LYELLOW   = 0xd000,  /* Light yellow background.                       */
  LOGS_BG_LBLUE     = 0xe000,  /* Light blue background.                         */
  LOGS_BG_LMAGENTA  = 0xf000,  /* Light magenta background.                      */
  LOGS_BG_LCYAN     = 0xf1000, /* Light cyan background.                         */
  LOGS_BG_DEFAULT   = 0xf2000, /* Use the default background color.              */
  LOGS_INVALID      = 0xf3000  /* Represents the invalid text style.             */
} log_textstyle;

/* The underlying type used for characters in output. */

typedef char logchar_t;

/*
 * log_stdio_dest
 * Configuration for stdio destinations (stdout and stderr).
 */

typedef struct
{
  log_levels levels;
  log_options opts;
} log_stdio_dest;

/*
 * log_syslog_dest
 * Configuration for the syslog destination.
 */

typedef struct
{
  log_levels levels;
  bool includePID;
} log_syslog_dest;

/*
 * loginit
 * Initialization data for libsir.
 *
 * Allocate an instance of this struct and pass it to log_init
 * in order to begin using libsir.
 */

typedef struct
{
  log_stdio_dest d_stdout;  /* stdout configuration.                */
  log_stdio_dest d_stderr;  /* stderr configuration.                */
  log_syslog_dest d_syslog; /* syslog configuration (if available). */

  /*
   * If set, defines the name that will appear in formatted output.
   * Set LOGO_NONAME for a destination to supppress it.
   */

  logchar_t processName[LOG_MAXNAME];
} loginit;

/* Library error type. */

typedef struct
{
  logerror_t code;
  const logchar_t *const message;
} logerror;

/* Text style attribute mask. */

# define _LOGS_ATTR_MASK 0xf

/* Text style foreground color mask. */

# define _LOGS_FG_MASK 0xff0

/* Text style background color mask. */

# define _LOGS_BG_MASK 0xff000

/* Magic number used to determine if libsir has been initialized. */

# define _LOG_MAGIC 0x60906090

/* Log file data. */

typedef struct
{
  logchar_t *path;
  log_levels levels;
  log_options opts;
  FILE *f;
  int id;
} logfile;

/* Log file cache. */

typedef struct
{
  logfile *files[LOG_MAXFILES];
  size_t count;
} logfcache;

/* Formatted output sent to destinations. */

typedef struct
{
  logchar_t *style;
  logchar_t *timestamp;
  logchar_t *msec;
  logchar_t *level;
  logchar_t *name;
  logchar_t *pid;
  logchar_t *tid;
  logchar_t *message;
  logchar_t *output;
} logoutput;

/* Indexes into logbuf buffers. */

typedef enum
{
  _SIRBUF_STYLE = 0,
  _SIRBUF_TIME,
  _SIRBUF_MSEC,
  _SIRBUF_LEVEL,
  _SIRBUF_NAME,
  _SIRBUF_PID,
  _SIRBUF_TID,
  _SIRBUF_MSG,
  _SIRBUF_OUTPUT,
  _SIRBUF_MAX
} logbuf_idx;

/* Buffers for output formatting. */

typedef struct
{
  logchar_t style     [LOG_MAXSTYLE];
  logchar_t timestamp [LOG_MAXTIME];
  logchar_t msec      [LOG_MAXMSEC];
  logchar_t level     [LOG_MAXLEVEL];
  logchar_t name      [LOG_MAXNAME];
  logchar_t pid       [LOG_MAXPID];
  logchar_t tid       [LOG_MAXPID];
  logchar_t message   [LOG_MAXMESSAGE];
  logchar_t output    [LOG_MAXOUTPUT];
} logbuf;

/* log_level <> log_textstyle mapping. */

typedef struct
{
  const log_level level; /* The level for which the style applies. */
  uint32_t style;        /* The default value.                     */
} log_style_map;

/* Public (log_textstyle) <> platform text style mapping. */

typedef struct
{
  uint32_t from; /* The public text style flag(s). */
  uint16_t to;   /* The internal value.            */
} log_style_priv_map;

/* Mutex <> protected section mapping. */

typedef enum
{
  _LOGM_INIT = 0,  /* The loginit section.       */
  _LOGM_FILECACHE, /* The logfcache section.     */
  _LOGM_TEXTSTYLE, /* The log_style_map section. */
} log_mutex_id;

/* Error type. */

typedef struct
{
  logerror_t lasterror;
  int os_error;
  logchar_t os_errmsg[LOG_MAXERROR - 1];

  struct
  {
    const logchar_t *func;
    const logchar_t *file;
    uint32_t line;
  } loc;
} log_thread_err;

/*
 * Used to encapsulate dynamic updating of
 * config; add members here if necessary.
 */

typedef struct
{
  log_levels *levels;
  log_options *opts;
} log_update_data;

#endif /* !_LOG_TYPES_H_INCLUDED */
