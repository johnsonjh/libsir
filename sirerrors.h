/*
 * SPDX-License-Identifier: MIT
 * scspell-id: 061962a7-2b0e-11ed-bba7-80ee73e9b8e7
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

//-V:LOG_E_NOERROR:616

#ifndef _LOG_ERRORS_H_INCLUDED
# define _LOG_ERRORS_H_INCLUDED

# include "sirhelpers.h"
# include "sirtypes.h"

/*
 * Functions, types, and codes used for error handling.
 */

/* Error codes. */

enum
{
  LOG_E_NOERROR   = 0,    /* The operation completed successfully    */
  LOG_E_NOTREADY  = 1,    /* libsir has not been initialized         */
  LOG_E_ALREADY   = 2,    /* libsir is already initialized           */
  LOG_E_DUPFILE   = 3,    /* File already managed by libsir          */
  LOG_E_NOFILE    = 4,    /* File not managed by libsir              */
  LOG_E_FCFULL    = 5,    /* Maximum number of files already managed */
  LOG_E_OPTIONS   = 6,    /* Option flags are invalid                */
  LOG_E_LEVELS    = 7,    /* Level flags are invalid                 */
  LOG_E_TEXTSTYLE = 8,    /* Text style is invalid                   */
  LOG_E_STRING    = 9,    /* Invalid string argument                 */
  LOG_E_NODEST    = 10,   /* No destinations registered for level    */
  LOG_E_PLATFORM  = 11,   /* Platform error %d %s                    */
  LOG_E_UNKNOWN   = 4095, /* Error is not known                      */
};

# define _LOG_E_NOERROR   _log_mkerror(LOG_E_NOERROR)
# define _LOG_E_NOTREADY  _log_mkerror(LOG_E_NOTREADY)
# define _LOG_E_ALREADY   _log_mkerror(LOG_E_ALREADY)
# define _LOG_E_DUPFILE   _log_mkerror(LOG_E_DUPFILE)
# define _LOG_E_NOFILE    _log_mkerror(LOG_E_NOFILE)
# define _LOG_E_FCFULL    _log_mkerror(LOG_E_FCFULL)
# define _LOG_E_OPTIONS   _log_mkerror(LOG_E_OPTIONS)
# define _LOG_E_LEVELS    _log_mkerror(LOG_E_LEVELS)
# define _LOG_E_TEXTSTYLE _log_mkerror(LOG_E_TEXTSTYLE)
# define _LOG_E_STRING    _log_mkerror(LOG_E_STRING)
# define _LOG_E_NODEST    _log_mkerror(LOG_E_NODEST)
# define _LOG_E_PLATFORM  _log_mkerror(LOG_E_PLATFORM)
# define _LOG_E_UNKNOWN   _log_mkerror(LOG_E_UNKNOWN)

static const struct
{
  logerror_t e;
  const logchar_t *msg;
} log_errors[] = {
  { _LOG_E_NOERROR,   "The operation completed successfully"    },
  { _LOG_E_NOTREADY,  "Logging has not been initialized"        },
  { _LOG_E_ALREADY,   "Logging is already initialized"          },
  { _LOG_E_DUPFILE,   "File already managed"                    },
  { _LOG_E_NOFILE,    "File not managed"                        },
  { _LOG_E_FCFULL,    "Maximum number of files already managed" },
  { _LOG_E_OPTIONS,   "Option flags are invalid"                },
  { _LOG_E_LEVELS,    "Level flags are invalid"                 },
  { _LOG_E_TEXTSTYLE, "Text style is invalid"                   },
  { _LOG_E_STRING,    "Invalid string argument"                 },
  { _LOG_E_NODEST,    "No destinations registered for level"    },
  { _LOG_E_PLATFORM,  "%d %s"                                   },
  { _LOG_E_UNKNOWN,   "Error is not known"                      },
};

void __log_seterror(logerror_t err, const logchar_t *func,
                    const logchar_t *file, uint32_t line);
# define _log_seterror(err) __log_seterror(err, __func__, __FILE__, __LINE__)

void __log_setoserror(int, const logchar_t *, const logchar_t *,
                      const logchar_t *, uint32_t);
# define _log_setoserror(err, msg) \
  __log_setoserror(err, msg, __func__, __FILE__, __LINE__)

/* Handle a C library error. */

void __log_handleerr(int code, const logchar_t *func, const logchar_t *file,
                     uint32_t line);
# define _log_handleerr(code) \
  __log_handleerr(code, __func__, __FILE__, __LINE__)

# ifdef _WIN32

/*
 * Some Win32 API error codes overlap C library error codes,
 * so they need to be handled separately. Mapping them sounds
 * great, but in practice valuable information about what went
 * wrong is totally lost in translation.
 */

void __log_handlewin32err(DWORD code, const logchar_t *func,
                          const logchar_t *file, uint32_t line);
#  define _log_handlewin32err(code) \
  __log_handlewin32err(code, __func__, __FILE__, __LINE__)
# endif /* ifdef _WIN32 */

/* Returns information about the last error that occurred. */

logerror_t _log_geterror(logchar_t message[LOG_MAXERROR - 1]);

# ifdef LOG_SELFLOG

/* Log an internal message to stderr. */

void _log_selflog(const logchar_t *format, ...);
# else /* ifdef LOG_SELFLOG */
#  define _log_selflog(format, ...) ((void)( 0 ))
# endif /* ifdef LOG_SELFLOG */

#endif /* !_LOG_ERRORS_H_INCLUDED */
