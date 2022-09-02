/*
 * scspell-id: e62b0803-2b0e-11ed-b8c6-80ee73e9b8e7
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

/*
 * Supported systems:
 * - Ubuntu 16.04   x64  (GCC)
 * - Windows 10     x64  (MinGW)
 * - macOS 10.13.4  x64  (Clang)
 * - Other platforms are untested.
 */

#ifndef _SIR_TESTS_H_INCLUDED
# define _SIR_TESTS_H_INCLUDED

# ifndef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE 200809L
# endif /* ifndef _POSIX_C_SOURCE */
# ifndef _DEFAULT_SOURCE
#  define _DEFAULT_SOURCE 1
# endif /* ifndef _DEFAULT_SOURCE */
# ifndef _ALLA_SOURCE
#  define _ALL_SOURCE 1
# endif /* ifndef _ALL_SOURCE */

# define _CRT_RAND_S

# include "../sir.h"
# include "../sirerrors.h"
# include "../sirfilecache.h"
# include "../sirinternal.h"
# include "tests.h"

# include <errno.h>
# include <fcntl.h>
# include <stdbool.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>

# ifndef _WIN32
#  include <dirent.h>
#  include <pthread.h>
#  include <sys/stat.h>
#  include <unistd.h>
# else  /* ifndef _WIN32 */
#  define _WIN32_WINNT 0x0600
#  include <process.h>
#  include <windows.h>
# endif /* ifndef _WIN32 */

# ifndef _WIN32
#  define STRFMT(clr, s) clr s "\033[0m"
#  define RED(s)        STRFMT("\033[1;91m", s)
#  define GREEN(s)      STRFMT("\033[1;92m", s)
#  define WHITE(s)      STRFMT("\033[1;97m", s)
#  define BLUE(s)       STRFMT("\033[1;34m", s)
# else  /* ifndef _WIN32 */
#  define RED(s)   s
#  define GREEN(s) s
#  define WHITE(s) s
#  define BLUE(s)  s
# endif /* ifndef _WIN32 */

# define INIT_N(var, l_stdout, o_stdout, l_stderr, o_stderr, name)  \
  sirinit var         = { 0 };                                      \
  var.d_stdout.opts   = o_stdout;                                   \
  var.d_stdout.levels = l_stdout;                                   \
  var.d_stderr.opts   = o_stderr;                                   \
  var.d_stderr.levels = l_stderr;                                   \
  if (strlen(name) > 0)                                             \
  (void)strncpy(var.processName, name, SIR_MAXNAME);                \
  bool var ## _init = sir_init(&var);

# define INIT(var, l_stdout, o_stdout, l_stderr, o_stderr) \
  INIT_N(var, l_stdout, o_stdout, l_stderr, o_stderr, "")

/* Function signature for a single test. */
typedef bool (*sir_test_fn) (void);

/* Map a test to a human-readable description. */
typedef struct
{
  const char *name;
  sir_test_fn fn;
} sir_test;

/*
 * Properly handle multiple threads competing for locked sections.
 */

bool sirtest_mthread_race(void);

/*
 * Properly handle messages that exceed internal buffer sizes.
 */

bool sirtest_exceedmaxsize(void);

/*
 * Properly handle adding and removing log files.
 */

bool sirtest_filecachesanity(void);

/*
 * Properly handle invalid text style.
 */

bool sirtest_failsetinvalidstyle(void);

/*
 * Properly handle the lack of any output destinations.
 */

bool sirtest_failnooutputdest(void);

/*
 * Properly handle invalid log file name.
 */

bool sirtest_failinvalidfilename(void);

/*
 * Properly handle log file without appropriate permissions.
 */

bool sirtest_failfilebadpermission(void);

/*
 * Properly handle null/empty input.
 */

bool sirtest_failnulls(void);

/*
 * Properly handle calls without initialization.
 */

bool sirtest_failwithoutinit(void);

/*
 * Properly handle two initialization calls without corresponding cleanup.
 */

bool sirtest_failinittwice(void);

/**
 * @test Properly handle calls after cleanup.
 */
bool sirtest_failaftercleanup(void);

/*
 * Properly handle initialization, cleanup, re-initialization.
 */

bool sirtest_initcleanupinit(void);

/*
 * Properly refuse to add a duplicate file.
 */

bool sirtest_faildupefile(void);

/*
 * Properly refuse to remove a file that isn't added.
 */

bool sirtest_failremovebadfile(void);

/*
 * Properly roll/archive a file when it hits max size.
 */

bool sirtest_rollandarchivefile(void);

/*
 * Properly return valid codes and messages for all possible errors.
 */

bool sirtest_errorsanity(void);

/*
 * Properly style stdio output for each level, and handle style overrides.
 */

bool sirtest_textstylesanity(void);

/*
 * Performance evaluation.
 */

bool sirtest_perf(void);

/*
 * Properly update levels/options at runtime.
 */

bool sirtest_updatesanity(void);

/*
 * bool sirtest_xxxx(void);
 */

bool printerror(bool pass);
void printexpectederr(void);

int getoserr(void);
unsigned int getrand(void);

bool rmfile(const char *filename);
bool deletefiles(const char *search, const char *filename, unsigned *data);
bool countfiles(const char *search, const char *filename, unsigned *data);

typedef bool (*fileenumproc) (const char *search, const char *filename,
                              unsigned *data);

bool enumfiles(const char *search, fileenumproc cb, unsigned *data);

typedef struct
{
# ifndef _WIN32
  struct timespec ts;
# else  /* ifndef _WIN32 */
  FILETIME ft;
# endif /* ifndef _WIN32 */
} sirtimer_t;

bool startsirtimer(sirtimer_t *timer);
float sirtimerelapsed(const sirtimer_t *timer);  // msec

#endif /* !_SIR_TESTS_H_INCLUDED */
