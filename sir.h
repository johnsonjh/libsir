/*
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

#ifndef _SIR_H_INCLUDED
# define _SIR_H_INCLUDED

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
 * Call sir_cleanup when you're through.
 *
 * si  = Initialization options.
 *
 * retval true  = libsir is initialized and ready to use.
 * retval false = Initialization failed.
 */

bool sir_init(sirinit *si);

/*
 * Sets levels sent to stdout.
 *
 * Sets the sir_level registration mask for this destination.
 *
 * Value          | Behavior
 * -----          | --------
 * `SIRL_NONE`    | No levels.
 * `SIRL_ALL`     | All levels.
 * `SIRL_DEFAULT` | Revert to the default (@ref sirdefaults.h).
 * `SIRL_*`       | Register for each level set.
 *
 * retval true  = Levels were updated successfully.
 * retval false = An error occurred while trying to update levels.
 */

bool sir_stdoutlevels(sir_levels levels);

/*
 * Sets formatting options for stdout.
 *
 * Sets the sir_option mask that controls the content of messages
 * sent to this destination.
 *
 * Value          | Behavior
 * -----          | --------
 * `0`            | Include all available data.
 * `SIRO_DEFAULT` | Revert to the default (@ref sirdefaults.h).
 * `SIRO_*`       | Apply each option set.
 *
 * retval true  = Options were updated successfully.
 * retval false = An error occurred while trying to update options.
 */

bool sir_stdoutopts(sir_options opts);

/*
 * Sets levels sent to stderr.
 *
 * Sets the sir_level registration mask for this destination.
 *
 * Value          | Behavior
 * -----          | --------
 * `SIRL_NONE`    | No levels.
 * `SIRL_ALL`     | All levels.
 * `SIRL_DEFAULT` | Revert to the default (@ref sirdefaults.h).
 * `SIRL_*`       | Register for each level set.
 *
 * @retval true  = Levels were updated successfully.
 * @retval false = An error occurred while trying to update levels.
 */

bool sir_stderrlevels(sir_levels levels);

/*
 * Sets formatting options for \a stderr.
 *
 * Sets the sir_option mask that controls the content of messages
 * sent to this destination.
 *
 * Value          | Behavior
 * -----          | --------
 * `0`            | Include all available data.
 * `SIRO_DEFAULT` | Revert to the default (@ref sirdefaults.h).
 * `SIRO_*`       | Apply each option set.
 *
 * retval true  = Options were updated successfully.
 * retval false = An error occurred while trying to update options.
 */

bool sir_stderropts(sir_options opts);

/*
 * Sets levels sent to syslog (if available).
 *
 * Sets the sir_level registration mask for this destination.
 *
 * If `SIR_NOSYSLOG` is defined upon compilation, returns
 * `false` and has no effect.
 *
 * Value          | Behavior
 * -----          | --------
 * `SIRL_NONE`    | No levels.
 * `SIRL_ALL`     | All levels.
 * `SIRL_DEFAULT` | Revert to the default (@ref sirdefaults.h).
 * `SIRL_*`       | Register for each level set.
 *
 * retval true  = Levels were updated successfully.
 * retval false = An error occurred while trying to update levels.
 */

bool sir_sysloglevels(sir_levels levels);

/*
 * Sets levels sent to a log file.
 *
 * Sets the sir_level registration mask for this destination.
 *
 * Value          | Behavior
 * -----          | --------
 * `SIRL_NONE`    | No levels.
 * `SIRL_ALL`     | All levels.
 * `SIRL_DEFAULT` | Revert to the default (@ref sirdefaults.h).
 * `SIRL_*`       | Register for each level set.
 *
 * @retval true Levels were updated successfully.
 * @retval false An error occurred while trying to update levels.
 */

bool sir_filelevels(sirfileid_t id, sir_levels levels);

/*
 * Sets formatting options for a log file.
 *
 * Sets the sir_option mask that controls the content of messages
 * sent to this destination.
 *
 * Value          | Behavior
 * -----          | --------
 * `0`            | Include all available data.
 * `SIRO_DEFAULT` | Revert to the default (@ref sirdefaults.h).
 * `SIRO_*`       | Apply each option set.
 *
 * retval true  = Options were updated successfully.
 * retval false = An error occurred while trying to update options.
 */

bool sir_fileopts(sirfileid_t id, sir_options opts);

/*
 * Frees allocated resources and resets internal state.
 *
 * Call sir_init to initialize libsir.
 *
 * It is not necessary to call this function from the same thread that
 * called sir_init. Any calls made after this to functions other than
 * to sir_init (in order to re-initialize) will fail.
 *
 * retval true  = libsir is cleaned up.
 * retval false = An error occurred.
 */

bool sir_cleanup(void);

/*
 * Retrieves information about the last error that occurred within
 * the context of libsir or lower.
 *
 * Most C library and OS calls made by libsir are evaluated for failure.
 * If a failure of this type is encountered, this function returns
 * SIR_E_PLATFORM, and message will contain a string identifying the
 * underlying error code and the message as reported by the platform.
 *
 * retval SIR_E_NOERROR   = The operation completed successfully
 * retval SIR_E_NOTREADY  = libsir has not been initialized
 * retval SIR_E_ALREADY   = libsir is already initialized
 * retval SIR_E_DUPFILE   = File already managed by SIR
 * retval SIR_E_NOFILE    = File not managed by SIR
 * retval SIR_E_FCFULL    = Maximum number of files already managed
 * retval SIR_E_OPTIONS   = Option flags are invalid
 * retval SIR_E_LEVELS    = Level flags are invalid
 * retval SIR_E_TEXTSTYLE = Text style is invalid
 * retval SIR_E_STRING    = Invalid string argument
 * retval SIR_E_NODEST    = No destinations registered for level
 * retval SIR_E_PLATFORM  = Platform error code %d: %s%
 * retval SIR_E_UNKNOWN   = Error is not known
 */

uint16_t sir_geterror(sirchar_t message[SIR_MAXERROR - 1]);

/*
 * Log a formatted debug-level message.
 *
 * This function maps internally to the logging level SIRL_DEBUG.
 *
 * retval true  = All destinations for this level were successfully processed.
 * retval false = One or more destinations were not successfully processed.
 */

bool sir_debug(const sirchar_t *format, ...);

/*
 * Log a formatted informational message.
 *
 * This function maps internally to the logging level SIRL_INFO.
 *
 * retval true  = All destinations for this level were successfully processed.
 * retval false = One or more destinations were not successfully processed.
 */

bool sir_info(const sirchar_t *format, ...);

/**
 * @brief Log a formatted notice message.
 *
 * This function maps internally to the logging level ::SIRL_NOTICE.
 *
 * @param format A printf-style format string.
 * @param ... \a (variadic) Additional arguments whose types correspond to
 * the format specifier at the same index in \p format.
 *
 * @return boolean
 * @retval true All destinations registered for this level were successfully
 * processed.
 * @retval false One or more destinations were not successfully processed.
 */

bool sir_notice(const sirchar_t *format, ...);

/**
 * @brief Log a formatted warning message.
 *
 * This function maps internally to the logging level ::SIRL_WARN.
 *
 * @param format A printf-style format string.
 * @param ... \a (variadic) Additional arguments whose types correspond to
 * the format specifier at the same index in \p format.
 *
 * @return boolean
 * @retval true All destinations registered for this level were successfully
 * processed.
 * @retval false One or more destinations were not successfully processed.
 */

bool sir_warn(const sirchar_t *format, ...);

/**
 * @brief Log a formatted error message.
 *
 * This function maps internally to the logging level ::SIRL_ERROR.
 *
 * @param format A printf-style format string.
 * @param ... \a (variadic) Additional arguments whose types correspond to
 * the format specifier at the same index in \p format.
 *
 * @return boolean
 * @retval true All destinations registered for this level were successfully
 * processed.
 * @retval false One or more destinations were not successfully processed.
 */

bool sir_error(const sirchar_t *format, ...);

/**
 * @brief Log a formatted critical error message.
 *
 * This function maps internally to the logging level ::SIRL_CRIT.
 *
 * @param format A printf-style format string.
 * @param ... \a (variadic) Additional arguments whose types correspond to
 * the format specifier at the same index in \p format.
 *
 * @return boolean
 * @retval true All destinations registered for this level were successfully
 * processed.
 * @retval false One or more destinations were not successfully processed.
 */

bool sir_crit(const sirchar_t *format, ...);

/**
 * @brief Log a formatted alert message.
 *
 * This function maps internally to the logging level ::SIRL_ALERT.
 *
 * @param format A printf-style format string.
 * @param ... \a (variadic) Additional arguments whose types correspond to
 * the format specifier at the same index in \p format.
 *
 * @return boolean
 * @retval true All destinations registered for this level were successfully
 * processed.
 * @retval false One or more destinations were not successfully processed.
 */

bool sir_alert(const sirchar_t *format, ...);

/**
 * @brief Log a formatted emergency message.
 *
 * This function maps internally to the logging level ::SIRL_EMERG.
 *
 * @param format A printf-style format string.
 * @param ... \a (variadic) Additional arguments whose types correspond to
 * the format specifier at the same index in \p format.
 *
 * @return boolean
 * @retval true All destinations registered for this level were successfully
 * processed.
 * @retval false One or more destinations were not successfully processed.
 */

bool sir_emerg(const sirchar_t *format, ...);

/**
 * @brief Add a log file to receive formatted output for one or more
 * ::sir_level.
 *
 * @param path The absolute or relative path to the file. If it does not
 * exist, it will be created.
 * @param levels One or more ::sir_level for which the file should be sent
 * output. Use
 * ::SIRL_ALL to send all logging levels to the file.
 * @param opts Zero or more ::sir_option flags used to control output
 * formatting.
 *
 * @attention The sirfileid_t type is a pointer to the internally mapped
 * file descriptor for a file. If you modify the value at this address, the
 * behavior is undefined, and bad things will happen.
 *
 * @return sirfileid_t
 * @retval The file descriptor for the file that was successfully added.
 * @retval NULL An error occurred while trying to add the file.
 */

sirfileid_t sir_addfile(const sirchar_t *path, sir_levels levels,
                        sir_options opts);

/**
 * @brief Remove a previously added log file.
 *
 * @param id The file descriptor returned from ::sir_addfile.
 *
 * @return boolean
 * @retval true The file was removed and will no longer receive output.
 * @retval false An error occurred while trying to remove the file.
 */

bool sir_remfile(sirfileid_t id);

/**
 * @brief Sets the text style in \a stdio output for a ::sir_level of output.
 *
 * @param level The level \a (only one is permitted per call) for which the
 * style will be set.
 * @param style Foreground color, background color, and brightness control
 * flags bitwise OR'd to create the text style for \p level. See
 * ::sir_textstyle.
 *
 * @return boolean
 * @retval true The style is valid and was applied.
 * @retval false An error occurred while trying to set the text style.
 */

bool sir_settextstyle(sir_level level, sir_textstyle style);

/**
 * @brief Resets all \a stdio text styles to their default values
 * (::sir_default_styles).
 *
 * @return boolean
 * @retval true All text styles were reset to their defaults.
 * @retval false An error occurred while trying to reset text styles.
 */

bool sir_resettextstyles(void);

#endif /* !_SIR_H_INCLUDED */
