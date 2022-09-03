/*
 * scspell-id: d9b3c85e-2b0e-11ed-a0dd-80ee73e9b8e7
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

#include "../sir.h"

/*
 * This is a basic example of initializing, configuring destinations,
 * and outputting messages.
 *
 * When it is finished running, you can see the output in the console, and
 * example the contents of 'log-example.log' in the current directory.
 */

int report_error(void);

int
main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  /*
   * Instantiate the initialization structure.
   *
   * Note: It is not necessary to retain this structure in memory;
   * libsir makes a copy of it before returning from log_init.
   */

  loginit si = {
    0
  };

  /*
   * Configure levels for stdout.
   * Send debug, information, warning, and notice messages there.
   */

  si.d_stdout.levels = SIRL_DEBUG | SIRL_INFO | SIRL_WARN | SIRL_NOTICE;

  /*
   * Configure options for stdout.
   * Don't show the time stamp or process ID.
   */

  si.d_stdout.opts = SIRO_NOTIME | SIRO_NOPID;

  /*
   * Configure levels for stderr.
   * Send error and above there.
   */

  si.d_stderr.levels = SIRL_ERROR | SIRL_CRIT | SIRL_ALERT | SIRL_EMERG;

  /*
   * Configure options for stderr.
   * Don't show the time stamp or process ID.
   */

  si.d_stderr.opts = SIRO_NOTIME | SIRO_NOPID;

  /*
   * Configure options for syslog.
   * Don't send any output there.
   */

  si.d_syslog.levels = 0;

  /* Configure a name to associate with our output. */

  strcpy(si.processName, "example");

  /* Initialize SIR. */

  if (!log_init(&si))
    {
      return report_error();
    }

  /*
   * Configure and add a log file.
   * Don't show the process name.
   * Send all severity levels there.
   */

  logfileid_t fileid1 = log_addfile("log-example.log", SIRL_ALL, SIRO_NONAME);

  if (NULL == fileid1)
    {
      return report_error();
    }

  /* Now we're ready to start generating output. */

  int n = 12345;
  const char *somestr = "my string contents";
  float f = 0.0009f;

  /*
   * This will be sent to all destinations registered for SIRL_DEBUG.
   * Notice that it is not necessary to add a newline at the end. libsir does
   * this automatically.
   */

  log_debug("debug-level message: {n=%d, somestr='%s', f=%.04f}", n, somestr, f);

  /* Do the same for the rest of available severity levels. */

  log_info("info-level message: {n=%d, somestr='%s', f=%.04f}", n, somestr, f);
  log_notice("notice-level message: {n=%d, somestr='%s', f=%.04f}", n, somestr, f);
  log_warn("warning-level message: {n=%d, somestr='%s', f=%.04f}", n, somestr, f);
  log_error("error-level message: {n=%d, somestr='%s', f=%.04f}", n, somestr, f);
  log_crit("critical error-level message: {n=%d, somestr='%s', f=%.04f}", n, somestr, f);
  log_alert("alert-level message: {n=%d, somestr='%s', f=%.04f}", n, somestr, f);
  log_emerg("emergency-level message: {n=%d, somestr='%s', f=%.04f}", n, somestr, f);

  /* Clean up. */

  log_cleanup();

  return 0;
}

int
report_error(void)
{
  logchar_t message[LOG_MAXERROR] = {
    0
  };
  uint16_t code = log_geterror(message);

  fprintf(stderr, "libsir error: (%hu, %s)\n", code, message);
  return 1;
}
