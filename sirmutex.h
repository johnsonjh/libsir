/*
 * scspell-id: 468f4e6a-2b0e-11ed-888e-80ee73e9b8e7
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

#ifndef _LOG_MUTEX_H_INCLUDED
# define _LOG_MUTEX_H_INCLUDED

# include "sirtypes.h"

/* Creates/initializes a new mutex. */

bool _logmutex_create(logmutex_t *mutex);

/* Determines if a mutex is locked without waiting. */

bool _logmutex_trylock(logmutex_t *mutex);

/* Attempts to lock a mutex and waits indefinitely. */

bool _logmutex_lock(logmutex_t *mutex);

/* Unlocks a previously locked mutex. */

bool _logmutex_unlock(logmutex_t *mutex);

/* Destroys a mutex. */

bool _logmutex_destroy(logmutex_t *mutex);

#endif /* !_LOG_MUTEX_H_INCLUDED */
