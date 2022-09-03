/*
 * scspell-id: 1d4896a0-2b0e-11ed-9a9f-80ee73e9b8e7
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

#ifndef _LOG_H_INCLUDED
# define _LOG_H_INCLUDED

# include "sirplatform.h"
# include "sirtypes.h"

/*
 * Functions and types that comprise the public interface to libsir.
 */

/*
 * Initializes libsir.
 *
 * Any thread may initialize SIR, but any calls to other
 * libsir functions will fail until this call has completed.
 * Call log_cleanup when you're through.
 *
 * si  = Initialization options.
 *
 * retval true  = libsir is initialized and ready to use.
 * retval false = Initialization failed.
 */

bool log_init(loginit *si);

/*
 * Sets levels sent to stdout.
 *
 * Sets the log_level registration mask for this destination.
 *
 * Value          | Behavior
 * -----          | --------
 * `SIRL_NONE`    | No levels.
 * `SIRL_ALL`     | All levels.
 * `SIRL_DEFAULT` | Revert to the default.
 * `SIRL_*`       | Register for each level set.
 *
 * retval true  = Levels were updated successfully.
 * retval false = An error occurred while trying to update levels.
 */

bool log_stdoutlevels(log_levels levels);

/*
 * Sets formatting options for stdout.
 *
 * Sets the log_option mask that controls the content of messages
 * sent to this destination.
 *
 * Value          | Behavior
 * -----          | --------
 * `0`            | Include all available data.
 * `SIRO_DEFAULT` | Revert to the default.
 * `SIRO_*`       | Apply each option set.
 *
 * retval true  = Options were updated successfully.
 * retval false = An error occurred while trying to update options.
 */

bool log_stdoutopts(log_options opts);

/*
 * Sets levels sent to stderr.
 *
 * Sets the log_level registration mask for this destination.
 *
 * Value          | Behavior
 * -----          | --------
 * `SIRL_NONE`    | No levels.
 * `SIRL_ALL`     | All levels.
 * `SIRL_DEFAULT` | Revert to the default.
 * `SIRL_*`       | Register for each level set.
 *
 * retval true  = Levels were updated successfully.
 * retval false = An error occurred while trying to update levels.
 */

bool log_stderrlevels(log_levels levels);

/*
 * Sets formatting options for stderr.
 *
 * Sets the log_option mask that controls the content of messages
 * sent to this destination.
 *
 * Value          | Behavior
 * -----          | --------
 * `0`            | Include all available data.
 * `SIRO_DEFAULT` | Revert to the default.
 * `SIRO_*`       | Apply each option set.
 *
 * retval true  = Options were updated successfully.
 * retval false = An error occurred while trying to update options.
 */

bool log_stderropts(log_options opts);

/*
 * Sets levels sent to syslog (if available).
 *
 * Sets the log_level registration mask for this destination.
 *
 * If `LOG_NOSYSLOG` is defined upon compilation, returns
 * `false` and has no effect.
 *
 * Value          | Behavior
 * -----          | --------
 * `SIRL_NONE`    | No levels.
 * `SIRL_ALL`     | All levels.
 * `SIRL_DEFAULT` | Revert to the default.
 * `SIRL_*`       | Register for each level set.
 *
 * retval true  = Levels were updated successfully.
 * retval false = An error occurred while trying to update levels.
 */

bool log_sysloglevels(log_levels levels);

/*
 * Sets levels sent to a log file.
 *
 * Sets the log_level registration mask for this destination.
 *
 * Value          | Behavior
 * -----          | --------
 * `SIRL_NONE`    | No levels.
 * `SIRL_ALL`     | All levels.
 * `SIRL_DEFAULT` | Revert to the default.
 * `SIRL_*`       | Register for each level set.
 *
 * retval true Levels were updated successfully.
 * retval false An error occurred while trying to update levels.
 */

bool log_filelevels(logfileid_t id, log_levels levels);

/*
 * Sets formatting options for a log file.
 *
 * Sets the log_option mask that controls the content of messages
 * sent to this destination.
 *
 * Value          | Behavior
 * -----          | --------
 * `0`            | Include all available data.
 * `SIRO_DEFAULT` | Revert to the default.
 * `SIRO_*`       | Apply each option set.
 *
 * retval true  = Options were updated successfully.
 * retval false = An error occurred while trying to update options.
 */

bool log_fileopts(logfileid_t id, log_options opts);

/*
 * Frees allocated resources and resets internal state.
 *
 * Call log_init to initialize libsir.
 *
 * It is not necessary to call this function from the same thread that
 * called log_init. Any calls made after this to functions other than
 * to log_init (in order to re-initialize) will fail.
 *
 * retval true  = libsir is cleaned up.
 * retval false = An error occurred.
 */

bool log_cleanup(void);

/*
 * Retrieves information about the last error that occurred within
 * the context of libsir or lower.
 *
 * Most C library and OS calls made by libsir are evaluated for failure.
 * If a failure of this type is encountered, this function returns
 * LOG_E_PLATFORM, and message will contain a string identifying the
 * underlying error code and the message as reported by the platform.
 *
 * retval LOG_E_NOERROR   = The operation completed successfully
 * retval LOG_E_NOTREADY  = libsir has not been initialized
 * retval LOG_E_ALREADY   = libsir is already initialized
 * retval LOG_E_DUPFILE   = File already managed by SIR
 * retval LOG_E_NOFILE    = File not managed by SIR
 * retval LOG_E_FCFULL    = Maximum number of files already managed
 * retval LOG_E_OPTIONS   = Option flags are invalid
 * retval LOG_E_LEVELS    = Level flags are invalid
 * retval LOG_E_TEXTSTYLE = Text style is invalid
 * retval LOG_E_STRING    = Invalid string argument
 * retval LOG_E_NODEST    = No destinations registered for level
 * retval LOG_E_PLATFORM  = Platform error code %d: %s%
 * retval LOG_E_UNKNOWN   = Error is not known
 */

uint16_t log_geterror(logchar_t message[LOG_MAXERROR - 1]);

/*
 * Log a formatted debug-level message.
 */

bool log_debug(const logchar_t *format, ...);

/*
 * Log a formatted informational message.
 */

bool log_info(const logchar_t *format, ...);

/*
 * Log a formatted notice message.
 */

bool log_notice(const logchar_t *format, ...);

/*
 * Log a formatted warning message.
 */

bool log_warn(const logchar_t *format, ...);

/*
 * Log a formatted error message.
 */

bool log_error(const logchar_t *format, ...);

/*
 * Log a formatted critical error message.
 */

bool log_crit(const logchar_t *format, ...);

/*
 * Log a formatted alert message.
 */

bool log_alert(const logchar_t *format, ...);

/*
 * Log a formatted emergency message.
 */

bool log_emerg(const logchar_t *format, ...);

/*
 * Add a log file to receive formatted output for one or more log_level.
 */

logfileid_t log_addfile(const logchar_t *path, log_levels levels,
                        log_options opts);

/*
 * Remove a previously added log file.
 */

bool log_remfile(logfileid_t id);

/*
 * Sets the text style in stdio output for a log_level of output.
 */

bool log_settextstyle(log_level level, log_textstyle style);

/*
 * Resets all stdio text styles to their default values (log_default_styles).
 */

bool log_resettextstyles(void);

#endif /* !_LOG_H_INCLUDED */
