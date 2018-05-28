#include <gtest/gtest.h>
#include <blackjack/table.h>

TEST(BlackJack, table_createTable_validOutput) {
  table_t table = createTable();
  ASSERT_TRUE(false);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
