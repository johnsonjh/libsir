/*
 * scspell-id: 2b3241cb-2b0e-11ed-84fb-80ee73e9b8e7
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

#ifndef _LOG_HELPERS_H_INCLUDED
# define _LOG_HELPERS_H_INCLUDED

# include "sirtypes.h"

/* Computes the size of an array. */

# define _log_countof(arr) ( sizeof ( arr ) / sizeof ( arr[0] ))

/*
 * Creates an error code that (hopefully) doesn't conflict
 * with any of those defined by the platform.
 */

# define _log_mkerror(code) (((uint32_t)( code & 0x7fff ) << 16 ) | 0x80000000 )

/* Validates an internal error. */

static inline bool
_log_validerror(sirerror_t err)
{
  sirerror_t masked = err & 0x8fffffff;

  return masked >= 0x80000000 && masked <= 0x8fff0000;
}

/* Extracts just the code from an internal error. */

static inline uint16_t
_log_geterrcode(sirerror_t err)
{
  return ( err >> 16 ) & 0x7fff;
}

/* Evil macro used for _log_lv wrappers. */

# define _LOG_L_START(format)  \
  bool r = false;              \
  va_list args;                \
  va_start(args, format);

/* Evil macro used for _log_lv wrappers. */

# define _LOG_L_END(args) va_end(args);

/* Validates a pointer. */

# define _log_validptr(p) ( NULL != p )

/* Checks a bitfield for a specific set of bits. */

static inline bool
_log_bittest(uint32_t flags, uint32_t test)
{
  return ( flags & test ) == test;
}

/* Wraps free. */

static inline void
__log_safefree(void **p)
{
  if (!p || ( p && !*p ))
    {
      return;
    }

  free(*p);
  *p = NULL;
}

/* Wraps free. */

static inline void
_log_safefree(void *p)
{
  __log_safefree(&p);
}

/* Validates a log file identifier. */

bool _log_validfid(int id);

/* Validates a set of log_level flags. */

bool _log_validlevels(log_levels levels);

/* Validates a single log_level. */

bool _log_validlevel(log_level level);

/* Applies default log_level flags if applicable. */

static inline void
_log_defaultlevels(log_levels *levels, log_levels def)
{
  if (levels && SIRL_DEFAULT == *levels)
    {
      *levels = def;
    }
}

/* Applies default log_options flags if applicable. */

static inline void
_log_defaultopts(log_options *opts, log_options def)
{
  if (opts && SIRO_DEFAULT == *opts)
    {
      *opts = def;
    }
}

/* Validates a set of log_option flags. */

bool _log_validopts(log_options opts);

/* Validates a string pointer and optionally fails if it's invalid. */

bool __log_validstr(const sirchar_t *str, bool fail);

/* Validates a string pointer and fails if it's invalid. */

static inline bool
_log_validstr(const sirchar_t *str)
{
  return __log_validstr(str, true);
}

/* Validates a string pointer but ignores if it's invalid. */

static inline bool
_log_validstrnofail(const sirchar_t *str)
{
  return __log_validstr(str, false);
}

static inline bool

_log_validupdatedata(log_update_data *data)
{
  return NULL != data
           && ((  NULL == data->levels || _log_validlevels(*data->levels))
             && ( NULL == data->opts   || _log_validopts  (*data->opts)));
}

/* Places a null terminator at the first index in a string buffer. */

static inline void
_log_resetstr(sirchar_t *str)
{
  str[0] = (sirchar_t)'\0';
}

#endif /* !_LOG_HELPERS_H_INCLUDED */
