/*
 * scspell-id: d6ad8321-2b0d-11ed-9408-80ee73e9b8e7
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

#include "sir.h"
#include "sirdefaults.h"
#include "sirfilecache.h"
#include "sirinternal.h"
#include "sirtextstyle.h"

bool
log_init(sirinit *si)
{
  return _log_init(si);
}

bool
log_stdoutlevels(log_levels levels)
{
  _log_defaultlevels(&levels, log_stdout_def_lvls);
  log_update_data data = {
    &levels, NULL
  };

  return _log_writeinit(&data, _log_stdoutlevels);
}

bool
log_stdoutopts(log_options opts)
{
  _log_defaultopts(&opts, log_stdout_def_opts);
  log_update_data data = {
    NULL, &opts
  };

  return _log_writeinit(&data, _log_stdoutopts);
}

bool
log_stderrlevels(log_levels levels)
{
  _log_defaultlevels(&levels, log_stderr_def_lvls);
  log_update_data data = {
    &levels, NULL
  };

  return _log_writeinit(&data, _log_stderrlevels);
}

bool
log_stderropts(log_options opts)
{
  _log_defaultopts(&opts, log_stderr_def_opts);
  log_update_data data = {
    NULL, &opts
  };

  return _log_writeinit(&data, _log_stderropts);
}

bool
log_sysloglevels(log_levels levels)
{
#ifndef LOG_NO_SYSLOG
  _log_defaultlevels(&levels, log_syslog_def_lvls);
  log_update_data data = {
    &levels, NULL
  };
  return _log_writeinit(&data, _log_sysloglevels);
#else /* ifndef LOG_NO_SYSLOG */
  return false;
#endif /* ifndef LOG_NO_SYSLOG */
}

bool
log_filelevels(sirfileid_t id, log_levels levels)
{
  _log_defaultlevels(&levels, log_file_def_lvls);
  log_update_data data = {
    &levels, NULL
  };

  return _log_updatefile(id, &data);
}

bool
log_fileopts(sirfileid_t id, log_options opts)
{
  _log_defaultopts(&opts, log_file_def_opts);
  log_update_data data = {
    NULL, &opts
  };

  return _log_updatefile(id, &data);
}

bool
log_cleanup(void)
{
  return _log_cleanup();
}

uint16_t
log_geterror(sirchar_t message[LOG_MAXERROR - 1])
{
  return _log_geterrcode(_log_geterror(message));
}

bool
log_debug(const sirchar_t *format, ...)
{
  _LOG_L_START(format);
  r = _log_logv(SIRL_DEBUG, format, args);
  _LOG_L_END(args);
  return r;
}

bool
log_info(const sirchar_t *format, ...)
{
  _LOG_L_START(format);
  r = _log_logv(SIRL_INFO, format, args);
  _LOG_L_END(args);
  return r;
}

bool
log_notice(const sirchar_t *format, ...)
{
  _LOG_L_START(format);
  r = _log_logv(SIRL_NOTICE, format, args);
  _LOG_L_END(args);
  return r;
}

bool
log_warn(const sirchar_t *format, ...)
{
  _LOG_L_START(format);
  r = _log_logv(SIRL_WARN, format, args);
  _LOG_L_END(args);
  return r;
}

bool
log_error(const sirchar_t *format, ...)
{
  _LOG_L_START(format);
  r = _log_logv(SIRL_ERROR, format, args);
  _LOG_L_END(args);
  return r;
}

bool
log_crit(const sirchar_t *format, ...)
{
  _LOG_L_START(format);
  r = _log_logv(SIRL_CRIT, format, args);
  _LOG_L_END(args);
  return r;
}

bool
log_alert(const sirchar_t *format, ...)
{
  _LOG_L_START(format);
  r = _log_logv(SIRL_ALERT, format, args);
  _LOG_L_END(args);
  return r;
}

bool
log_emerg(const sirchar_t *format, ...)
{
  _LOG_L_START(format);
  r = _log_logv(SIRL_EMERG, format, args);
  _LOG_L_END(args);
  return r;
}

sirfileid_t
log_addfile(const sirchar_t *path, log_levels levels, log_options opts)
{
  return _log_addfile(path, levels, opts);
}

bool
log_remfile(sirfileid_t id)
{
  return _log_remfile(id);
}

bool
log_settextstyle(log_level level, log_textstyle style)
{
  (void)log_override_styles;
  return _log_settextstyle(level, style);
}

bool
log_resettextstyles(void)
{
  return _log_resettextstyles();
}
