/*
 * SPDX-License-Identifier: MIT
 * scspell-id: f1549b01-2b0d-11ed-b2ff-80ee73e9b8e7
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

#ifndef _LOG_CONSOLE_H_INCLUDED
# define _LOG_CONSOLE_H_INCLUDED

# include "sirtypes.h"

# ifndef _WIN32
bool _log_stderr_write(const logchar_t *message);
bool _log_stdout_write(const logchar_t *message);
# else  /* ifndef _WIN32 */
bool _log_stderr_write(uint16_t style, const logchar_t *message);
bool _log_stdout_write(uint16_t style, const logchar_t *message);
# endif /* ifndef _WIN32 */

#endif /* !_LOG_CONSOLE_H_INCLUDED */
