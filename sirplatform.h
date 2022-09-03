/*
 * scspell-id: 4c52662b-2b0e-11ed-a804-80ee73e9b8e7
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

#ifndef _LOG_PLATFORM_H_INCLUDED
# define _LOG_PLATFORM_H_INCLUDED

# if defined( __APPLE__ ) && defined( __MACH__ )
#  define __MACOS__
#  define _DARWIN_C_SOURCE
# elif defined( _WIN32 )
#  define _WIN32_WINNT 0x0600
# else /* if defined( __APPLE__ ) && defined( __MACH__ ) */
#  if defined( __GNUC__ ) && !defined( _GNU_SOURCE )
#   define _GNU_SOURCE
#  endif /* if defined( __GNUC__ ) && !defined( _GNU_SOURCE ) */
#  ifndef _POSIX_C_SOURCE
#   define _POSIX_C_SOURCE 200809L
#  endif /* ifndef _POSIX_C_SOURCE */
#  ifndef _DEFAULT_SOURCE
#   define _DEFAULT_SOURCE 1
#  endif /* ifndef _DEFAULT_SOURCE */
#  ifndef _ALL_SOURCE
#   define _ALL_SOURCE 1
#  endif /* ifndef _DEFAULT_SOURCE */
# endif /* if defined( __APPLE__ ) && defined( __MACH__ ) */

# include <assert.h>
# include <errno.h>
# include <stdarg.h>
# include <stdbool.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <time.h>

# ifndef _WIN32
#  include <pthread.h>
#  include <strings.h>
#  ifndef _AIX
#   include <sys/syscall.h>
#  endif
#  include <syslog.h>
#  include <unistd.h>

#  ifdef __linux__
#   include <linux/limits.h>
#  endif /* ifdef __linux__ */

#  ifdef PATH_MAX
#   define LOG_MAXPATH PATH_MAX
#  endif /* ifdef PATH_MAX */

#  if _POSIX_TIMERS > 0
#   define LOG_MSEC_TIMER
#   define LOG_MSEC_POSIX
#  else /* if _POSIX_TIMERS > 0 */
#   undef LOG_MSEC_TIMER
#  endif /* if _POSIX_TIMERS > 0 */

/* The mutex type. */

typedef pthread_mutex_t sirmutex_t;

/* The one-time type. */

typedef pthread_once_t sironce_t;

/* The one-time execution function type. */

typedef void (*log_once_fn) (void);

/* The one-time initializer. */

#  define LOG_ONCE_INIT PTHREAD_ONCE_INIT
# else /* ifndef _WIN32 */
#  define WIN32_LEAN_AND_MEAN

#  include <io.h>
#  include <synchapi.h>
#  include <windows.h>

#  define LOG_MAXPATH MAX_PATH
#  define LOG_NO_SYSLOG
#  define LOG_MSEC_TIMER
#  define LOG_MSEC_WIN32

/* The mutex type. */

typedef HANDLE sirmutex_t;

/* The one-time type. */

typedef INIT_ONCE sironce_t;

/* Process/thread ID. */

typedef int pid_t;

/* The one-time execution function type. */

typedef BOOL ( CALLBACK *log_once_fn ) (PINIT_ONCE, PVOID, PVOID *);

/* The one-time initializer. */

#  define LOG_ONCE_INIT INIT_ONCE_STATIC_INIT
# endif /* ifndef _WIN32 */

# ifndef thread_local
#  if ( __STDC_VERSION__ >= 201112 && !defined( __STDC_NO_THREADS__ ) ) || \
     ( defined(__SUNPRO_C) || defined(__SUNPRO_CC) || defined(__SUNPRO_C_COMPAT) )
#   define thread_local _Thread_local
#  elif defined( _WIN32 )
#   define thread_local __declspec( thread )
#  elif defined( __GNUC__ ) || defined(__xlc__) || defined(__xlC__)
#   define thread_local __thread
#  else /* if __STDC_VERSION__ >= 201112 && !defined( __STDC_NO_THREADS__ ) */
#   error "Unable to configure thread_local!"
#  endif /* if __STDC_VERSION__ >= 201112 && !defined( __STDC_NO_THREADS__ ) */
# endif /* ifndef thread_local */

/*
 * A sensible (?) constraint for the limit of a file's path. Note that this
 * value is only used in the absence of PATH_MAX (or MAX_PATH on windows).
 */

# ifndef LOG_MAXPATH
#  define LOG_MAXPATH 65535
# endif /* ifndef LOG_MAXPATH */

#endif /* !_LOG_PLATFORM_H_INCLUDED */
