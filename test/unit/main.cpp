/** @file main.cpp
  @brief Runs nftsim's unit tests.

  Usage: unit_tests [substring]
  Runs every registered test (or only those whose name contains substring),
  prints one line per test, and exits with status 1 if any check failed.
*/

#include "unit_test.h"

#include <chrono>    // std::chrono::steady_clock;
#include <cstdio>    // std::remove;
#include <cstdlib>   // std::getenv; mkstemp; EXIT_SUCCESS; EXIT_FAILURE;
#include <cstring>   // std::strstr;
#include <fstream>   // std::ofstream;
#include <iostream>  // std::cout; std::cerr;
#include <stdexcept> // std::runtime_error;
#include <string>    // std::string;
#include <vector>    // std::vector;
#include <unistd.h>  // close;

namespace unit {

std::vector<TestCase>& registry() {
  static std::vector<TestCase> tests;
  return tests;
}

namespace {
int failures_in_test = 0;
}

void report_failure( const char* file, int line, const std::string& message ) {
  ++failures_in_test;
  std::cout << "\n    " << file << ":" << line << ": " << message;
}

TempConfig::TempConfig( const std::string& contents ) {
  const char* tmpdir = std::getenv("TMPDIR");
  std::string pattern = std::string( (tmpdir != nullptr && *tmpdir != '\0') ? tmpdir : "/tmp" )
                        + "/nftsim_unit_XXXXXX";
  std::vector<char> name(pattern.begin(), pattern.end());
  name.push_back('\0');
  int fd = mkstemp(name.data());
  if( fd == -1 ) {
    throw std::runtime_error("TempConfig: cannot create a file matching " + pattern);
  }
  close(fd);
  path_ = name.data();
  std::ofstream file(path_.c_str());
  file << contents;
  if( !file ) {
    throw std::runtime_error("TempConfig: cannot write " + path_);
  }
}

TempConfig::~TempConfig() {
  std::remove(path_.c_str());
}

} // namespace unit

int main( int argc, char* argv[] ) {
  const char* filter = argc > 1 ? argv[1] : nullptr;
  int run = 0;
  int failed = 0;
  const auto start = std::chrono::steady_clock::now();
  for( const unit::TestCase& test : unit::registry() ) {
    if( filter != nullptr && std::strstr(test.name, filter) == nullptr ) {
      continue;
    }
    ++run;
    unit::failures_in_test = 0;
    std::cout << test.name << " ..." << std::flush;
    try {
      test.function();
    } catch( const std::exception& err ) {
      unit::report_failure(__FILE__, __LINE__, std::string("uncaught exception: ") + err.what());
    }
    if( unit::failures_in_test == 0 ) {
      std::cout << " ok\n";
    } else {
      ++failed;
      std::cout << "\n  FAILED (" << unit::failures_in_test << " failed checks)\n";
    }
  }
  const double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
  std::cout << "\n" << run - failed << " of " << run << " tests passed (" << seconds << " s)\n";
  if( run == 0 ) {
    std::cout << "No tests matched '" << (filter != nullptr ? filter : "") << "'\n";
    return EXIT_FAILURE;
  }
  return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
