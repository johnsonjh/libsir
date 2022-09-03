/*
 * scspell-id: e076319a-2b0e-11ed-836b-80ee73e9b8e7
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

#include "tests.h"

static const log_test log_tests[] = {
  { "performance",             sirtest_perf                  },
  { "multi-thread race",       sirtest_mthread_race          },
  { "exceed max buffer size",  sirtest_exceedmaxsize         },
  { "file cache sanity",       sirtest_filecachesanity       },
  { "set invalid text style",  sirtest_failsetinvalidstyle   },
  { "no output destination",   sirtest_failnooutputdest      },
  { "invalid file name",       sirtest_failinvalidfilename   },
  { "bad file permissions",    sirtest_failfilebadpermission },
  { "null pointers",           sirtest_failnulls             },
  { "output without init",     sirtest_failwithoutinit       },
  { "superfluous init",        sirtest_failinittwice         },
  { "output after cleanup",    sirtest_failaftercleanup      },
  { "re-initialize",           sirtest_initcleanupinit       },
  { "duplicate file name",     sirtest_faildupefile          },
  { "remove nonexistent file", sirtest_failremovebadfile     },
  { "roll/archive large file", sirtest_rollandarchivefile    },
  { "error handling sanity",   sirtest_errorsanity           },
  { "text style sanity",       sirtest_textstylesanity       },
  { "update levels/options",   sirtest_updatesanity          },
};

static const char *arg_wait
  = "--wait";  /* Wait for key press before exiting.              */
static const char *arg_perf
  = "--perf";  /* Run performance test instead of standard tests. */

int
main(int argc, char **argv)
{
  bool wait = false;
  bool perf = false;

  for (size_t n = 1; n < (size_t)argc; n++)
    {
      if (0 == strncmp(argv[n], arg_wait, strlen(arg_wait)))
        {
          wait = true;
        }
      else if (0 == strncmp(argv[n], arg_perf, strlen(arg_perf)))
        {
          perf = true;
        }
    }

  bool allpass = true;
  size_t tests = ( perf ? 1 : sizeof ( log_tests ) / sizeof ( log_test ));
  size_t first = ( perf ? 0 : 1 );
  size_t passed = 0;
  sirtimer_t timer = {
    0
  };

  printf(WHITE("running %lu libsir test(s)...\n"), tests);

  if (!startsirtimer(&timer))
    {
      printf(
        RED(
          "failed to start timer; elapsed time won't be measured "
          "correctly!") "\n");
    }

  for (size_t n = first; n < tests; n++)
    {
      printf(WHITE("\t'%s'...") "\n", log_tests[n].name);
      bool thispass = log_tests[n].fn();
      allpass &= thispass;
      passed += ( thispass ? 1 : 0 );
      printf(
        WHITE("\t'%s' finished; result: ") "%s\n",
        log_tests[n].name,
        thispass ? GREEN("PASS") : RED("FAIL"));
    }

  float elapsed = sirtimerelapsed(&timer);

  printf(
    WHITE("done; ")
    BLUE("%lu/%lu libsir test(s) passed in %.04fsec") "\n",
    passed,
    tests - first,
    elapsed / 1e3);

  if (wait)
    {
      printf(WHITE("press any key to exit") "\n");
      getc(stdin);
    }

  return allpass ? EXIT_SUCCESS : EXIT_FAILURE;
}

bool
sirtest_exceedmaxsize(void)
{
  INIT(si, SIRL_ALL, 0, 0, 0);
  bool pass = si_init;

  char toobig[LOG_MAXMESSAGE + 100] = {
    0
  };

  (void)memset(toobig, 'a', LOG_MAXMESSAGE - 99);

  pass &= log_info(toobig);

  log_cleanup();
  return printerror(pass);
}

bool
sirtest_filecachesanity(void)
{
  INIT(si, SIRL_ALL, 0, 0, 0);
  bool pass = si_init;

  size_t numfiles = LOG_MAXFILES + 1;
  sirfileid_t ids[LOG_MAXFILES] = {
    0
  };

  log_options even = SIRO_MSGONLY;
  log_options odd = 0;

  for (size_t n = 0; n < numfiles - 1; n++)
    {
      char path[LOG_MAXPATH] = {
        0
      };
      (void)snprintf(path, LOG_MAXPATH, "test-%lu.log", n);
      rmfile(path);
      ids[n] = log_addfile(path, SIRL_ALL, ( n % 2 ) ? odd : even);
      pass &= NULL != ids[n] && log_info("test %u", n);
    }

  pass &= log_info("test test test");

  /* This one should fail; max files already added */
  pass &= NULL == log_addfile("should-fail.log", SIRL_ALL, SIRO_MSGONLY);

  log_info("test test test");

  /* Now remove previously added files in a different order */
  int removeorder[LOG_MAXFILES];

  (void)memset(removeorder, -1, sizeof ( removeorder ));

  long processed = 0;

  printf("\tcreating random file ID order...\n");

  do
    {
      unsigned int rnd = getrand() % LOG_MAXFILES;
      bool skip = false;

      for (size_t n = 0; n < LOG_MAXFILES; n++)
        {
          if ((long long)removeorder[(long long)n] == (long long)rnd)
            {
              skip = true;
              break;
            }
        }

      if (skip)
        {
          continue;
        }

      removeorder[processed++] = rnd;

      if (processed == LOG_MAXFILES)
        {
          break;
        }
    }
  while (true);

  printf("\tremove order: {");
  for (size_t n = 0; n < LOG_MAXFILES; n++)
    {
      printf(" %d", removeorder[n]);
    }

  printf(" }...\n");

  for (size_t n = 0; n < LOG_MAXFILES; n++)
    {
      pass &= log_remfile(ids[removeorder[n]]);

      char path[LOG_MAXPATH] = {
        0
      };
      (void)snprintf(path, LOG_MAXPATH, "test-%lu.log", n);
      rmfile(path);
    }

  pass &= log_info("test test test");

  log_cleanup();
  return printerror(pass);
}

bool
sirtest_failsetinvalidstyle(void)
{
  INIT(si, SIRL_ALL, 0, 0, 0);
  bool pass = si_init;

  pass &= !log_settextstyle(SIRL_INFO, 0xfefe);
  pass &= !log_settextstyle(SIRL_ALL, SIRS_FG_RED | SIRS_FG_DEFAULT);

  if (pass)
    {
      printexpectederr();
    }

  log_cleanup();
  return printerror(pass);
}

bool
sirtest_failnooutputdest(void)
{
  INIT(si, 0, 0, 0, 0);
  bool pass = si_init;
  const char *logfile = "levels.log";

  pass &= !log_info("this goes nowhere!");

  if (pass)
    {
      printexpectederr();

      pass &= log_stdoutlevels(SIRL_INFO);
      pass &= log_info("this goes to stdout");
      pass &= log_stdoutlevels(SIRL_NONE);

      pass &= NULL != log_addfile(logfile, SIRL_INFO, SIRO_DEFAULT);
      pass &= log_info("this goes to %s", logfile);

      rmfile(logfile);
    }

  log_cleanup();
  return printerror(pass);
}

bool
sirtest_failinvalidfilename(void)
{
  INIT(si, SIRL_ALL, 0, 0, 0);
  bool pass = si_init;

  pass &= NULL == log_addfile("bad file!/name", SIRL_ALL, SIRO_MSGONLY);

  if (pass)
    {
      printexpectederr();
    }

  log_cleanup();
  return printerror(pass);
}

bool
sirtest_failfilebadpermission(void)
{
  INIT(si, SIRL_ALL, 0, 0, 0);
  bool pass = si_init;

  pass &= NULL == log_addfile("/noperms", SIRL_ALL, SIRO_MSGONLY);

  if (pass)
    {
      printexpectederr();
    }

  log_cleanup();
  return printerror(pass);
}

bool
sirtest_failnulls(void)
{
  INIT(si, SIRL_ALL, 0, 0, 0);
  bool pass = si_init;

  pass &= !log_info(NULL);
  pass &= NULL == log_addfile(NULL, SIRL_ALL, SIRO_MSGONLY);

  if (pass)
    {
      printexpectederr();
    }

  log_cleanup();
  return printerror(pass);
}

bool
sirtest_failwithoutinit(void)
{
  bool pass = !log_info("sir isn't initialized; this needs to fail");

  if (pass)
    {
      printexpectederr();
    }

  return printerror(pass);
}

bool
sirtest_failinittwice(void)
{
  INIT(si, SIRL_ALL, 0, 0, 0);
  bool pass = si_init;

  INIT(si2, SIRL_ALL, 0, 0, 0);
  pass &= !si2_init;

  if (pass)
    {
      printexpectederr();
    }

  log_cleanup();
  return printerror(pass);
}

bool
sirtest_failaftercleanup()
{
  INIT(si, SIRL_ALL, 0, 0, 0);
  bool pass = si_init;

  log_cleanup();
  pass &= !log_info("already cleaned up; this needs to fail");

  if (pass)
    {
      printexpectederr();
    }

  return printerror(pass);
}

bool
sirtest_initcleanupinit(void)
{
  INIT(si1, SIRL_ALL, 0, 0, 0);
  bool pass = si1_init;

  pass &= log_info("init called once; testing output...");
  log_cleanup();

  INIT(si2, SIRL_ALL, 0, 0, 0);
  pass &= si2_init;

  pass &= log_info("init called again after re-init; testing output...");
  log_cleanup();

  return printerror(pass);
}

bool
sirtest_faildupefile(void)
{
  INIT(si, SIRL_ALL, 0, 0, 0);
  bool pass = si_init;

  pass &= NULL != log_addfile("foo.log", SIRL_ALL, SIRO_DEFAULT);
  pass &= NULL == log_addfile("foo.log", SIRL_ALL, SIRO_DEFAULT);

  rmfile("foo.log");
  log_cleanup();
  return printerror(pass);
}

bool
sirtest_failremovebadfile(void)
{
  INIT(si, SIRL_ALL, 0, 0, 0);
  bool pass = si_init;

  int invalidid = 9999999;

  pass &= !log_remfile(&invalidid);

  log_cleanup();
  return printerror(pass);
}

bool
sirtest_rollandarchivefile(void)
{
  /* Roll size minus 1KB so we can write until it maxes. */
  const long deltasize               = 1024;
  const long fillsize                = LOG_FROLLSIZE - deltasize;
  const sirchar_t *const logfilename = "rollandarchive";
  const sirchar_t *const line        = "hello, i am some data. nice to meet you.";

  unsigned delcount = 0;

  if (!enumfiles(logfilename, deletefiles, &delcount))
    {
      fprintf(
        stderr,
        "\tfailed to delete existing log files! error: %d\n",
        getoserr());
      return false;
    }

  if (delcount > 0)
    {
      printf("\tfound and removed %u log file(s)\n", delcount);
    }

  FILE *f = fopen(logfilename, "w");

  if (!f)
    {
      fprintf(stderr, "\tfopen failed! error: %d\n", errno);
      return false;
    }

  if (0 != fseek(f, fillsize, SEEK_SET))
    {
      fprintf(stderr, "\tfseek failed! error: %d\n", errno);
      fclose(f);
      return false;
    }

  if (EOF == fputc('\0', f))
    {
      fprintf(stderr, "\tfputc failed! error: %d\n", errno);
      fclose(f);
      return false;
    }

  fclose(f);

  INIT(si, 0, 0, 0, 0);
  bool pass = si_init;

  sirfileid_t fileid
    = log_addfile(logfilename, SIRL_DEBUG, SIRO_MSGONLY | SIRO_NOHDR);

  if (pass &= NULL != fileid)
    {
      /* Write an (approximately) known quantity until we should have rolled */
      size_t written = 0;
      size_t linesize = strlen(line);

      do
        {
          pass &= log_debug("%s", line);
          if (!pass)
            {
              break;
            }

          written += linesize;
        }
      while (written < deltasize + ( linesize * 50 ));

      /* Look for files matching the original name. */
      unsigned foundlogs = 0;
      if (!enumfiles(logfilename, countfiles, &foundlogs))
        {
          fprintf(stderr, "\tfailed to count log files! error: %d\n", errno);
          pass = false;
        }

      /* If two are present, the test is a pass. */
      pass &= foundlogs == 2;
    }

  if (pass)
    {
      pass &= log_remfile(fileid);
    }

  delcount = 0;
  if (!enumfiles(logfilename, deletefiles, &delcount))
    {
      fprintf(stderr, "\tfailed to delete log files! error: %d\n", getoserr());
      return false;
    }

  if (delcount > 0)
    {
      printf("\tfound and removed %u log file(s)\n", delcount);
    }

  log_cleanup();
  return printerror(pass);
}

bool
sirtest_errorsanity(void)
{
  INIT(si, SIRL_ALL, 0, 0, 0);
  bool pass = si_init;

  struct
  {
    uint16_t code;
    const char *name;
  } errors[] = {
    { LOG_E_NOERROR,   "LOG_E_NOERROR"   }, /* = 0    */
    { LOG_E_NOTREADY,  "LOG_E_NOTREADY"  }, /* = 1    */
    { LOG_E_ALREADY,   "LOG_E_ALREADY"   }, /* = 2    */
    { LOG_E_DUPFILE,   "LOG_E_DUPFILE"   }, /* = 3    */
    { LOG_E_NOFILE,    "LOG_E_NOFILE"    }, /* = 4    */
    { LOG_E_FCFULL,    "LOG_E_FCFULL"    }, /* = 5    */
    { LOG_E_OPTIONS,   "LOG_E_OPTIONS"   }, /* = 6    */
    { LOG_E_LEVELS,    "LOG_E_LEVELS"    }, /* = 7    */
    { LOG_E_TEXTSTYLE, "LOG_E_TEXTSTYLE" }, /* = 8    */
    { LOG_E_STRING,    "LOG_E_STRING"    }, /* = 9    */
    { LOG_E_NODEST,    "LOG_E_NODEST"    }, /* = 10   */
    { LOG_E_PLATFORM,  "LOG_E_PLATFORM"  }, /* = 11   */
    { LOG_E_UNKNOWN,   "LOG_E_UNKNOWN"   }, /* = 4095 */
  };

  sirchar_t message[LOG_MAXERROR] = {
    0
  };

  for (size_t n = 0; n < ( sizeof ( errors ) / sizeof ( errors[0] )); n++)
    {
      _log_seterror(_log_mkerror(errors[n].code));
      (void)memset(message, 0, LOG_MAXERROR);
      uint16_t err /* = err */ = log_geterror(message);
      (void)err;
      pass &= errors[n].code == err && *message != '\0';
      printf("\t%s = %s\n", errors[n].name, message);
    }

  log_cleanup();
  return printerror(pass);
}

bool
sirtest_textstylesanity(void)
{
  INIT(si, SIRL_ALL, 0, 0, 0);
  bool pass = si_init;

  if (pass)
    {
      pass &= log_debug("default style");
      pass &= log_settextstyle(SIRL_DEBUG, SIRS_FG_YELLOW | SIRS_BG_DGRAY);
      pass &= log_debug("override style");

      pass &= log_info("default style");
      pass &= log_settextstyle(SIRL_INFO, SIRS_FG_GREEN | SIRS_BG_MAGENTA);
      pass &= log_info("override style");

      pass &= log_notice("default style");
      pass &= log_settextstyle(SIRL_NOTICE, SIRS_FG_BLACK | SIRS_BG_LYELLOW);
      pass &= log_notice("override style");

      pass &= log_warn("default style");
      pass &= log_settextstyle(SIRL_WARN, SIRS_FG_BLACK | SIRS_BG_WHITE);
      pass &= log_warn("override style");

      pass &= log_error("default style");
      pass &= log_settextstyle(SIRL_ERROR, SIRS_FG_WHITE | SIRS_BG_BLUE);
      pass &= log_error("override style");

      pass &= log_crit("default style");
      pass &= log_settextstyle(SIRL_CRIT, SIRS_FG_DGRAY | SIRS_BG_LGREEN);
      pass &= log_crit("override style");

      pass &= log_alert("default style");
      pass &= log_settextstyle(SIRL_ALERT, SIRS_BRIGHT | SIRS_FG_LBLUE);
      pass &= log_alert("override style");

      pass &= log_emerg("default style");
      pass &= log_settextstyle(SIRL_EMERG, SIRS_BRIGHT | SIRS_FG_DGRAY);
      pass &= log_emerg("override style");
    }

  printf("\tcleanup to reset styles...\n");
  log_cleanup();

  INIT(si2, SIRL_ALL, 0, 0, 0);
  pass &= si2_init;

  pass &= log_debug  ("default style");
  pass &= log_info   ("default style");
  pass &= log_notice ("default style");
  pass &= log_warn   ("default style");
  pass &= log_error  ("default style");
  pass &= log_crit   ("default style");
  pass &= log_alert  ("default style");
  pass &= log_emerg  ("default style");

  log_cleanup();

  return printerror(pass);
}

bool
sirtest_perf(void)
{
  const sirchar_t *logfilename = "sirperf";

#ifndef _WIN32
  const size_t perflines = 1e6;
#else  /* ifndef _WIN32 */
  /* stdio is hilariously slow on windows; do less. */
  const size_t perflines = 1e4;
#endif /* ifndef _WIN32 */
  INIT(si, SIRL_ALL, 0, 0, 0);
  bool pass = si_init;

  if (pass)
    {
      float printfelapsed = 0.0f;
      float stdioelapsed  = 0.0f;
      float fileelapsed   = 0.0f;

      printf("\t%lu lines printf...\n", perflines);

      sirtimer_t printftimer = {
        0
      };
      startsirtimer(&printftimer);

      for (size_t n = 0; n < perflines; n++)
        {
#ifndef _WIN32
          printf("\033[97mlorem ipsum foo bar blah\033[0m\n");
#else  /* ifndef _WIN32 */
          printf("lorem ipsum foo bar blah\n");
#endif /* ifndef _WIN32 */
        }

      printfelapsed = sirtimerelapsed(&printftimer);

      printf("\t%lu lines libsir stdout...\n", perflines);

      sirtimer_t stdiotimer = {
        0
      };
      startsirtimer(&stdiotimer);

      for (size_t n = 0; n < perflines; n++)
        {
          log_debug("lorem ipsum foo bar blah");
        }

      stdioelapsed = sirtimerelapsed(&stdiotimer);

      log_cleanup();

      INIT(si2, 0, 0, 0, 0);
      pass &= si2_init;

      sirfileid_t logid = log_addfile(logfilename, SIRL_ALL, SIRO_MSGONLY);
      pass &= NULL != logid;

      if (pass)
        {
          printf("\t%lu lines log file...\n", perflines);

          sirtimer_t filetimer = {
            0
          };
          startsirtimer(&filetimer);

          for (size_t n = 0; n < perflines; n++)
            {
              log_debug("lorem ipsum foo bar blah");
            }

          fileelapsed = sirtimerelapsed(&filetimer);

          pass &= log_remfile(logid);
        }

      if (pass)
        {
          printf("\t" WHITE("%lu lines printf:")
                 " "  GREEN("%.2fsec (%.1f lines/sec)") "\n",
            perflines,    printfelapsed / 1e3,
            perflines / ( printfelapsed / 1e3 ));
          printf("\t" WHITE("%lu lines libsir stdout:")
                 " "  GREEN("%.2fsec (%.1f lines/sec)") "\n",
            perflines,    stdioelapsed / 1e3,
            perflines / ( stdioelapsed / 1e3 ));
          printf("\t" WHITE("%lu lines log file:")
                 " "  GREEN("%.2fsec (%.1f lines/sec)") "\n",
            perflines,    fileelapsed / 1e3,
            perflines / ( fileelapsed / 1e3 ));
        }
    }

  unsigned deleted = 0;
  enumfiles(logfilename, deletefiles, &deleted);

  if (deleted > 0)
    {
      printf("\tdeleted %d log file(s)\n", deleted);
    }

  log_cleanup();
  return printerror(pass);
}

bool
sirtest_updatesanity(void)
{
  INIT_N(si, SIRL_DEFAULT, 0, SIRL_DEFAULT, 0, "update_sanity");
  bool pass = si_init;
  const char *logfile = "update.log";

  rmfile(logfile);
  sirfileid_t id1 = log_addfile(logfile, SIRL_DEFAULT, SIRO_DEFAULT);

  pass &= NULL != id1;

  if (pass)
    {
      pass &= log_debug        ("default config");
      pass &= log_info         ("default config");
      pass &= log_notice       ("default config");
      pass &= log_warn         ("default config");
      pass &= log_error        ("default config");
      pass &= log_crit         ("default config");
      pass &= log_alert        ("default config");
      pass &= log_emerg        ("default config");

      pass &= log_stdoutlevels (SIRL_DEBUG);
      pass &= log_stdoutopts   (SIRO_NOTIME);
      pass &= log_stderrlevels (SIRL_ALL);
      pass &= log_stderropts   (SIRO_NONAME);

      pass &= log_filelevels   (id1, SIRL_DEBUG);
      pass &= log_fileopts     (id1, SIRO_MSGONLY);

      pass &= log_debug        ("modified config");
      pass &= log_info         ("modified config");
      pass &= log_notice       ("modified config");
      pass &= log_warn         ("modified config");
      pass &= log_error        ("modified config");
      pass &= log_crit         ("modified config");
      pass &= log_alert        ("modified config");
      pass &= log_emerg        ("modified config");
      pass &= log_remfile      (id1);
    }

  rmfile(logfile);

  printerror(pass);
  log_cleanup();
  return pass;
}

/*
 * bool sirtest_XXX(void) {
 *
 *  INIT(si, SIRL_ALL, 0, 0, 0);
 *  bool pass = si_init;
 *
 *  log_cleanup();
 *  return printerror(pass);
 * }
 */

#ifndef _WIN32
static void *sirtest_thread(void *arg);
#else  /* ifndef _WIN32 */
static unsigned sirtest_thread(void *arg);
#endif /* ifndef _WIN32 */

#define NUM_THREADS 2

bool
sirtest_mthread_race(void)
{
#ifndef _WIN32
  pthread_t thrds[NUM_THREADS];
#else  /* ifndef _WIN32 */
  uintptr_t thrds[NUM_THREADS];
#endif /* ifndef _WIN32 */

  INIT_N(si, SIRL_ALL, SIRO_NOPID, 0, 0, "multi-thread race");
  bool pass = si_init;

  for (size_t n = 0; n < NUM_THREADS; n++)
    {
      char *path = (char *)calloc(LOG_MAXPATH, sizeof ( char ));
      (void)snprintf(path, LOG_MAXPATH, "%lu.log", n);

#ifndef _WIN32
      int create
        = pthread_create(&thrds[n], NULL, sirtest_thread, (void *)path);
      if (0 != create)
        {
          errno = create;
#else  /* ifndef _WIN32 */
      thrds[n] = _beginthreadex(NULL, 0, sirtest_thread, (void *)path, 0, NULL);
      if (0 == thrds[n])
        {
#endif /* ifndef _WIN32 */
          printf(RED("\tfailed to create thread; err: %d") "\n", errno);
          pass = false;
        }

#if defined(_GNU_SOURCE) && !defined(_AIX)
      char thrd_name[LOG_MAXPID];
      (void)snprintf(thrd_name, LOG_MAXPID, "%lu", n);
      create = pthread_setname_np(thrds[n], thrd_name);
      if (0 != create)
        {
          printf(
            RED("\twarning: failed to set thread name; err: %d") "\n",
            errno);
        }
#endif /* if defined(_GNU_SOURCE) && !defined(_AIX) */
    }

  if (pass)
    {
      for (size_t j = 0; j < NUM_THREADS; j++)
        {
#ifndef _WIN32
          pthread_join(thrds[j], NULL);
#else  /* ifndef _WIN32 */
          WaitForSingleObject((HANDLE)thrds[j], INFINITE);
#endif /* ifndef _WIN32 */
        }
    }

  log_cleanup();
  return printerror(pass);
}

#ifndef _WIN32
static void *
sirtest_thread(void *arg)
{
#else  /* ifndef _WIN32 */
unsigned
sirtest_thread(void *arg)
{
#endif /* ifndef _WIN32 */
  pid_t threadid = _log_gettid();

  char mypath[LOG_MAXPATH + 16] = {
    0
  };
  (void)strncpy(mypath, (const char *)arg, LOG_MAXPATH - 1);
  free(arg);

  rmfile(mypath);
  sirfileid_t id = log_addfile(mypath, SIRL_ALL, SIRO_MSGONLY);

  if (NULL == id)
    {
      fprintf(stderr, "\t" RED("Failed to add file %s!") "\n", mypath);
#ifndef _WIN32
      return NULL;

#else  /* ifndef _WIN32 */
      return 0;

#endif /* ifndef _WIN32 */
    }

  printf("\thi, i'm thread #%d, log file: '%s'\n", threadid, mypath);

  for (size_t n = 0; n < 100; n++)
    {
      for (size_t i = 0; i < 10; i++)
        {
          log_debug(
            "thread %lu: hello, how do you do? %d",
            threadid,
            ( n * i ) + i);

          int r = getrand() % 15;

          if (r % 2 == 0)
            {
              if (!log_remfile(id))
                {
                  printerror(false);
                }

              id = log_addfile(mypath, SIRL_ALL, SIRO_MSGONLY);

              if (NULL == id)
                {
                  printerror(false);
                }

              if (!log_settextstyle(SIRL_DEBUG, SIRS_FG_RED | SIRS_BG_DEFAULT))
                {
                  printerror(false);
                }
            }
          else
            {
              if (!log_settextstyle(SIRL_DEBUG, SIRS_FG_CYAN | SIRS_BG_YELLOW))
                {
                  printerror(false);
                }
            }
        }
    }

  rmfile(mypath);

#ifndef _WIN32
  return NULL;

#else  /* ifndef _WIN32 */
  return 0;

#endif /* ifndef _WIN32 */
}

bool
printerror(bool pass)
{
  if (!pass)
    {
      sirchar_t message[LOG_MAXERROR] = {
        0
      };
      uint16_t code = log_geterror(message);
      printf("\t" RED("!! Unexpected (%hu, %s)") "\n", code, message);
    }

  return pass;
}

void
printexpectederr(void)
{
  sirchar_t message[LOG_MAXERROR] = {
    0
  };
  uint16_t code = log_geterror(message);

  printf("\t" GREEN("Expected (%hu, %s") "\n", code, message);
}

int
getoserr(void)
{
#ifndef _WIN32
  return errno;

#else  /* ifndef _WIN32 */
  return (int)GetLastError();

#endif /* ifndef _WIN32 */
}

unsigned int
getrand(void)
{
  static unsigned int seed = 0;

#ifndef _WIN32
  return (unsigned int)rand_r(&seed);

#else  /* ifndef _WIN32 */
  if (0 == rand_s(&seed))
    {
      return seed;
    }
  else
    {
      srand(seed);
      return (unsigned int)rand();
    }

#endif /* ifndef _WIN32 */
}

bool
rmfile(const char *filename)
{
#ifndef _WIN32
  return 0 == remove(filename);

#else  /* ifndef _WIN32 */
  return FALSE != DeleteFile(filename);

#endif /* ifndef _WIN32 */
}

bool
deletefiles(const char *search, const char *filename, unsigned *data)
{
  if (strstr(filename, search))
    {
      struct stat st;
      if (0 == stat(filename, &st))
        {
          printf("\tdeleting %s (size: %lu)...\n", filename, st.st_size);
        }

      if (!rmfile(filename))
        {
          fprintf(
            stderr,
            "\tfailed to delete %s! error: %d\n",
            filename,
            getoserr());
        }

      ( *data )++;
    }

  return true;
}

bool
countfiles(const char *search, const char *filename, unsigned *data)
{
  if (strstr(filename, search))
    {
      ( *data )++;
    }

  return true;
}

bool
enumfiles(const char *search, fileenumproc cb, unsigned *data)
{
#ifndef _WIN32
  DIR *d = opendir(".");
  if (!d)
    {
      return false;
    }

  rewinddir(d);
  struct dirent *di = readdir(d);
  if (!di)
    {
      return false;
    }

  while (NULL != di)
    {
      if (!cb(search, di->d_name, data))
        {
          break;
        }

      di = readdir(d);
    }
  ;

  closedir(d);
  d = NULL;
#else  /* ifndef _WIN32 */
  WIN32_FIND_DATA finddata = {
    0
  };
  HANDLE enumerator = FindFirstFile("./*", &finddata);

  if (INVALID_HANDLE_VALUE == enumerator)
    {
      return false;
    }

  do
    {
      if (!cb(search, finddata.cFileName, data))
        {
          break;
        }
    }
  while (FindNextFile(enumerator, &finddata) > 0);

  FindClose(enumerator);
  enumerator = NULL;
#endif /* ifndef _WIN32 */

  return true;
}

bool
startsirtimer(sirtimer_t *timer)
{
#ifndef _WIN32
  int gettime = clock_gettime(CLOCK_MONOTONIC, &timer->ts);
  return 0 == gettime;

#else  /* ifndef _WIN32 */
  GetSystemTimePreciseAsFileTime(&timer->ft);
  return true;

#endif /* ifndef _WIN32 */
}

float
sirtimerelapsed(const sirtimer_t *timer)
{
#ifndef _WIN32
  struct timespec now;
  if (!clock_gettime(CLOCK_MONOTONIC, &now))
    {
      return (float)(( now.tv_sec * 1e3 ) + ( now.tv_nsec / 1e6 )
                     - ( timer->ts.tv_sec * 1e3 ) + ( timer->ts.tv_nsec / 1e6 ));
    }

  return 0;

#else  /* ifndef _WIN32 */
  FILETIME now;
  GetSystemTimePreciseAsFileTime(&now);
  ULARGE_INTEGER start;
  start.LowPart    = timer->ft.dwLowDateTime;
  start.HighPart   = timer->ft.dwHighDateTime;
  ULARGE_INTEGER n100sec;
  n100sec.LowPart  = now.dwLowDateTime;
  n100sec.HighPart = now.dwHighDateTime;
  return ( n100sec.QuadPart - start.QuadPart ) / 1e4;

#endif /* ifndef _WIN32 */
}
