#include <iostream>
#include <gtest/gtest.h>

int Add(int x, int y)
{
    return x + y;
}

TEST(Plus_Test, Equal)
{
    ASSERT_EQ(Add(10, 10), 20);
}

TEST(ERROR_TEST, NE)
{
    ASSERT_EQ(Add(10, 10), 30);
}

TEST(EXPECT_TEXT, EXPECT)
{
    EXPECT_LT(Add(20, 20), 10);
  
    std::cout << "Output in EXPECT_TEST, but after EXPECT_LT" << std::endl;

    ASSERT_EQ(Add(10, 10), 30);

    std::cout << "Output in EXPECT_TEST" << std::endl;
}

int main(int argc, char* argv[])
{
    // Init Test Module
    testing::InitGoogleTest(&argc, argv);

    // Invokle all tests
    return RUN_ALL_TESTS();
}