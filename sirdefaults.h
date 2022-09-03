/*
 * SPDX-License-Identifier: MIT
 * scspell-id: f93e5ca6-2b0d-11ed-a75e-80ee73e9b8e7
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

#ifndef _LOG_DEFAULTS_H_INCLUDED
# define _LOG_DEFAULTS_H_INCLUDED

# include "sirtypes.h"

/* Default levels for stdout. */

static const log_levels log_stdout_def_lvls
  = LOGL_DEBUG | LOGL_INFO | LOGL_NOTICE | LOGL_WARN;

/* Default options for stdout. */

static const log_options log_stdout_def_opts
  = LOGO_NOTIME | LOGO_NOPID | LOGO_NOTID;

/* Default levels for stderr. */

static const log_levels log_stderr_def_lvls
  = LOGL_ERROR | LOGL_CRIT | LOGL_EMERG;

/* Default options for stderr. */

static const log_options log_stderr_def_opts
  = LOGO_NOTIME | LOGO_NOPID | LOGO_NOTID;

/* Default levels for syslog (if enabled). */

static const log_levels log_syslog_def_lvls
  = LOGL_WARN | LOGL_CRIT | LOGL_ALERT | LOGL_EMERG;

/* Default levels for log files. */

static const log_levels log_file_def_lvls = LOGL_ALL;

/* Default options for log files. */

static const log_options log_file_def_opts = 0; /* (all output) */

/* Default mapping of log_level to log_textstyle. */

static const log_style_map log_default_styles[LOG_NUMLEVELS] = {
  { LOGL_DEBUG,  LOGS_FG_DGRAY                                 },
  { LOGL_INFO,   LOGS_FG_WHITE                                 },
  { LOGL_NOTICE, LOGS_FG_CYAN                                  },
  { LOGL_WARN,   LOGS_FG_YELLOW                                },
  { LOGL_ERROR,  LOGS_FG_RED                                   },
  { LOGL_CRIT,   LOGS_BRIGHT | LOGS_FG_RED                     },
  { LOGL_ALERT,  LOGS_BRIGHT | LOGS_FG_BLACK | LOGS_BG_LYELLOW },
  { LOGL_EMERG,  LOGS_BRIGHT | LOGS_FG_LYELLOW | LOGS_BG_RED   },
};

#endif /* !_LOG_DEFAULTS_H_INCLUDED */
