/*
 * SPDX-License-Identifier: MIT
 * scspell-id: 23f9365c-2b0e-11ed-bb28-80ee73e9b8e7
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

#include "sirhelpers.h"
#include "sirerrors.h"

bool
_log_validfid(int id)
{
  bool valid = id >= 0;

  if (!valid)
    {
      _log_seterror(_LOG_E_NOFILE);
      assert(valid);
    }

  return valid;
}

bool
_log_validlevels(log_levels levels)
{
  bool valid = levels <= LOGL_ALL;

  if (!valid)
    {
      _log_seterror(_LOG_E_LEVELS);
      assert(valid);
    }

  return valid;
}

bool
_log_validlevel(log_level level)
{
  bool valid = 0 != level && !( level & ( level - 1 ));

  if (!valid)
    {
      _log_seterror(_LOG_E_LEVELS);
      assert(valid);
    }

  return valid;
}

bool
_log_validopts(log_options opts)
{
  bool valid = ( opts & LOGL_ALL ) == 0 && opts <= 0xfff00;

  if (!valid)
    {
      _log_seterror(_LOG_E_OPTIONS);
      assert(valid);
    }

  return valid;
}

bool
__log_validstr(const logchar_t *str, bool fail)
{
  bool valid = str && ( *str != (logchar_t)'\0' );

  if (!valid && fail)
    {
      _log_seterror(_LOG_E_STRING);
      assert(valid);
    }

  return valid;
}
