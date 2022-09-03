/*
 * scspell-id: 312060a7-2b0e-11ed-abd0-80ee73e9b8e7
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

#include "sirinternal.h"
#include "sirconsole.h"
#include "sirdefaults.h"
#include "sirfilecache.h"
#include "sirmutex.h"
#include "sirtextstyle.h"

static loginit _log_si = {
  0
};
static logfcache _log_fc = {
  0
};

static logmutex_t si_mutex;
static logonce_t  si_once = LOG_ONCE_INIT;

static logmutex_t fc_mutex;
static logonce_t  fc_once = LOG_ONCE_INIT;

static logmutex_t ts_mutex;
static logonce_t  ts_once = LOG_ONCE_INIT;

static volatile uint32_t _log_magic;

bool
_log_sanity(void)
{
  if (_LOG_MAGIC == _log_magic)
    {
      return true;
    }

  _log_seterror(_LOG_E_NOTREADY);
  return false;
}

bool
_log_options_sanity(const loginit *si)
{
  if (!_log_validptr(si))
    {
      return false;
    }

  bool levelcheck = true;

  levelcheck &= _log_validlevels(si->d_stdout.levels);
  levelcheck &= _log_validlevels(si->d_stderr.levels);

#ifndef LOG_NO_SYSLOG
  levelcheck &= _log_validlevels(si->d_syslog.levels);
#endif /* ifndef LOG_NO_SYSLOG */

  bool optscheck = true;
  optscheck &= _log_validopts(si->d_stdout.opts);
  optscheck &= _log_validopts(si->d_stderr.opts);

  char *nullterm = strrchr(si->processName, '\0');
  return levelcheck && optscheck && _log_validptr(nullterm);
}

bool
_log_init(loginit *si)
{
  _log_seterror(_LOG_E_NOERROR);

  if (!_log_validptr(si))
    {
      return false;
    }

  if (_log_magic == _LOG_MAGIC)
    {
      _log_seterror(_LOG_E_ALREADY);
      return false;
    }

  _log_defaultlevels (&si->d_stdout.levels, log_stdout_def_lvls);
  _log_defaultopts   (&si->d_stdout.opts,   log_stdout_def_opts);

  _log_defaultlevels (&si->d_stderr.levels, log_stderr_def_lvls);
  _log_defaultopts   (&si->d_stderr.opts,   log_stderr_def_opts);

#ifndef LOG_NO_SYSLOG
  _log_defaultlevels (&si->d_syslog.levels, log_syslog_def_lvls);
#endif /* ifndef LOG_NO_SYSLOG */

  if (!_log_options_sanity(si))
    {
      return false;
    }

  loginit *_si = _log_locksection(_LOGM_INIT);
  assert(_si);

  if (_si)
    {
      (void)memcpy(_si, si, sizeof ( loginit ));

#ifndef LOG_NO_SYSLOG
      if (0 != _si->d_syslog.levels)
        {
          openlog(
            _log_validstrnofail(_si->processName) ? _si->processName : "",
            ( _si->d_syslog.includePID ? LOG_PID : 0 ) | LOG_ODELAY, LOG_USER);
        }
#endif /* ifndef LOG_NO_SYSLOG */

      _log_magic = _LOG_MAGIC;
      (void)_log_unlocksection(_LOGM_INIT);
      return true;
    }

  return false;
}

void
_log_stdoutlevels(loginit *si, log_update_data *data)
{
  si->d_stdout.levels = *data->levels;
}

void
_log_stdoutopts(loginit *si, log_update_data *data)
{
  si->d_stdout.opts = *data->opts;
}

void
_log_stderrlevels(loginit *si, log_update_data *data)
{
  si->d_stderr.levels = *data->levels;
}

void
_log_stderropts(loginit *si, log_update_data *data)
{
  si->d_stderr.opts = *data->opts;
}

void
_log_sysloglevels(loginit *si, log_update_data *data)
{
  si->d_syslog.levels = *data->levels;
}

bool
_log_writeinit(log_update_data *data, loginit_update update)
{
  _log_seterror(_LOG_E_NOERROR);

  if (_log_sanity() && _log_validupdatedata(data) && _log_validptr(update))
    {
      loginit *si = _log_locksection(_LOGM_INIT);
      assert(si);
      if (si)
        {
          update(si, data);
          return _log_unlocksection(_LOGM_INIT);
        }
    }

  return false;
}

void *
_log_locksection(log_mutex_id mid)
{
  logmutex_t *m = NULL;
  void *sec     = NULL;

  if (_log_mapmutexid(mid, &m, &sec))
    {
      bool enter = _logmutex_lock(m);
      assert(enter);
      return enter ? sec : NULL;
    }

  return NULL;
}

bool
_log_unlocksection(log_mutex_id mid)
{
  logmutex_t *m = NULL;
  void *sec     = NULL;

  if (_log_mapmutexid(mid, &m, &sec))
    {
      bool leave = _logmutex_unlock(m);
      assert(leave);
      return leave;
    }

  return false;
}

bool
_log_mapmutexid(log_mutex_id mid, logmutex_t **m, void **section)
{
  if (!_log_validptr(m))
    {
      return false;
    }

  logmutex_t *tmpm;
  void *tmpsec;

  switch (mid)
    {
    case _LOGM_INIT:
      _log_once(&si_once, _log_initmutex_si_once);
      tmpm   = &si_mutex;
      tmpsec = &_log_si;
      break;

    case _LOGM_FILECACHE:
      _log_once(&fc_once, _log_initmutex_fc_once);
      tmpm   = &fc_mutex;
      tmpsec = &_log_fc;
      break;

    case _LOGM_TEXTSTYLE:
      _log_once(&ts_once, _log_initmutex_ts_once);
      tmpm   = &ts_mutex;
      tmpsec = log_override_styles;
      break;

    default:
      tmpm   = NULL;
      tmpsec = NULL;
    }

  *m = tmpm;
  if (section)
    {
      *section = tmpsec;
    }

  return *m != NULL && ( !section || *section != NULL );
}

bool
_log_cleanup(void)
{
  if (!_log_sanity())
    {
      return false;
    }

  bool cleanup   = true;
  logfcache *sfc = _log_locksection(_LOGM_FILECACHE);

  assert(sfc);

  if (cleanup &= NULL != sfc)
    {
      bool destroyfc = _log_fcache_destroy(sfc);
      assert(destroyfc);
      cleanup &= _log_unlocksection(_LOGM_FILECACHE) && destroyfc;
    }

  loginit *si = _log_locksection(_LOGM_INIT);

  assert(si);

  if (cleanup &= NULL != si)
    {
      (void)memset(si, 0, sizeof ( loginit ));
      cleanup &= _log_unlocksection(_LOGM_INIT);
    }

  (void)_log_resettextstyles();
  _log_magic = 0;
  _log_selflog("%s: libsir is cleaned up\n", __func__);
  return cleanup;
}

#ifndef _WIN32
void
_log_initmutex_si_once(void)
{
  _log_initmutex(&si_mutex);
}

void
_log_initmutex_fc_once(void)
{
  _log_initmutex(&fc_mutex);
}

void
_log_initmutex_ts_once(void)
{
  _log_initmutex(&ts_mutex);
}
#else  /* ifndef _WIN32 */
BOOL CALLBACK
_log_initmutex_si_once(PINIT_ONCE ponce, PVOID param, PVOID *ctx)
{
  _log_initmutex(&si_mutex);
  return TRUE;
}

BOOL CALLBACK
_log_initmutex_fc_once(PINIT_ONCE ponce, PVOID param, PVOID *ctx)
{
  _log_initmutex(&fc_mutex);
  return TRUE;
}

BOOL CALLBACK
_log_initmutex_ts_once(PINIT_ONCE ponce, PVOID param, PVOID *ctx)
{
  _log_initmutex(&ts_mutex);
  return TRUE;
}
#endif /* ifndef _WIN32 */

void
_log_initmutex(logmutex_t *mutex)
{
  bool init = _logmutex_create(mutex);

  (void)init;
  assert(init);
}

void
_log_once(logonce_t *once, log_once_fn func)
{
#ifndef _WIN32
  (void)pthread_once(once, func);
#else  /* ifndef _WIN32 */
  BOOL result   = InitOnceExecuteOnce(once, func, NULL, NULL);
  assert(FALSE != result);
#endif /* ifndef _WIN32 */
}

bool
_log_logv(log_level level, const logchar_t *format, va_list args)
{
  _log_seterror(_LOG_E_NOERROR);

  if (!_log_sanity() || !_log_validlevel(level) || !_log_validstr(format))
    {
      return false;
    }

  loginit *si = _log_locksection(_LOGM_INIT);

  if (!si)
    {
      return false;
    }

  loginit tmpsi = {
    0
  };

  (void)memcpy(&tmpsi, si, sizeof ( loginit ));
  (void)_log_unlocksection(_LOGM_INIT);

  logbuf buf;
  logoutput output = {
    0
  };

  output.style = _logbuf_get(&buf, _SIRBUF_STYLE);
  assert(output.style);

  bool appliedstyle = false;
  log_textstyle style = _log_gettextstyle(level);

  assert(SIRS_INVALID != style);

  if (SIRS_INVALID != style)
    {
      bool fmtstyle = _log_formatstyle(style, output.style, LOG_MAXSTYLE);
      assert(fmtstyle);
      appliedstyle = fmtstyle;
    }

  if (!appliedstyle)
    {
      _log_resetstr(output.style);
    }

  output.timestamp = _logbuf_get(&buf, _SIRBUF_TIME);
  assert(output.timestamp);

  time_t now;
  long long nowmsec;
  bool gettime = _log_getlocaltime(&now, &nowmsec);

  assert(gettime);

  if (gettime)
    {
      bool fmttime = _log_formattime(now, output.timestamp, LOG_TIMEFORMAT);
      assert(fmttime);

      if (!fmttime)
        {
          _log_resetstr(output.timestamp);
        }

      output.msec = _logbuf_get(&buf, _SIRBUF_MSEC);
      assert(output.msec);

      int fmtmsec = snprintf(output.msec, LOG_MAXMSEC, LOG_MSECFORMAT, nowmsec);
      assert(fmtmsec >= 0);

      if (fmtmsec < 0)
        {
          _log_resetstr(output.msec);
        }
    }
  else
    {
      _log_resetstr(output.msec);
    }

  output.level = _logbuf_get(&buf, _SIRBUF_LEVEL);
  assert(output.level);
  (void)snprintf(
    output.level,
    LOG_MAXLEVEL,
    LOG_LEVELFORMAT,
    _log_levelstr(level));

  output.name = _logbuf_get(&buf, _SIRBUF_NAME);
  assert(output.name);

  if (_log_validstrnofail(tmpsi.processName))
    {
      (void)strncpy(output.name, tmpsi.processName, LOG_MAXNAME);
    }
  else
    {
      _log_resetstr(output.name);
    }

  output.pid = _logbuf_get(&buf, _SIRBUF_PID);
  assert(output.pid);

  pid_t pid  = _log_getpid();
  int pidfmt = snprintf(output.pid, LOG_MAXPID, LOG_PIDFORMAT, pid);

  assert(pidfmt >= 0);

  if (pidfmt < 0)
    {
      _log_resetstr(output.pid);
    }

  pid_t tid = _log_gettid();

  output.tid = _logbuf_get(&buf, _SIRBUF_TID);
  assert(output.tid);

  if (tid != pid)
    {
      if (!_log_getthreadname(output.tid))
        {
          pidfmt = snprintf(output.tid, LOG_MAXPID, LOG_PIDFORMAT, tid);
          assert(pidfmt >= 0);

          if (pidfmt < 0)
            {
              _log_resetstr(output.tid);
            }
        }
    }
  else
    {
      _log_resetstr(output.tid);
    }

  /* TODO: Add support for glibc's %m? */
  output.message = _logbuf_get(&buf, _SIRBUF_MSG);
  assert(output.message);
  int msgfmt = vsnprintf(output.message, LOG_MAXMESSAGE, format, args);

  assert(msgfmt >= 0);

  if (msgfmt < 0)
    {
      _log_resetstr(output.message);
    }

  output.output = _logbuf_get(&buf, _SIRBUF_OUTPUT);
  assert(output.output);

  return _log_dispatch(&tmpsi, level, &output);
}

bool
_log_dispatch(loginit *si, log_level level, logoutput *output)
{
  if (_log_validptr(output))
    {
      bool r            = true;
      size_t dispatched = 0;
      size_t wanted     = 0;

      if (_log_bittest(si->d_stdout.levels, level))
        {
          const logchar_t *write /* = write */ = _log_format(true, si->d_stdout.opts, output);
          (void)write;
          assert(write);
#ifndef _WIN32
          bool wrote  = _log_stdout_write(write);
          r          &= NULL != write && wrote;
#else  /* ifndef _WIN32 */
          uint16_t *style  = (uint16_t *)output->style;
          bool wrote       = _log_stdout_write(*style, write);
          r               &= NULL != write && NULL != style && wrote;
#endif /* ifndef _WIN32 */
          if (wrote)
            {
              dispatched++;
            }

          wanted++;
        }

      if (_log_bittest(si->d_stderr.levels, level))
        {
          const logchar_t *write /* = write */ = _log_format(true, si->d_stderr.opts, output);
          (void)write;
          assert(write);
#ifndef _WIN32
          bool wrote  = _log_stderr_write(write);
          r          &= NULL != write && wrote;
#else  /* ifndef _WIN32 */
          uint16_t *style  = (uint16_t *)output->style;
          bool wrote       = _log_stderr_write(*style, write);
          r               &= NULL != write && NULL != style && wrote;
#endif /* ifndef _WIN32 */
          if (wrote)
            {
              dispatched++;
            }

          wanted++;
        }

#ifndef LOG_NO_SYSLOG
      if (_log_bittest(si->d_syslog.levels, level))
        {
          syslog(_log_syslog_maplevel(level), "%s", output->message);
          dispatched++;
          wanted++;
        }

#endif /* ifndef LOG_NO_SYSLOG */
      logfcache *sfc = _log_locksection(_LOGM_FILECACHE);

      if (sfc)
        {
          size_t fdispatched  = 0;
          size_t fwanted      = 0;
          r                  &= _log_fcache_dispatch(sfc, level, output, &fdispatched, &fwanted);
          r                  &= _log_unlocksection(_LOGM_FILECACHE);
          dispatched         += fdispatched;
          wanted             += fwanted;
        }

      if (0 == wanted)
        {
          _log_seterror(_LOG_E_NODEST);
          return false;
        }

      return r && ( dispatched == wanted );
    }

  return false;
}

const logchar_t *
_log_format(bool styling, log_options opts, logoutput *output)
{
  if (_log_validopts(opts) && _log_validptr(output)
      && _log_validptr(output->output))
    {
      bool first = true;

      _log_resetstr(output->output);

#ifndef _WIN32
      if (styling)
        {
          (void)strncat(output->output, output->style, LOG_MAXSTYLE);
        }
#endif /* ifndef _WIN32 */

      if (!_log_bittest(opts, SIRO_NOTIME))
        {
          (void)strncat(output->output, output->timestamp, LOG_MAXTIME);
          first = false;

#ifdef LOG_MSEC_TIMER
          if (!_log_bittest(opts, SIRO_NOMSEC))
            {
              (void)strncat(output->output, output->msec, LOG_MAXMSEC);
            }
#endif /* ifdef LOG_MSEC_TIMER */
        }

      if (!_log_bittest(opts, SIRO_NOLEVEL))
        {
          if (!first)
            {
              (void)strcat(output->output, " ");
            }

          (void)strncat(output->output, output->level, LOG_MAXLEVEL);
          first = false;
        }

      bool name = false;
      if (!_log_bittest(opts, SIRO_NONAME)
          && _log_validstrnofail(output->name))
        {
          if (!first)
            {
              (void)strcat(output->output, " ");
            }

          (void)strncat(output->output, output->name, LOG_MAXNAME);
          first = false;
          name  = true;
        }

      bool wantpid = !_log_bittest(opts, SIRO_NOPID) && _log_validstrnofail(output->pid);
      bool wanttid = !_log_bittest(opts, SIRO_NOTID) && _log_validstrnofail(output->tid);

      if (wantpid || wanttid)
        {
          if (name)
            {
              (void)strcat(output->output, "(");
            }
          else if (!first)
            {
              (void)strcat(output->output, " ");
            }

          if (wantpid)
            {
              (void)strncat(output->output, output->pid, LOG_MAXPID);
            }

          if (wanttid)
            {
              if (wantpid)
                {
                  (void)strcat(output->output, LOG_PIDSEPARATOR);
                }

              (void)strncat(output->output, output->tid, LOG_MAXPID);
            }

          if (name)
            {
              (void)strcat(output->output, ")");
            }
        }

      if (!first)
        {
          (void)strcat(output->output, ": ");
        }

      (void)strncat(output->output, output->message, LOG_MAXMESSAGE);

#ifndef _WIN32
      if (styling)
        {
          (void)strncat(output->output, LOG_ENDSTYLE, LOG_MAXSTYLE);
        }
#endif /* ifndef _WIN32 */

      (void)strcat(output->output, "\n");
      return output->output;
    }

  return NULL;
}

#ifndef LOG_NO_SYSLOG
int
_log_syslog_maplevel(log_level level)
{
  assert(_log_validlevel(level));

  switch (level)
    {
    case SIRL_EMERG:
      return LOG_EMERG;

    case SIRL_ALERT:
      return LOG_ALERT;

    case SIRL_CRIT:
      return LOG_CRIT;

    case SIRL_ERROR:
      return LOG_ERR;

    case SIRL_WARN:
      return LOG_WARNING;

    case SIRL_NOTICE:
      return LOG_NOTICE;

    case SIRL_INFO:
      return LOG_INFO;

    case SIRL_DEBUG:
      return LOG_DEBUG;

    default:
      /* assert(false); */
      return LOG_INFO;
    }
}
#endif /* ifndef LOG_NO_SYSLOG */

/* In case there's a better way to implement this, abstract it away. */
logchar_t *
_logbuf_get(logbuf *buf, size_t idx)
{
  assert(idx <= _SIRBUF_MAX);

  switch (idx)
    {
    case _SIRBUF_STYLE:
      return buf->style;

    case _SIRBUF_TIME:
      return buf->timestamp;

    case _SIRBUF_MSEC:
      return buf->msec;

    case _SIRBUF_LEVEL:
      return buf->level;

    case _SIRBUF_NAME:
      return buf->name;

    case _SIRBUF_PID:
      return buf->pid;

    case _SIRBUF_TID:
      return buf->tid;

    case _SIRBUF_MSG:
      return buf->message;

    case _SIRBUF_OUTPUT:
      return buf->output;

    default:
	  ;
      /* assert(false); */
    }

  return NULL;
}

const logchar_t *
_log_levelstr(log_level level)
{
  assert(_log_validlevel(level));

  switch (level)
    {
    case SIRL_INFO:
      return SIRL_S_INFO;

    case SIRL_NOTICE:
      return SIRL_S_NOTICE;

    case SIRL_WARN:
      return SIRL_S_WARN;

    case SIRL_ERROR:
      return SIRL_S_ERROR;

    case SIRL_CRIT:
      return SIRL_S_CRIT;

    case SIRL_ALERT:
      return SIRL_S_ALERT;

    case SIRL_EMERG:
      return SIRL_S_EMERG;

    case SIRL_DEBUG:
    default:
      return SIRL_S_DEBUG;
    }
}

bool
_log_formattime(time_t now, logchar_t *buffer, const logchar_t *format)
{
  if (0 != now && _log_validptr(buffer) && _log_validstr(format))
    {
      size_t fmttime = strftime(buffer, LOG_MAXTIME, format, localtime(&now));
      assert(0 != fmttime);

      if (0 == fmttime)
        {
          _log_selflog(
            "%s: strftime returned 0; format string: '%s'",
            __func__,
            format);
        }

      return 0 != fmttime;
    }

  return false;
}

bool
_log_getlocaltime(time_t *tbuf, long long *nsecbuf)
{
  if (tbuf)
    {
      (void)time(tbuf);
#ifdef LOG_MSEC_POSIX
      struct timespec ts = {
        0
      };
      int clock = clock_gettime(LOG_MSECCLOCK, &ts);
      assert(0 == clock);

      if (0 == clock)
        {
          if (nsecbuf)
            {
              *nsecbuf = ( ts.tv_nsec / 1e6 );
              assert(*nsecbuf < 1000);
            }
        }
      else
        {
          *nsecbuf = 0;
          _log_selflog("%s: clock_gettime failed; errno: %d\n", __func__, errno);
        }

#elif defined( LOG_MSEC_WIN32 )
      static const ULONGLONG uepoch = 116444736e9;

      FILETIME ftutc = {
        0
      };
      GetSystemTimePreciseAsFileTime(&ftutc);

      ULARGE_INTEGER ftnow;
      ftnow.HighPart = ftutc.dwHighDateTime;
      ftnow.LowPart  = ftutc.dwLowDateTime;
      ftnow.QuadPart = ( ftnow.QuadPart - uepoch ) / 1e7;

      *tbuf = (time_t)ftnow.QuadPart;

      if (nsecbuf)
        {
          SYSTEMTIME st = {
            0
          };
          FileTimeToSystemTime(&ftutc, &st);
          *nsecbuf = st.wMilliseconds;
        }

#else /* ifdef LOG_MSEC_POSIX */
      (void)time(tbuf);
      if (nsecbuf)
        {
          *nsecbuf = 0;
        }

#endif /* ifdef LOG_MSEC_POSIX */
      return true;
    }

  return false;
}

pid_t
_log_getpid(void)
{
#ifndef _WIN32
  return getpid();
#else /* ifndef _WIN32 */
  return (pid_t)GetProcessId(GetCurrentProcess());
#endif /* ifndef _WIN32 */
}

pid_t
_log_gettid(void)
{
  pid_t tid = 0;

#ifdef __MACOS__
  uint64_t tid64 = 0;
  int gettid     = pthread_threadid_np(NULL, &tid64);
  if (0 != gettid)
    {
      _log_handleerr(gettid);
    }

  tid = (pid_t)tid64;
#elif defined( _WIN32 )
  tid = (pid_t)GetCurrentThreadId();
#elif defined(_AIX)
  tid = (pid_t)thread_self();
#else  /* ifdef __MACOS__ */
  tid = syscall(SYS_gettid);
#endif /* ifdef __MACOS__ */
  return tid;
}

bool
_log_getthreadname(char name[LOG_MAXPID])
{
  (void)name;
#if defined(_GNU_SOURCE) && !defined(_AIX)
  return 0 == pthread_getname_np(pthread_self(), name, LOG_MAXPID);
#else /* if defined(_GNU_SOURCE) && !defined(_AIX) */
  return false;
#endif /* if defined(_GNU_SOURCE) && !defined(_AIX) */
}
