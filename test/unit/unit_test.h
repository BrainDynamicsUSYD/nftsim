/** @file unit_test.h
  @brief A minimal, dependency-free unit-test harness for nftsim.

  Tests are plain functions declared with the TEST macro, which registers them
  to be run by test/unit/main.cpp. Inside a test, the CHECK macros record a
  failure (with file and line) and carry on, so one run reports every failed
  check. Build and run the tests with `make test-unit`.

      TEST(sigmoid_is_half_qmax_at_threshold) {
        CHECK(1 + 1 == 2);
        CHECK_CLOSE(computed, expected, 1e-12);
      }
*/

#ifndef NFTSIM_TEST_UNIT_UNIT_TEST_H
#define NFTSIM_TEST_UNIT_UNIT_TEST_H

#include <cmath>    // std::fabs; std::isnan;
#include <iostream> // std::cerr; std::streambuf;
#include <sstream>  // std::ostringstream;
#include <string>   // std::string;
#include <vector>   // std::vector;

namespace unit {

struct TestCase {
  const char* name;
  void (*function)();
};

/// All registered tests, in registration order.
std::vector<TestCase>& registry();

/// Records a failed check in the test that is running.
void report_failure( const char* file, int line, const std::string& message );

struct Registrar {
  Registrar( const char* name, void (*function)() ) {
    registry().push_back( TestCase{name, function} );
  }
};

/// Writes a configuration file to a unique temporary path, and deletes it
/// when it goes out of scope. nftsim's classes read their parameters through
/// Configf, which only reads files.
class TempConfig {
  std::string path_;
 public:
  explicit TempConfig( const std::string& contents );
  ~TempConfig();
  TempConfig(const TempConfig&) = delete;
  TempConfig& operator=(const TempConfig&) = delete;
  const std::string& path() const { return path_; }
};

/// Discards everything written to std::cerr while it is in scope (eg the
/// warnings nftsim prints for intentionally odd test parameters).
class SilenceCerr {
  std::ostringstream sink_;
  std::streambuf* saved_;
 public:
  SilenceCerr() : saved_(std::cerr.rdbuf(sink_.rdbuf())) {}
  ~SilenceCerr() { std::cerr.rdbuf(saved_); }
  SilenceCerr(const SilenceCerr&) = delete;
  SilenceCerr& operator=(const SilenceCerr&) = delete;
};

} // namespace unit

/// Defines and registers a test function.
#define TEST(name) \
  static void name(); \
  static const unit::Registrar name##_registrar(#name, name); \
  static void name()

/// Fails the current test if cond is false.
#define CHECK(cond) \
  do { \
    if( !(cond) ) { \
      unit::report_failure(__FILE__, __LINE__, "CHECK(" #cond ") failed"); \
    } \
  } while(0)

/// Like CHECK, with an explanatory message built with operator<<.
#define CHECK_MSG(cond, msg) \
  do { \
    if( !(cond) ) { \
      std::ostringstream unit_oss_; \
      unit_oss_ << "CHECK(" #cond ") failed: " << msg; \
      unit::report_failure(__FILE__, __LINE__, unit_oss_.str()); \
    } \
  } while(0)

/// Fails the current test unless a == b; prints both values.
#define CHECK_EQ(a, b) \
  do { \
    const auto unit_a_ = (a); \
    const auto unit_b_ = (b); \
    if( !(unit_a_ == unit_b_) ) { \
      std::ostringstream unit_oss_; \
      unit_oss_ << "CHECK_EQ(" #a ", " #b ") failed: " << unit_a_ << " != " << unit_b_; \
      unit::report_failure(__FILE__, __LINE__, unit_oss_.str()); \
    } \
  } while(0)

/// Fails the current test unless |a - b| <= tol (NaN always fails).
#define CHECK_CLOSE(a, b, tol) \
  do { \
    const double unit_a_ = (a); \
    const double unit_b_ = (b); \
    const double unit_t_ = (tol); \
    if( !(std::fabs(unit_a_ - unit_b_) <= unit_t_) ) { \
      std::ostringstream unit_oss_; \
      unit_oss_.precision(17); \
      unit_oss_ << "CHECK_CLOSE(" #a ", " #b ", " #tol ") failed: " << unit_a_ \
                << " vs " << unit_b_ << ", difference " << std::fabs(unit_a_ - unit_b_) \
                << " > " << unit_t_; \
      unit::report_failure(__FILE__, __LINE__, unit_oss_.str()); \
    } \
  } while(0)

#endif // NFTSIM_TEST_UNIT_UNIT_TEST_H
