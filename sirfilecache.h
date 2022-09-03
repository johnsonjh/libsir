/*
 * scspell-id: 143b10af-2b0e-11ed-9701-80ee73e9b8e7
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

#ifndef _LOG_FILECACHE_H_INCLUDED
# define _LOG_FILECACHE_H_INCLUDED

# include "sirtypes.h"

typedef bool (*log_fcache_pred) (const void *match, logfile *iter);

typedef void (*log_fcache_update) (logfile *si, log_update_data *data);

logfileid_t _log_addfile(const logchar_t *path, log_levels levels,
                         log_options opts);

bool _log_updatefile(logfileid_t id, log_update_data *data);

bool _log_remfile(logfileid_t id);

logfile *_logfile_create(const logchar_t *path, log_levels levels,
                         log_options opts);

bool _logfile_open(logfile *sf);

void _logfile_close(logfile *sf);

bool _log_write(logfile *sf, const logchar_t *output);

bool _logfile_writeheader(logfile *sf, const logchar_t *msg);

bool _logfile_needsroll(logfile *sf);

bool _logfile_roll(logfile *sf, logchar_t **newpath);

bool _logfile_archive(logfile *sf, const logchar_t *newpath);

bool _logfile_splitpath(logfile *sf, logchar_t **name, logchar_t **ext);

void _logfile_destroy(logfile *sf);

bool _logfile_validate(logfile *sf);

void _logfile_update(logfile *sf, log_update_data *data);

logfileid_t _log_fcache_add(logfcache *sfc, const logchar_t *path,
                            log_levels levels, log_options opts);

bool _log_fcache_update(logfcache *sfc, logfileid_t id,
                        log_update_data *data);

bool _log_fcache_rem(logfcache *sfc, logfileid_t id);

bool _log_fcache_pred_path(const void *match, logfile *iter);

bool _log_fcache_pred_id(const void *match, logfile *iter);

logfile *_log_fcache_find(logfcache *sfc, const void *match,
                          log_fcache_pred pred);

bool _log_fcache_destroy(logfcache *sfc);

bool _log_fcache_dispatch(logfcache *sfc, log_level level, logoutput *output,
                          size_t *dispatched, size_t *wanted);

FILE *_log_fopen(const logchar_t *path);

void _log_fclose(FILE **f);

void _log_fflush(FILE *f);

bool _log_fflush_all(void);

#endif /* !_LOG_FILECACHE_H_INCLUDED */
