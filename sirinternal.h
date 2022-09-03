/*
 * SPDX-License-Identifier: MIT
 * scspell-id: 397945c3-2b0e-11ed-a96a-80ee73e9b8e7
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

#ifndef _LOG_INTERNAL_H_INCLUDED
# define _LOG_INTERNAL_H_INCLUDED

# include "sirerrors.h"
# include "sirhelpers.h"

/* Evaluates whether or not libsir has been initialized. */

bool _log_sanity(void);

/* Validates the configuration passed to log_init. */

bool _log_options_sanity(const loginit *si);

/* Initializes libsir. */

bool _log_init(loginit *si);

/* Updates levels for stdout. */

void _log_stdoutlevels(loginit *si, log_update_data *data);

/* Updates options for stdout. */

void _log_stdoutopts(loginit *si, log_update_data *data);

/* Updates levels for stderr. */

void _log_stderrlevels(loginit *si, log_update_data *data);

/* Updates options for stderr. */

void _log_stderropts(loginit *si, log_update_data *data);

/* Updates levels for syslog. */

void _log_sysloglevels(loginit *si, log_update_data *data);

/* Updates levels/options in the global init structure. */

typedef void (*loginit_update) (loginit *, log_update_data *);

/* Updates levels/options in the global init structure. */

bool _log_writeinit(log_update_data *data, loginit_update update);

/* Locks a protected section. */

void *_log_locksection(log_mutex_id mid);

/* Unlocks a protected section. */

bool _log_unlocksection(log_mutex_id mid);

/* Maps a log_mutex_id to a logmutex_t and protected section. */

bool _log_mapmutexid(log_mutex_id mid, logmutex_t **m, void **section);

/* Frees allocated resources. */

bool _log_cleanup(void);

# ifndef _WIN32

/* Initializes a specific mutex. */

void _log_initmutex_si_once(void);

/* Initializes a specific mutex. */

void _log_initmutex_fc_once(void);

/* Initializes a specific mutex. */

void _log_initmutex_ts_once(void);

# else  /* ifndef _WIN32 */

/* Initializes a specific mutex. */

BOOL CALLBACK _log_initmutex_si_once(PINIT_ONCE ponce, PVOID param,
                                     PVOID *ctx);

/* Initializes a specific mutex. */

BOOL CALLBACK _log_initmutex_fc_once(PINIT_ONCE ponce, PVOID param,
                                     PVOID *ctx);

/* Initializes a specific mutex. */

BOOL CALLBACK _log_initmutex_ts_once(PINIT_ONCE ponce, PVOID param,
                                     PVOID *ctx);
# endif /* ifndef _WIN32 */

/* Initializes a specific mutex. */

void _log_initmutex(logmutex_t *mutex);

/* Executes only one time. */

void _log_once(logonce_t *once, log_once_fn func);

/* Core output formatting. */

bool _log_logv(log_level level, const logchar_t *format, va_list args);

/* Output dispatching. */

bool _log_dispatch(loginit *si, log_level level, logoutput *output);

/* Specific destination formatting. */

const logchar_t *_log_format(bool styling, log_options opts,
                             logoutput *output);

# ifndef LOG_NO_SYSLOG

/* Maps a log_level to a syslog level. */

int _log_syslog_maplevel(log_level level);

# endif /* ifndef LOG_NO_SYSLOG */

/* Retrieves a buffer from a logbuf. */

logchar_t *_logbuf_get(logbuf *buf, size_t idx);

/* Converts a log_level to its human-readable form. */

const logchar_t *_log_levelstr(log_level level);

/* Retrieves the current local time w/ optional milliseconds. */

bool _log_getlocaltime(time_t *tbuf, long long *nsecbuf);

/* Formats the current time as a string. */

bool _log_formattime(time_t now, logchar_t *buffer, const logchar_t *format);

/* Returns the current process identifier. */

pid_t _log_getpid(void);

/* Returns the current thread identifier. */

pid_t _log_gettid(void);

/* Returns the current thread's name. */

bool _log_getthreadname(char name[LOG_MAXPID]);

#endif /* !_LOG_INTERNAL_H_INCLUDED */
