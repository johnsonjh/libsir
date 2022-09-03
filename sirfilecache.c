/*
 * SPDX-License-Identifier: MIT
 * scspell-id: 0c8622d6-2b0e-11ed-90cd-80ee73e9b8e7
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

#include "sirfilecache.h"
#include "sirdefaults.h"
#include "sirinternal.h"
#include "sirmutex.h"

volatile unsigned long long int sir_sequence_counter = 0;

logfileid_t
_log_addfile(const logchar_t *path, log_levels levels, log_options opts)
{
  _log_seterror(_LOG_E_NOERROR);

  if (_log_sanity())
    {
      logfcache *sfc = _log_locksection(_LOGM_FILECACHE);
      assert(sfc);

      if (sfc)
        {
          _log_defaultlevels (&levels, log_file_def_lvls);
          _log_defaultopts   (&opts,   log_file_def_opts);

          logfileid_t r = _log_fcache_add(sfc, path, levels, opts);
          (void)_log_unlocksection(_LOGM_FILECACHE);
          return r;
        }
    }

  return NULL;
}

bool
_log_updatefile(logfileid_t id, log_update_data *data)
{
  _log_seterror(_LOG_E_NOERROR);

  if (_log_sanity() && _log_validptr(id) && _log_validfid(*id)
      && _log_validupdatedata(data))
    {
      logfcache *sfc = _log_locksection(_LOGM_FILECACHE);
      assert(sfc);

      if (sfc)
        {
          bool r = _log_fcache_update(sfc, id, data);
          return _log_unlocksection(_LOGM_FILECACHE) && r;
        }
    }

  return false;
}

bool
_log_remfile(logfileid_t id)
{
  _log_seterror(_LOG_E_NOERROR);

  if (_log_sanity() && _log_validptr(id) && _log_validfid(*id))
    {
      logfcache *sfc = _log_locksection(_LOGM_FILECACHE);
      assert(sfc);

      if (sfc)
        {
          bool r = _log_fcache_rem(sfc, id);
          return _log_unlocksection(_LOGM_FILECACHE) && r;
        }
    }

  return false;
}

logfile *
_logfile_create(const logchar_t *path, log_levels levels, log_options opts)
{
  logfile *sf = NULL;

  if (_log_validstr(path) && _log_validlevels(levels)
      && _log_validopts(opts))
    {
      sf = (logfile *)calloc(1, sizeof ( logfile ));

      if (_log_validptr(sf))
        {
          size_t pathLen = strnlen(path, LOG_MAXPATH);
          sf->path = (logchar_t *)calloc(pathLen + 1, sizeof ( logchar_t ));

          if (_log_validptr(sf->path))
            {
              (void)strncpy(sf->path, path, pathLen);

              sf->levels = levels;
              sf->opts   = opts;

              if (!_logfile_open(sf) || !_logfile_validate(sf))
                {
                  _log_safefree(sf);
				  return NULL;
                }
            }
        }
    }

  return sf;
}

bool
_logfile_open(logfile *sf)
{
  if (_log_validptr(sf) && _log_validstr(sf->path))
    {
      FILE *f = _log_fopen(sf->path);

      if (f)
        {
          int fd = fileno(f);
          if (-1 == fd)
            {
              _log_handleerr(errno);
            }

          if (_log_validfid(fd))
            {
              _logfile_close(sf);

              sf->f  = f;
              sf->id = fd;
              return true;
            }
        }
    }

  return false;
}

void
_logfile_close(logfile *sf)
{
  if (_log_validptr(sf))
    {
      if (_log_validptr(sf->f) && _log_validfid(sf->id))
        {
          _log_fflush(sf->f);
          _log_fclose(&sf->f);
          sf->id = LOG_INVALID;
        }
    }
}

bool
_logfile_write(logfile *sf, const logchar_t *output)
{
  if (_logfile_validate(sf) && _log_validstr(output))
    {
      if (_logfile_needsroll(sf))
        {
          bool rolled        = false;
          logchar_t *newpath = NULL;

          if (_logfile_roll(sf, &newpath))
            {
              logchar_t header[LOG_MAXMESSAGE] = { 0 };
              (void)snprintf(header, LOG_MAXMESSAGE, LOG_FHROLLED);
              rolled = _logfile_writeheader(sf, header);
            }

          _log_safefree(newpath);
          if (!rolled)
            {
              return false;
            }
        }

      size_t writeLen = strnlen(output, LOG_MAXOUTPUT);
      size_t write    = fwrite(output, sizeof ( logchar_t ), writeLen, sf->f);

      assert(write == writeLen);

      if (write < writeLen)
        {
          int err = ferror(sf->f);
          int eof = feof(sf->f);

          _log_handleerr(err);
          _log_handleerr(eof);

          _log_selflog(
            "%s: wrote %'lu/%'lu bytes to %d; ferror: %d, feof: %d\n",
            __func__,
            write,
            writeLen,
            sf->id,
            err,
            eof);

          /*
           * TODO: If an error occurs on write, consider
           * removing file from targets, or at least attempt
           * to roll the file (out of space?)
           */

          clearerr(sf->f);
        }

      return write == writeLen;
    }

  return false;
}

bool
_logfile_writeheader(logfile *sf, const logchar_t *msg)
{
  if (_logfile_validate(sf) && _log_validstr(msg))
    {
      time_t now;
      bool gettime = _log_getlocaltime(&now, NULL);
      assert(gettime);

      if (gettime)
        {
          logchar_t timestamp[LOG_MAXTIME] = { 0 };
          bool fmttime = _log_formattime(now, timestamp, LOG_FHTIMEFORMAT);
          assert(fmttime);

          if (fmttime)
            {
              logchar_t header[LOG_MAXOUTPUT] = { 0 };
              int fmt = snprintf(header, LOG_MAXOUTPUT, LOG_FHFORMAT, msg, timestamp);

              if (fmt < 0)
                {
                  _log_handleerr(errno);
                }

              return fmt >= 0 && _logfile_write(sf, header);
            }
        }
    }

  return false;
}

bool
_logfile_needsroll(logfile *sf)
{
  if (_logfile_validate(sf))
    {
      struct stat st = { 0 };
      int getstat = fstat(sf->id, &st);

      if (0 != getstat)
        {
          _log_handleerr(errno);
          return false;
        }

      return st.st_size >= LOG_FROLLSIZE;
    }

  return false;
}

bool
_logfile_roll(logfile *sf, logchar_t **newpath)
{
  if (_logfile_validate(sf) && newpath)
    {
      bool r          = false;
      logchar_t *name = NULL;
      logchar_t *ext  = NULL;

      bool split = _logfile_splitpath(sf, &name, &ext);
      assert(split);

      if (split)
        {
          time_t now;

          bool gettime = _log_getlocaltime(&now, NULL);
          assert(gettime);

          if (gettime)
            {
              logchar_t timestamp[LOG_MAXTIME] = {
                0
              };
              bool fmttime = _log_formattime(now, timestamp, LOG_FNAMETIMEFORMAT);
              assert(fmttime);

              if (fmttime)
                {
                  *newpath = (logchar_t *)calloc(LOG_MAXPATH, sizeof ( logchar_t ));

                  if (_log_validptr(*newpath))
                    {
                      int fmtpath
                        = snprintf(
                            *newpath,
                            LOG_MAXPATH,
                            LOG_FNAMEFORMAT,
                            name,
                            timestamp,
							sir_sequence_counter++,
                            _log_validstrnofail(ext) ? ext : "");

                      if (fmtpath < 0)
                        {
                          _log_handleerr(errno);
                        }

                      if (access(*newpath, F_OK) == 0)
                        {
                          _log_safefree(name);
                          _log_safefree(ext);
                          return NULL;
                        }

                      r = fmtpath >= 0 && _logfile_archive(sf, *newpath);
                    }
                }
            }
        }

      _log_safefree(name);
      _log_safefree(ext);
      return r;
    }

  return false;
}

bool
_logfile_archive(logfile *sf, const logchar_t *newpath)
{
  if (_logfile_validate(sf) && _log_validstr(newpath))
    {
#ifdef _WIN32
      /* Need to close the old file first on windows. */
      _logfile_close(sf);
#endif /* ifdef _WIN32 */

      if (access(newpath, F_OK) == 0)
	    {
          _log_selflog(
            "%s: '%s' exists!\n",
            __func__,
			newpath);
		  return false;
        }

      if (0 != rename(sf->path, newpath))
        {
          _log_handleerr(errno);
          return false;
        }

      if (_logfile_open(sf))
        {
          _log_selflog(
            "%s: archived '%s' -> '%s'\n",
            __func__,
            sf->path,
            newpath);
          return true;
        }
    }

  return false;
}

bool
_logfile_splitpath(logfile *sf, logchar_t **name, logchar_t **ext)
{
  if (name)
    {
      *name = NULL;
    }

  if (ext)
    {
      *ext = NULL;
    }

  if (_logfile_validate(sf) && _log_validptr(name) && _log_validptr(ext))
    {
      logchar_t *lastfullstop = strrchr(sf->path, '.');

      if (lastfullstop)
        {
          uintptr_t namesize = lastfullstop - sf->path;
          assert(namesize < LOG_MAXPATH);

          if (namesize < LOG_MAXPATH)
            {
              *name = (logchar_t *)calloc(namesize + 1, sizeof ( logchar_t ));
              (void)strncpy(*name, sf->path, namesize);
            }

          *ext = strdup(lastfullstop);
        }
      else
        {
          *name = strdup(sf->path);
        }

      return _log_validstr(*name) && ( !lastfullstop || _log_validstr(*ext));
    }

  return false;
}

void
_logfile_destroy(logfile *sf)
{
  if (sf)
    {
      _logfile_close (sf);
      _log_safefree  (sf->path);
      _log_safefree  (sf);
    }
}

bool
_logfile_validate(logfile *sf)
{
  return _log_validptr(sf) && _log_validfid(sf->id) && _log_validptr(sf->f)
         && _log_validstr(sf->path);
}

void
_logfile_update(logfile *sf, log_update_data *data)
{
  if (_logfile_validate(sf) && _log_validupdatedata(data))
    {
      if (data->levels && _log_validlevels(*data->levels))
        {
          sf->levels = *data->levels;
        }

      if (data->opts && _log_validopts(*data->opts))
        {
          sf->opts = *data->opts;
        }
    }
}

logfileid_t
_log_fcache_add(logfcache *sfc, const logchar_t *path, log_levels levels,
                log_options opts)
{
  if (_log_validptr(sfc) && _log_validstr(path) && _log_validlevels(levels)
      && _log_validopts(opts))
    {
      if (sfc->count >= LOG_MAXFILES)
        {
          _log_seterror(_LOG_E_FCFULL);
          return NULL;
        }

      logfile *existing
        = _log_fcache_find(sfc, (const void *)path, _log_fcache_pred_path);

      if (NULL != existing)
        {
          _log_seterror(_LOG_E_DUPFILE);
          _log_selflog(
            "%s: file with path '%s' already added.\n",
            __func__,
            path);
          return NULL;
        }

      logfile *sf = _logfile_create(path, levels, opts);

      if (_logfile_validate(sf))
        {
          sfc->files[sfc->count++] = sf;

          if (!_log_bittest(sf->opts, SIRO_NOHDR))
            {
              (void)_logfile_writeheader(sf, LOG_FHBEGIN);
            }

          return &sf->id;
        }
    _log_safefree(sf);
    }

  return NULL;
}

bool
_log_fcache_update(logfcache *sfc, logfileid_t id, log_update_data *data)
{
  if (_log_validptr(sfc) && _log_validptr(id) && _log_validfid(*id)
      && _log_validupdatedata(data))
    {
      logfile *found
        = _log_fcache_find(sfc, (const void *)id, _log_fcache_pred_id);
      if (!found)
        {
          _log_seterror(_LOG_E_NOFILE);
          return false;
        }

      _logfile_update(found, data);
      return true;
    }

  return false;
}

bool
_log_fcache_rem(logfcache *sfc, logfileid_t id)
{
  if (_log_validptr(sfc) && _log_validptr(id) && _log_validfid(*id))
    {
      for (size_t n = 0; n < sfc->count; n++)
        {
          assert(_logfile_validate(sfc->files[n]));

          if (sfc->files[n]->id == *id)
            {
              _logfile_destroy(sfc->files[n]);

              for (size_t i = n; i < sfc->count - 1; i++)
                {
                  sfc->files[i]     = sfc->files[i + 1];
                  sfc->files[i + 1] = NULL;
                }

              assert(sfc->count > 0);
              sfc->count--;

              return true;
            }
        }

      _log_seterror(_LOG_E_NOFILE);
    }

  return false;
}

bool
_log_fcache_pred_path(const void *match, logfile *iter)
{
  const logchar_t *path = (const logchar_t *)match;

#ifndef _WIN32
  return 0 == strncmp(path, iter->path, LOG_MAXPATH);
#else /* ifndef _WIN32 */
  /* paths/file names are not case sensitive on windows. */
  return 0 == _strnicmp(path, iter->path, LOG_MAXPATH);
#endif /* ifndef _WIN32 */
}

bool
_log_fcache_pred_id(const void *match, logfile *iter)
{
  logfileid_t id = (logfileid_t)match;

  return iter->id == *id;
}

logfile *
_log_fcache_find(logfcache *sfc, const void *match, log_fcache_pred pred)
{
  if (_log_validptr(sfc) && _log_validptr(match) && _log_validptr(pred))
    {
      for (size_t n = 0; n < sfc->count; n++)
        {
          if (pred(match, sfc->files[n]))
            {
              return sfc->files[n];
            }
        }
    }

  return NULL;
}

bool
_log_fcache_destroy(logfcache *sfc)
{
  if (_log_validptr(sfc))
    {
      for (size_t n = 0; n < sfc->count; n++)
        {
          assert(_logfile_validate(sfc->files[n]));
          _logfile_destroy(sfc->files[n]);
          sfc->files[n] = NULL;
          assert(sfc->count > 0);
          sfc->count--;
        }

      (void)memset(sfc, 0, sizeof ( logfcache ));
      return true;
    }

  return false;
}

bool
_log_fcache_dispatch(logfcache *sfc, log_level level, logoutput *output,
                     size_t *dispatched, size_t *wanted)
{
  if (_log_validptr(sfc) && _log_validlevel(level) && _log_validptr(output)
      && _log_validptr(dispatched) && _log_validptr(wanted))
    {
      bool r                 = true;
      const logchar_t *write = NULL;
      log_options lastopts   = 0;

      *dispatched = 0;
      *wanted     = 0;

      for (size_t n = 0; n < sfc->count; n++)
        {
          assert(_logfile_validate(sfc->files[n]));

          if (!_log_bittest(sfc->files[n]->levels, level))
            {
              _log_selflog(
                "%s: levels for %d (%04lx) not set for (%04lx); skipping...\n",
                __func__,
                sfc->files[n]->id,
                sfc->files[n]->levels,
                level);
              continue;
            }

          ( *wanted )++;

          if (!write || sfc->files[n]->opts != lastopts)
            {
              write = _log_format(false, sfc->files[n]->opts, output);
              assert(write);
              lastopts = sfc->files[n]->opts;
            }

          if (write && _logfile_write(sfc->files[n], write))
            {
              r &= true;
              ( *dispatched )++;
            }
          else
            {
              _log_selflog(
                "%s: write to %d failed!\n",
                __func__,
                sfc->files[n]->id);
            }
        }

      if (*dispatched > 0)
        {
          (void)_log_fflush_all();
        }

      return r && ( *dispatched == *wanted );
    }

  return false;
}

FILE *
_log_fopen(const logchar_t *path)
{
  if (_log_validstr(path))
    {
#ifdef __STDC_SECURE_LIB__
      FILE *tmp    = NULL;
      errno_t open = fopen_s(&tmp, path, LOG_FOPENMODE);
      _log_handleerr(open);
      return tmp;
#else /* ifdef __STDC_SECURE_LIB__ */
      FILE *f = fopen(path, LOG_FOPENMODE);
      if (!f)
        {
          _log_handleerr(errno);
        }

      return f;
#endif /* ifdef __STDC_SECURE_LIB__ */
    }

  return NULL;
}

void
_log_fclose(FILE **f)
{
  if (_log_validptr(f) && _log_validptr(*f))
    {
      if (0 != fclose(*f))
        {
          _log_handleerr(errno);
        }

      *f = NULL;
    }
}

void
_log_fflush(FILE *f)
{
  if (_log_validptr(f))
    {
      if (0 != fflush(f))
        {
          _log_handleerr(errno);
        }
    }
}

bool
_log_fflush_all(void)
{
  if (0 != fflush(NULL))
    {
      _log_handleerr(errno);
      return false;
    }

  return true;
}
