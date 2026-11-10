/** @file test_configf.cpp
  @brief Configf::indices() and parseIndex(), which read lists of indices
         such as the node numbers after `Node:`.

  indices() exits on an invalid index, which this harness cannot test, so the
  rejection of negative and non-whole numbers is tested through parseIndex(),
  which indices() uses to read each token.
*/

#include "unit_test.h"

#include "configf.h"

#include <string>
#include <vector>

namespace {

using size_type = Configf::size_type;

/// parseIndex(token), and the index it reads (unchanged, 99, if it fails).
IndexParse parse( const std::string& token, size_type& index ) {
  index = 99;
  return parseIndex(token, index);
}

} // namespace

TEST(parse_index_accepts_whole_numbers) {
  size_type index;
  CHECK(parse("1", index) == IndexParse::ok);
  CHECK_EQ(index, size_type(1));
  CHECK(parse("2097", index) == IndexParse::ok);
  CHECK_EQ(index, size_type(2097));
  CHECK(parse("0", index) == IndexParse::ok); // callers check the range
  CHECK_EQ(index, size_type(0));
  // Any number with a whole value, as when indices were read as doubles.
  CHECK(parse("+3", index) == IndexParse::ok);
  CHECK_EQ(index, size_type(3));
  CHECK(parse("12.0", index) == IndexParse::ok);
  CHECK_EQ(index, size_type(12));
  CHECK(parse("1.2e1", index) == IndexParse::ok);
  CHECK_EQ(index, size_type(12));
}

TEST(parse_index_rejects_negative_and_non_whole_numbers) {
  size_type index;
  CHECK(parse("-1", index) == IndexParse::negative);
  CHECK(parse("-12.0", index) == IndexParse::negative);
  CHECK(parse("2.5", index) == IndexParse::not_whole);
  CHECK(parse("-2.5", index) == IndexParse::not_whole);
  CHECK(parse("1e-1", index) == IndexParse::not_whole);
  CHECK(parse("3,", index) == IndexParse::not_whole);
  CHECK(parse("4x", index) == IndexParse::not_whole);
  CHECK(parse("1e30", index) == IndexParse::too_large);
  CHECK_EQ(index, size_type(99)); // unchanged on failure
}

TEST(parse_index_stops_at_words) {
  // Tokens that do not start like a number end a list of indices.
  size_type index;
  CHECK(parse("All", index) == IndexParse::not_a_number);
  CHECK(parse("Start:", index) == IndexParse::not_a_number);
  CHECK(parse("-", index) == IndexParse::not_a_number);
  CHECK(parse(".", index) == IndexParse::not_a_number);
  CHECK(parse("nan", index) == IndexParse::not_a_number);
  CHECK(parse("inf", index) == IndexParse::not_a_number);
  CHECK(parse("", index) == IndexParse::not_a_number);
  CHECK_EQ(index, size_type(99));
}

TEST(configf_indices_reads_a_list) {
  unit::TempConfig conf("Output: Node: 1 8 28 Start: 0.5 Interval: 1e-4\n");
  Configf configf(conf.path());
  CHECK(configf.next("Output"));
  CHECK(configf.next("Node"));
  const std::vector<size_type> nodes = configf.indices();
  CHECK_EQ(nodes.size(), size_type(3));
  if( nodes.size() == 3 ) {
    CHECK_EQ(nodes[0], size_type(1));
    CHECK_EQ(nodes[1], size_type(8));
    CHECK_EQ(nodes[2], size_type(28));
  }
  // The next entry is still read correctly.
  double start = 0.0;
  CHECK(configf.optional("Start", start));
  CHECK_EQ(start, 0.5);
}

TEST(configf_indices_empty_for_all) {
  // `Node: All` (any word) gives no indices, and leaves the word unread.
  unit::TempConfig conf("Output: Node: All Start: 5\n");
  Configf configf(conf.path());
  CHECK(configf.next("Output"));
  CHECK(configf.next("Node"));
  CHECK(configf.indices().empty());
  double start = 0.0;
  CHECK(configf.optional("Start", start));
  CHECK_EQ(start, 5.0);
}

TEST(configf_indices_at_end_of_file) {
  // The last index is kept even with no newline after it.
  unit::TempConfig conf("Node: 3 4");
  Configf configf(conf.path());
  CHECK(configf.next("Node"));
  const std::vector<size_type> nodes = configf.indices();
  CHECK_EQ(nodes.size(), size_type(2));
  if( nodes.size() == 2 ) {
    CHECK_EQ(nodes[0], size_type(3));
    CHECK_EQ(nodes[1], size_type(4));
  }
}
