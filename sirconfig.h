/*
 * SPDX-License-Identifier: MIT
 * scspell-id: e1aeb7d3-2b0d-11ed-8599-80ee73e9b8e7
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

#ifndef _LOG_CONFIG_H_INCLUDED
# define _LOG_CONFIG_H_INCLUDED

/* The time stamp format string at the beginning of log messages. */

# define LOG_TIMEFORMAT "%H:%M:%S"

/* The format for the current millisecond in time stamps. */

# define LOG_MSECFORMAT ".%03lld"

/* The format for the human-readable logging level. */

# define LOG_LEVELFORMAT "[%s]"

/*
 * The format for the current process/thread ID,
 */

# define LOG_PIDFORMAT "%d"

/* The character to place between process and thread IDs. */

# define LOG_PIDSEPARATOR "."

/* The string passed to fopen/fopen_s for log files. */

# define LOG_FOPENMODE "a"

/* The size, in bytes, at which a log file will be rolled/archived. */

# define LOG_FROLLSIZE ( 1024L * 1024L * 10L )

/*
 * The time format string in file headers (see LOG_FHFORMAT).
 */

# define LOG_FHTIMEFORMAT "%H:%M:%S %a %d %b %y (%z)"

/*
 * The format string written to a log file when logging begins or the file
 * is rolled/archived.
 * - The first %s format specifier is the message (e.g., LOG_FHBEGIN, LOG_FHROLLED)
 * - The second %s is the current date/time (see LOG_FHTIMEFORMAT).
 */

# define LOG_FHFORMAT "\n\n----- %s %s -----\n\n"

/*
 * The string included in LOG_FHFORMAT when a logging session begins.
 */

# define LOG_FHBEGIN "session begin @"

/*
 * The string included in LOG_FHFORMAT when a file is rolled/archived
 *   due to size.
 * The %s format specifier is the path of the archived file.
 */

# define LOG_FHROLLED "archived as %s due to size @"

/*
 * The time format string for rolled/archived log files (see LOG_FNAMEFORMAT).
 */

# define LOG_FNAMETIMEFORMAT "%Y-%m-%d-%H%M%S"

/*
 * The format string for rolled/archived log file names.
 * - The first %s format specifier is the name part (the name up to the
 *     last '.') of the original file name.
 * - The second %s is the time stamp as defined by LOG_ROLLTIMEFORMAT.
 * - The third %s is the extension part (the name after and including the
 *     last '.') of the original file name if one is present.
 */

# define LOG_FNAMEFORMAT "%s-%s%s"

/* The human-readable form of the LOGL_EMERG level. */

# define LOGL_S_EMERG    "EMRG"

/* The human-readable form of the LOGL_ALERT level. */

# define LOGL_S_ALERT    "ALRT"

/* The human-readable form of the LOGL_CRIT level. */

# define LOGL_S_CRIT     "CRIT"

/* The human-readable form of the LOGL_ERROR level. */

# define LOGL_S_ERROR    "ERRO"

/* The human-readable form of the LOGL_WARN level. */

# define LOGL_S_WARN     "WARN"

/* The human-readable form of the LOGL_NOTICE level. */

# define LOGL_S_NOTICE   "NOTI"

/* The human-readable form of the LOGL_INFO level. */

# define LOGL_S_INFO     "INFO"

/* The human-readable form of the LOGL_DEBUG level. */

# define LOGL_S_DEBUG    "DEBG"

/* The maximum number of log files that may be registered. */

# define LOG_MAXFILES 16

/*
 * The maximum number of characters that may be included in one message,
 * not including other parts of the output, like the timestamp and level.
 */

# define LOG_MAXMESSAGE 2048

/* The size, in characters, of the buffer used to hold time format strings. */

# define LOG_MAXTIME 64

/* The size, in characters, of the buffer used to hold millisecond strings. */

# define LOG_MAXMSEC 5

/* The size, in characters, of the buffer used to hold level format strings. */

# define LOG_MAXLEVEL 7

/*
 * The size, in characters, of the buffer used to hold process/appname
 * format strings.
 */

# define LOG_MAXNAME 32

/*
 * The size, in characters, of the buffer used to hold process/thread
 * IDs/names.
 */

# define LOG_MAXPID 16

/* The maximum number of whitespace and misc. characters included in output. */

# define LOG_MAXMISC 7

/* The maximum size, in characters, of final formatted output. */

# define LOG_MAXOUTPUT                                                \
  LOG_MAXMESSAGE + ( LOG_MAXSTYLE * 2 ) + LOG_MAXTIME + LOG_MAXLEVEL  \
  + LOG_MAXNAME  + ( LOG_MAXPID   * 2 ) + LOG_MAXMISC + 1

/* The maximum size, in characters, of an error message. */

# define LOG_MAXERROR 256

/*
 * The format string for error messages returned by _log_geterror.
 * - The first %s format specifier is the function name.
 * - The second %s is the file name.
 * - The %u is the line number in the file.
 * - The third %s is the error message.
 */

# define LOG_ERRORFORMAT "Error in %s (%s:%u): '%s'"

/* The string that represents any unknown. */

# define LOG_UNKNOWN "<unknown>"

/* The value that represents an invalid file identifier. */

# define LOG_INVALID (int)-1

/*
 * Used for level <> text style mapping. Update if adding or removing
 * levels or bad things will happen.
 */

# define LOG_NUMLEVELS 9

# ifndef _WIN32

/* The size, in characters, of the buffer used to hold text styling data. */

#  define LOG_MAXSTYLE 16

/* The string used to reset any styling applied to text in stdio output. */

#  define LOG_ENDSTYLE "\033[0m"

/*
 * The default clock used to obtain the
 * current millisecond from clock_gettime.
 */

#  define LOG_MSECCLOCK CLOCK_MONOTONIC

# else /* ifndef _WIN32 */

#  define LOG_MAXSTYLE sizeof ( uint16_t )
#  undef LOG_ENDSTYLE
#  undef LOG_MSECCLOCK

# endif /* ifndef _WIN32 */

#endif /* !_LOG_CONFIG_H_INCLUDED */
