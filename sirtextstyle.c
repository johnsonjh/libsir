/*
 * scspell-id: bc3d6378-2b0e-11ed-86f8-80ee73e9b8e7
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

#include "sirtextstyle.h"
#include "sirdefaults.h"
#include "sirinternal.h"

bool
_log_validstyle(log_textstyle style, uint32_t *pattr, uint32_t *pfg,
                uint32_t *pbg)
{
  uint32_t attr = ( style & _SIRS_ATTR_MASK );
  uint32_t fg   = ( style & _SIRS_FG_MASK );
  uint32_t bg   = ( style & _SIRS_BG_MASK );

  bool attrvalid = attr <= SIRS_DIM;
  bool fgvalid   = fg   <= SIRS_FG_DEFAULT;
  bool bgvalid   = bg   <= SIRS_BG_DEFAULT;

  (void)log_override_styles;

  if (pattr && pfg && pbg)
    {
      *pattr = attrvalid ? attr : 0;
      *pfg   = fgvalid   ? fg   : 0;
      *pbg   = bgvalid   ? bg   : 0;
    }

  if (!attrvalid || !fgvalid || !bgvalid)
    {
      _log_seterror(_LOG_E_TEXTSTYLE);

      assert(attrvalid);
      assert(fgvalid);
      assert(bgvalid);
      return false;
    }

  return true;
}

log_textstyle
_log_gettextstyle(log_level level)
{
  if (_log_validlevel(level))
    {
      log_style_map *map = _log_locksection(_LOGM_TEXTSTYLE);
      assert(map);

      if (map)
        {
          log_textstyle found = SIRS_INVALID;
          bool override = false;

          for (size_t n = 0; n < LOG_NUMLEVELS; n++)
            {
              if (map[n].level == level && map[n].style != SIRS_INVALID)
                {
                  override = true;
                  found = map[n].style;
                  break;
                }
            }

          if (!override)
            {
              found = _log_getdefstyle(log_default_styles, level);
            }

          (void)_log_unlocksection(_LOGM_TEXTSTYLE);
          return found;
        }
    }

  return SIRS_INVALID;
}

log_textstyle
_log_getdefstyle(const log_style_map *map, log_level level)
{
  if (_log_validlevel(level))
    {
      if (map)
        {
          log_textstyle found = SIRS_INVALID;
          for (size_t n = 0; n < LOG_NUMLEVELS; n++)
            {
              if (map[n].level == level)
                {
                  found = map[n].style;
                  break;
                }
            }

          return found;
        }
    }

  return SIRS_INVALID;
}

bool
_log_settextstyle(log_level level, log_textstyle style)
{
  _log_seterror(_LOG_E_NOERROR);

  if (_log_sanity() && _log_validlevel(level)
      && _log_validstyle(style, NULL, NULL, NULL))
    {
      log_style_map *map = _log_locksection(_LOGM_TEXTSTYLE);
      assert(map);

      if (map)
        {
          bool updated = false;
          for (size_t n = 0; n < LOG_NUMLEVELS; n++)
            {
              if (map[n].level == level)
                {
                  map[n].style = style;
                  updated      = true;
                  break;
                }
            }

          return _log_unlocksection(_LOGM_TEXTSTYLE) && updated;
        }
    }

  return false;
}

bool
_log_resettextstyles(void)
{
  log_style_map *map = _log_locksection(_LOGM_TEXTSTYLE);

  assert(map);

  if (map)
    {
      for (size_t n = 0; n < LOG_NUMLEVELS; n++)
        {
          map[n].style = SIRS_INVALID;
        }

      (void)_log_unlocksection(_LOGM_TEXTSTYLE);
      return true;
    }

  return false;
}

uint16_t
_log_getprivstyle(uint32_t cat)
{
  for (size_t n = 0; n < _log_countof(log_priv_map); n++)
    {
      if (log_priv_map[n].from == cat)
        {
          return log_priv_map[n].to;
        }
    }

  return _log_getprivstyle(SIRS_NONE);
}

bool
_log_formatstyle(log_textstyle style, logchar_t *buf, size_t size)
{
  if (_log_validptr(buf))
    {
      uint32_t attr;
      uint32_t fg;
      uint32_t bg;

      if (_log_validstyle(style, &attr, &fg, &bg))
        {
          uint16_t privattr = _log_getprivstyle(attr);
          uint16_t privfg   = _log_getprivstyle(fg);
          uint16_t privbg   = _log_getprivstyle(bg);

#ifndef _WIN32
          logchar_t fgfmt[5] = {
            0
          };
          logchar_t bgfmt[5] = {
            0
          };

          if (privfg != 0)
            {
              (void)snprintf(fgfmt, 5, ";%03hhu", (unsigned char)privfg);
            }

          if (privbg != 0)
            {
              (void)snprintf(bgfmt, 5, ";%03hhu", (unsigned char)privbg);
            }

          /* '\033[n;nnn;nnnm' */
          (void)snprintf(buf, size, "\033[%.1hu%s%sm", privattr, fgfmt, bgfmt);

          return _log_validstr(buf);

#else /* ifndef _WIN32 */
          uint16_t final = privattr | privfg | privbg;
          (void)memcpy(buf, &final, sizeof ( uint16_t ));
          return true;
#endif /* ifndef _WIN32 */
        }
    }

  return false;
}
