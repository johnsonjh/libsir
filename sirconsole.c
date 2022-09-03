/*
 * SPDX-License-Identifier: MIT
 * scspell-id: eb67ec00-2b0d-11ed-b65e-80ee73e9b8e7
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

#include "sirconsole.h"
#include "sirinternal.h"
#include "sirtextstyle.h"

#ifndef _WIN32

static bool _log_write_std(const logchar_t *message, FILE *stream);

bool
_log_stderr_write(const logchar_t *message)
{
  return _log_write_std(message, stderr);
}

bool
_log_stdout_write(const logchar_t *message)
{
  return _log_write_std(message, stdout);
}

static bool
_log_write_std(const logchar_t *message, FILE *stream)
{
  (void)log_override_styles;
  if (!_log_validstr(message) || !_log_validptr(stream))
    {
      return false;
    }

  if (EOF == fputs(message, stream))
    {
      _log_handleerr(errno);
      return false;
    }

  return true;
}

#else /* ifndef _WIN32 */

static CRITICAL_SECTION stdout_cs;
static logonce_t stdout_once = LOG_ONCE_INIT;

static CRITICAL_SECTION stderr_cs;
static logonce_t stderr_once = LOG_ONCE_INIT;

static bool _log_write_stdwin32(uint16_t style, const logchar_t *message,
                                HANDLE console, CRITICAL_SECTION *cs);
static BOOL CALLBACK _log_initcs(PINIT_ONCE ponce, PVOID param, PVOID *ctx);

bool
_log_stderr_write(uint16_t style, const logchar_t *message)
{
  BOOL initcs
    = InitOnceExecuteOnce(&stderr_once, _log_initcs, &stderr_cs, NULL);

  assert(FALSE != initcs);
  return _log_write_stdwin32(
    style,
    message,
    GetStdHandle(STD_ERROR_HANDLE),
    &stderr_cs);
}

bool
_log_stdout_write(uint16_t style, const logchar_t *message)
{
  BOOL initcs
    = InitOnceExecuteOnce(&stdout_once, _log_initcs, &stdout_cs, NULL);

  assert(FALSE != initcs);
  return _log_write_stdwin32(
    style,
    message,
    GetStdHandle(STD_OUTPUT_HANDLE),
    &stdout_cs);
}

static bool
_log_write_stdwin32(uint16_t style, const logchar_t *message, HANDLE console,
                    CRITICAL_SECTION *cs)
{
  if (!_log_validstr(message))
    {
      return false;
    }

  assert(INVALID_HANDLE_VALUE != console);
  if (INVALID_HANDLE_VALUE == console)
    {
      _log_handlewin32err(GetLastError());
      return false;
    }

  if (!_log_validptr(cs))
    {
      return false;
    }

  CONSOLE_SCREEN_BUFFER_INFO csbfi = {
    0
  };

  EnterCriticalSection(cs);
  if (!GetConsoleScreenBufferInfo(console, &csbfi))
    {
      _log_handlewin32err(GetLastError());
      return false;
    }

  if (!SetConsoleTextAttribute(console, style))
    {
      _log_handlewin32err(GetLastError());
      return false;
    }

  size_t chars = strnlen(message, LOG_MAXOUTPUT) - 1;
  DWORD written = 0;

  do
    {
      DWORD pass = 0;

      if (!WriteConsole(
            console,
            message + written,
            chars   - written,
            &pass,
            NULL))
        {
          _log_handlewin32err(GetLastError());
          break;
        }

      written += pass;
    }
  while (written < chars);

  /*
   * Have to set the attributes back before writing the newline
   * or you get background color artifacting before the next output.
   */

  SetConsoleTextAttribute(console, csbfi.wAttributes);
  DWORD newline = 0;

  WriteConsole(console, "\n", 1, &newline, NULL);
  LeaveCriticalSection(cs);

  return written == chars;
}

static BOOL CALLBACK
_log_initcs(PINIT_ONCE ponce, PVOID param, PVOID *ctx)
{
  InitializeCriticalSection((LPCRITICAL_SECTION)param);
  return TRUE;
}

#endif /* !_WIN32 */
