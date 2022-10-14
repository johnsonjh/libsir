/*
 * SPDX-License-Identifier: MIT
 * scspell-id: 006ad2b6-2b0e-11ed-98a7-80ee73e9b8e7
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

#include "sirerrors.h"

/* Per-thread error data */

static thread_local log_thread_err log_te
  = {
  _LOG_E_NOERROR, 0, { 0 }, { LOG_UNKNOWN, LOG_UNKNOWN, 0 }
  };

void
__log_seterror(logerror_t err, const logchar_t *func, const logchar_t *file,
               uint32_t line)
{
  if (_log_validerror(err))
    {
      log_te.lasterror = err;
      log_te.loc.func  = func;
      log_te.loc.file  = file;
      log_te.loc.line  = line;
    }

  assert(_LOG_E_NOERROR == err);
}

void
__log_setoserror(int code, const logchar_t *message, const logchar_t *func,
                 const logchar_t *file, uint32_t line)
{
  log_te.os_error = code;
  _log_resetstr(log_te.os_errmsg);

  if (_log_validstrnofail(message))
    {
      (void)strncpy(log_te.os_errmsg, message, LOG_MAXERROR - 2);
    }

  __log_seterror(_LOG_E_PLATFORM, func, file, line);
}

void
__log_handleerr(int code, const logchar_t *func, const logchar_t *file,
                uint32_t line)
{
  if (LOG_E_NOERROR != code)
    {
      logchar_t message[LOG_MAXERROR - 1] = {
        0
      };

#ifndef _WIN32
      errno = LOG_E_NOERROR;
# if _POSIX_C_SOURCE >= 200112L && !defined( _GNU_SOURCE )
      int finderr = strerror_r(code, message, LOG_MAXERROR - 1);
# else /* if _POSIX_C_SOURCE >= 200112L && !defined( _GNU_SOURCE ) */
      int finderr = 0;
      char *tmp   = strerror_r(code, message, LOG_MAXERROR - 1);
      if (tmp != message)
        {
          (void)strncpy(message, tmp, strnlen(tmp, LOG_MAXERROR - 1));
        }

# endif /* if _POSIX_C_SOURCE >= 200112L && !defined( _GNU_SOURCE ) */
#else  /* ifndef _WIN32 */
      errno_t finderr = strerror_s(message, LOG_MAXERROR - 1, code);
#endif /* ifndef _WIN32 */
      if (0 == finderr && _log_validstrnofail(message)) //-V560
        {
          __log_setoserror(code, message, func, file, line);
        }
      else
        {
#ifndef _WIN32
          _log_selflog("%s: strerror_r failed! error: %d\n", __func__, errno);
#else  /* ifndef _WIN32 */
          _log_selflog("%s: strerror_s failed! error: %d\n", __func__, finderr);
#endif /* ifndef _WIN32 */
        }
    }

  assert(LOG_E_NOERROR == code);
}

#ifdef _WIN32
void
__log_handlewin32err(DWORD code, const logchar_t *func, const logchar_t *file,
                     uint32_t line)
{
  if (ERROR_SUCCESS != code)
    {
      logchar_t *errbuf = NULL;
      DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER
                  | FORMAT_MESSAGE_FROM_SYSTEM
                  | FORMAT_MESSAGE_IGNORE_INSERTS
                  | FORMAT_MESSAGE_MAX_WIDTH_MASK;
      DWORD fmtmsg = FormatMessageA(
        flags,
        NULL,
        code,
        0,
        (LPSTR)&errbuf,
        LOG_MAXERROR - 1,
        NULL);

      if (0 == fmtmsg && _log_validstrnofail(errbuf))
        {
          __log_setoserror((int)code, errbuf, func, file, line);
        }
      else
        {
          _log_selflog(
            "%s: FormatMessage failed! error: %d\n",
            __func__,
            GetLastError());
          assert(false);
        }

      if (errbuf)
        {
          BOOL heapfree = HeapFree(GetProcessHeap(), 0, errbuf);
          assert(0 != heapfree);
          errbuf = NULL;
        }
    }

  assert(ERROR_SUCCESS == code);
}
#endif /* ifdef _WIN32 */

logerror_t
_log_geterror(logchar_t message[LOG_MAXERROR - 1])
{
  _log_resetstr(message);
  for (size_t n = 0; n < _log_countof(log_errors); n++)
    {
      if (log_errors[n].e == log_te.lasterror)
        {
          logchar_t *final = NULL;
          bool alloc       = false;

          if (_LOG_E_PLATFORM == log_errors[n].e)
            {
              final = (logchar_t *)calloc(LOG_MAXERROR + 1, sizeof ( logchar_t ));

              if (_log_validptr(final))
                {
                  alloc = true;
                  (void)snprintf(
                    final,
                    LOG_MAXERROR + 1,
                    log_errors[n].msg,
                    log_te.os_error,
                    _log_validstrnofail(log_te.os_errmsg) ? log_te.os_errmsg : LOG_UNKNOWN);
                }
            }
          else
            {
              final = (logchar_t *)log_errors[n].msg;
            }

          if (final != NULL)
            {
              int fmtmsg
                = snprintf(
                    message,
                    LOG_MAXERROR - 1,
                    LOG_ERRORFORMAT,
                    log_te.loc.func,
                    log_te.loc.file,
                    log_te.loc.line,
                    final);
              (void)fmtmsg;
              assert(fmtmsg >= 0);

              if (alloc)
                {
                  _log_safefree(final);
                }
            }

          return log_errors[n].e;
        }
    }

  /* assert(false && log_te.lasterror); */
  return _LOG_E_UNKNOWN;
}

#ifdef LOG_SELFLOG
void
_log_selflog(const logchar_t *format, ...)
{
  logchar_t output[LOG_MAXMESSAGE] = {
    0
  };
  va_list args;

  va_start(args, format);
  int print = vsnprintf(output, LOG_MAXMESSAGE, format, args);
  (void)print;
  va_end(args);

  assert(print > 0);

  if (print > 0)
    {
      int put = fputs(output, stderr);
      (void)put;
      assert(put != EOF);
    }
}
#endif /* ifdef LOG_SELFLOG */
