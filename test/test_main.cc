#include <gtest/gtest.h>

#include "env.hh"

/* Conbine to a single file for testing static function */
#include "test_common.cc"
#include "test_util.cc"
#include "test_array.cc"
#include "test_object.cc"
#include "test_lexer.cc"
#include "test_parser.cc"

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
