/*
 * SPDX-License-Identifier: MIT
 * scspell-id: c6e3a71f-2b0e-11ed-938c-80ee73e9b8e7
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

#ifndef _LOG_TEXTSTYLE_H_INCLUDED
# define _LOG_TEXTSTYLE_H_INCLUDED

# include "sirtypes.h"

/* Overrides for level <> text style mappings (log_settextstyle). */

static log_style_map log_override_styles[LOG_NUMLEVELS] = {
  { LOGL_DEBUG,  LOGS_INVALID },
  { LOGL_INFO,   LOGS_INVALID },
  { LOGL_NOTICE, LOGS_INVALID },
  { LOGL_WARN,   LOGS_INVALID },
  { LOGL_ERROR,  LOGS_INVALID },
  { LOGL_CRIT,   LOGS_INVALID },
  { LOGL_ALERT,  LOGS_INVALID },
  { LOGL_EMERG,  LOGS_INVALID },
};

/* Mapping of log_textstyle <> platform values. */

static const log_style_priv_map log_priv_map[] = {
# ifndef _WIN32
  { LOGS_NONE,                                                                0 },
  { LOGS_BRIGHT,                                                              1 },
  { LOGS_DIM,                                                                 2 },
  /* foreground */
  { LOGS_FG_BLACK,                                                           30 },
  { LOGS_FG_RED,                                                             31 },
  { LOGS_FG_GREEN,                                                           32 },
  { LOGS_FG_YELLOW,                                                          33 },
  { LOGS_FG_BLUE,                                                            34 },
  { LOGS_FG_MAGENTA,                                                         35 },
  { LOGS_FG_CYAN,                                                            36 },
  { LOGS_FG_LGRAY,                                                           37 },
  { LOGS_FG_DGRAY,                                                           90 },
  { LOGS_FG_LRED,                                                            91 },
  { LOGS_FG_LGREEN,                                                          92 },
  { LOGS_FG_LYELLOW,                                                         93 },
  { LOGS_FG_LBLUE,                                                           94 },
  { LOGS_FG_LMAGENTA,                                                        95 },
  { LOGS_FG_LCYAN,                                                           96 },
  { LOGS_FG_WHITE,                                                           97 },
  { LOGS_FG_DEFAULT,                                                         39 },
  /* background */
  { LOGS_BG_BLACK,                                                           40 },
  { LOGS_BG_RED,                                                             41 },
  { LOGS_BG_GREEN,                                                           42 },
  { LOGS_BG_YELLOW,                                                          43 },
  { LOGS_BG_BLUE,                                                            44 },
  { LOGS_BG_MAGENTA,                                                         45 },
  { LOGS_BG_CYAN,                                                            46 },
  { LOGS_BG_LGRAY,                                                           47 },
  { LOGS_BG_DGRAY,                                                          100 },
  { LOGS_BG_LRED,                                                           101 },
  { LOGS_BG_LGREEN,                                                         102 },
  { LOGS_BG_LYELLOW,                                                        103 },
  { LOGS_BG_LBLUE,                                                          104 },
  { LOGS_BG_LMAGENTA,                                                       105 },
  { LOGS_BG_LCYAN,                                                          106 },
  { LOGS_BG_WHITE,                                                          107 },
  { LOGS_BG_DEFAULT,                                                         49 },
# else  /* ifndef _WIN32 */
  { LOGS_NONE,                                                                0 },
  { LOGS_BRIGHT,      FOREGROUND_INTENSITY                                      },
  { LOGS_DIM,                                                                 0 },
  { LOGS_FG_BLACK,                                                            0 },
  { LOGS_FG_RED,      FOREGROUND_RED                                            },
  { LOGS_FG_GREEN,    FOREGROUND_GREEN                                          },
  { LOGS_FG_YELLOW,   FOREGROUND_RED | FOREGROUND_GREEN                         },
  { LOGS_FG_BLUE,     FOREGROUND_BLUE                                           },
  { LOGS_FG_MAGENTA,  FOREGROUND_RED | FOREGROUND_BLUE                          },
  { LOGS_FG_CYAN,     FOREGROUND_GREEN | FOREGROUND_BLUE                        },
  { LOGS_FG_LGRAY,    FOREGROUND_INTENSITY                                      },
  { LOGS_FG_DGRAY,    FOREGROUND_INTENSITY                                      },
  { LOGS_FG_LRED,     FOREGROUND_RED | FOREGROUND_INTENSITY                     },
  { LOGS_FG_LGREEN,   FOREGROUND_GREEN | FOREGROUND_INTENSITY                   },
  { LOGS_FG_LYELLOW,  FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY  },
  { LOGS_FG_LBLUE,    FOREGROUND_BLUE | FOREGROUND_INTENSITY                    },
  { LOGS_FG_LMAGENTA, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY   },
  { LOGS_FG_LCYAN,    FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY },
  { LOGS_FG_WHITE,    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE       },
  { LOGS_FG_DEFAULT,  FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE       },
  { LOGS_BG_BLACK,                                                            0 },
  { LOGS_BG_RED,      BACKGROUND_RED                                            },
  { LOGS_BG_GREEN,    BACKGROUND_GREEN                                          },
  { LOGS_BG_YELLOW,   BACKGROUND_RED | BACKGROUND_GREEN                         },
  { LOGS_BG_BLUE,     BACKGROUND_BLUE                                           },
  { LOGS_BG_MAGENTA,  BACKGROUND_RED | BACKGROUND_BLUE                          },
  { LOGS_BG_CYAN,     BACKGROUND_GREEN | BACKGROUND_BLUE                        },
  { LOGS_BG_LGRAY,    BACKGROUND_INTENSITY                                      },
  { LOGS_BG_DGRAY,    BACKGROUND_INTENSITY                                      },
  { LOGS_BG_LRED,     BACKGROUND_RED | BACKGROUND_INTENSITY                     },
  { LOGS_BG_LGREEN,   BACKGROUND_GREEN | BACKGROUND_INTENSITY                   },
  { LOGS_BG_LYELLOW,  BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY  },
  { LOGS_BG_LBLUE,    BACKGROUND_BLUE | BACKGROUND_INTENSITY                    },
  { LOGS_BG_LMAGENTA, BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY   },
  { LOGS_BG_LCYAN,    BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY },
  { LOGS_BG_WHITE,    BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE       },
  { LOGS_BG_DEFAULT,                                                          0 },
# endif /* ifndef _WIN32 */
};

/* Validates a log_textstyle and splits it into its component parts. */

bool _log_validstyle(log_textstyle style, uint32_t *pattr, uint32_t *pfg,
                     uint32_t *pbg);

/*
 * Retrieves the override log_textstyle for a log_level  if one is set.
 * Otherwise, returns the default text style for that level.
 */

log_textstyle _log_gettextstyle(log_level level);

/* Retrieves the default log_textstyle for a log_level. */

log_textstyle _log_getdefstyle(const log_style_map *map, log_level level);

/* Sets the log_textstyle for a log_level. */

bool _log_settextstyle(log_level level, log_textstyle style);

/* Resets all override log_textstyle. */

bool _log_resettextstyles(void);

/* Retrieves the platform value for a component part of a log_textstyle. */

uint16_t _log_getprivstyle(uint32_t cat);

/*
 * Combines component parts of a platform text style value into its final
 * form.
 */

bool _log_formatstyle(log_textstyle style, logchar_t *buf, size_t size);

#endif /* !_LOG_TEXTSTYLE_H_INCLUDED */
