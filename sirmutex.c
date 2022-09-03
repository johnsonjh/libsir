/*
 * scspell-id: 3f7c94ed-2b0e-11ed-8bb3-80ee73e9b8e7
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

#include "sirmutex.h"
#include "sirinternal.h"
#include "sirplatform.h"

#ifndef _WIN32 /* pthread mutex implementation */

bool
_logmutex_create(logmutex_t *mutex)
{
  if (_log_validptr(mutex))
    {
      pthread_mutexattr_t attr;

      int op = pthread_mutexattr_init(&attr);
      _log_handleerr(op);

      if (0 == op)
        {
          op = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
          _log_handleerr(op);

          if (0 == op)
            {
              op = pthread_mutex_init(mutex, &attr);
              _log_handleerr(op);

              return 0 == op;
            }
        }
    }

  return false;
}

bool
_logmutex_trylock(logmutex_t *mutex)
{
  if (_log_validptr(mutex))
    {
      int op = pthread_mutex_trylock(mutex);
      _log_handleerr(op);
      return 0 == op;
    }

  return false;
}

bool
_logmutex_lock(logmutex_t *mutex)
{
  if (_log_validptr(mutex))
    {
      int op = pthread_mutex_lock(mutex);
      _log_handleerr(op);
      return 0 == op;
    }

  return false;
}

bool
_logmutex_unlock(logmutex_t *mutex)
{
  if (_log_validptr(mutex))
    {
      int op = pthread_mutex_unlock(mutex);
      _log_handleerr(op);
      return 0 == op;
    }

  return false;
}

bool
_logmutex_destroy(logmutex_t *mutex)
{
  if (_log_validptr(mutex))
    {
      int op = pthread_mutex_destroy(mutex);
      _log_handleerr(op);
      return 0 == op;
    }

  return false;
}

#else /* Win32 mutex implementation */

static bool _logmutex_waitwin32(logmutex_t mutex, DWORD msec);

bool
_logmutex_create(logmutex_t *mutex)
{
  if (_log_validptr(mutex))
    {
      logmutex_t tmp = CreateMutex(NULL, FALSE, NULL);

      if (!tmp)
        {
          _log_handlewin32err(GetLastError());
          return false;
        }

      *mutex = tmp;
      return true;
    }

  return false;
}

bool
_logmutex_trylock(logmutex_t *mutex)
{
  return _logmutex_waitwin32(*mutex, 0);
}

bool
_logmutex_lock(logmutex_t *mutex)
{
  return _logmutex_waitwin32(*mutex, INFINITE);
}

bool
_logmutex_unlock(logmutex_t *mutex)
{
  if (_log_validptr(mutex))
    {
      BOOL release = ReleaseMutex(*mutex);

      if (!release)
        {
          _log_handlewin32err(GetLastError());
        }

      return FALSE != release;
    }

  return false;
}

bool
_logmutex_destroy(logmutex_t *mutex)
{
  if (_log_validptr(mutex))
    {
      BOOL close = CloseHandle(*mutex);

      if (!close)
        {
          _log_handlewin32err(GetLastError());
        }

      return FALSE != close;
    }

  return false;
}

static bool
_logmutex_waitwin32(logmutex_t mutex, DWORD msec)
{
  if (_log_validptr(mutex))
    {
      DWORD wait = WaitForSingleObject(mutex, msec);

      switch (wait)
        {
        case WAIT_ABANDONED:
        case WAIT_FAILED:
          _log_selflog(
            "%s: warning: WaitForSingleObject returned 0x%08lx; danger ahead\n",
            __func__,
            wait);
          return WAIT_FAILED != wait;

          break;

        case WAIT_TIMEOUT:
        case WAIT_OBJECT_0:
          return true;
        }
    }

  return false;
}

#endif /* !_WIN32 */
