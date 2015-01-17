/**
 * @file
 * @brief tests for Themis
 *
 * (c) CossackLabs
 */

#include <stdio.h>
#include <string.h>
#include "themis_test.h"

int main(int argc, char* argv[])
{
  testsuite_start_testing();

  run_secure_message_test();
//  run_secure_session_test();

  testsuite_finish_testing();
  return testsuite_get_return_value();
}

