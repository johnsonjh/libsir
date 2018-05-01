/*!
 * \file sirconsole.h
 * \brief Internal definitions for console color management in the SIR library.
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 */
#ifndef _SIR_CONSOLE_H_INCLUDED
#define _SIR_CONSOLE_H_INCLUDED

#include "sirplatform.h"
#include "sirtypes.h"

/*! \cond PRIVATE */

static const sir_style_priv_map sir_priv_map[] = {

#ifndef _WIN32
    {SIRS_NONE, 0},
    {SIRS_BRIGHT, 1},
    /* foreground */
    {SIRS_FG_BLACK, 30},    
    {SIRS_FG_RED, 31},
    {SIRS_FG_GREEN, 32},
    {SIRS_FG_YELLOW, 33},
    {SIRS_FG_BLUE, 34},
    {SIRS_FG_MAGENTA, 35},
    {SIRS_FG_CYAN, 36},
    {SIRS_FG_LGRAY, 37},
    {SIRS_FG_DGRAY, 90},
    {SIRS_FG_LRED, 91},
    {SIRS_FG_LGREEN, 92},
    {SIRS_FG_LYELLOW, 93},
    {SIRS_FG_LBLUE, 94},
    {SIRS_FG_LMAGENTA, 95},
    {SIRS_FG_LCYAN, 96},
    {SIRS_FG_WHITE, 97},    
    {SIRS_FG_DEFAULT, 39},
    /* background */
    {SIRS_BG_BLACK, 40},    
    {SIRS_BG_RED, 41},
    {SIRS_BG_GREEN, 42},
    {SIRS_BG_YELLOW, 43},    
    {SIRS_BG_BLUE, 44},
    {SIRS_BG_MAGENTA, 45},
    {SIRS_BG_CYAN, 46},
    {SIRS_BG_LGRAY, 47},
    {SIRS_BG_DGRAY, 100},
    {SIRS_BG_LRED, 101},
    {SIRS_BG_LGREEN, 102},
    {SIRS_BG_LYELLOW, 103},
    {SIRS_BG_LBLUE, 104},
    {SIRS_BG_LMAGENTA, 105},
    {SIRS_BG_LCYAN, 106},    
    {SIRS_BG_WHITE, 107},
    {SIRS_BG_DEFAULT, 49}    
#else
    {SIRS_NONE, 0},
    {SIRS_BRIGHT, FOREGROUND_INTENSITY},

    {SIRS_FG_BLACK, 0},    
    {SIRS_FG_RED, FOREGROUND_RED},
    {SIRS_FG_GREEN, FOREGROUND_GREEN},
    {SIRS_FG_YELLOW, FOREGROUND_RED | FOREGROUND_GREEN},
    {SIRS_FG_BLUE, FOREGROUND_BLUE},
    {SIRS_FG_MAGENTA, FOREGROUND_RED | FOREGROUND_BLUE},
    {SIRS_FG_CYAN, FOREGROUND_GREEN | FOREGROUND_BLUE},
    {SIRS_FG_LGRAY, FOREGROUND_INTENSITY},
    {SIRS_FG_DGRAY, FOREGROUND_INTENSITY},
    {SIRS_FG_LRED, FOREGROUND_RED | FOREGROUND_INTENSITY},
    {SIRS_FG_LGREEN, FOREGROUND_GREEN | FOREGROUND_INTENSITY},
    {SIRS_FG_LYELLOW, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY},
    {SIRS_FG_LBLUE, FOREGROUND_BLUE | FOREGROUND_INTENSITY},
    {SIRS_FG_LMAGENTA, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY},
    {SIRS_FG_LCYAN, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY},    
    {SIRS_FG_WHITE, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE},
    {SIRS_FG_DEFAULT, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE},

    {SIRS_BG_BLACK, 0},
    {SIRS_BG_RED, BACKGROUND_RED},
    {SIRS_BG_GREEN, BACKGROUND_GREEN},
    {SIRS_BG_YELLOW, BACKGROUND_RED | BACKGROUND_GREEN},    
    {SIRS_BG_BLUE, BACKGROUND_BLUE},
    {SIRS_BG_MAGENTA, BACKGROUND_RED | BACKGROUND_BLUE},
    {SIRS_BG_CYAN, BACKGROUND_GREEN | BACKGROUND_BLUE},
    {SIRS_BG_LGRAY, BACKGROUND_INTENSITY},
    {SIRS_BG_DGRAY, BACKGROUND_INTENSITY},
    {SIRS_BG_LRED, BACKGROUND_RED | BACKGROUND_INTENSITY},
    {SIRS_BG_LGREEN, BACKGROUND_GREEN | BACKGROUND_INTENSITY},
    {SIRS_BG_LYELLOW, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY},
    {SIRS_BG_LBLUE, BACKGROUND_BLUE | BACKGROUND_INTENSITY},
    {SIRS_BG_LMAGENTA, BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY},
    {SIRS_BG_LCYAN, BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY},     
    {SIRS_BG_WHITE, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE},    
    {SIRS_BG_DEFAULT, 0}
#endif
};

#ifndef _WIN32
bool             _sir_stderr_write(const sirchar_t* message);
bool             _sir_stdout_write(const sirchar_t* message);
#else
bool             _sir_stderr_write(uint16_t style, const sirchar_t* message);
bool             _sir_stdout_write(uint16_t style, const sirchar_t* message);
#endif

sir_textstyle _sir_getdefstyle(sir_level level);
uint16_t _sir_getprivstyle(uint16_t cat);
bool _sir_formatstyle(sir_textstyle style, sirchar_t* buf, size_t size);

/*! \endcond PRIVATE */

#endif /* !_SIR_CONSOLE_H_INCLUDED */
