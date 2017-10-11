#include <gtest/gtest.h>


class FirstTest: public testing::Test
{
protected:
    FirstTest() = default;
    virtual ~FirstTest() = default;

    int BladeRunner(void){return 2018;}

};

TEST_F(FirstTest, HelloBaby)
{
    ASSERT_EQ(2019, BladeRunner());
}

int main(int args, char** argv)
{
    ::testing::InitGoogleTest(&args, argv);
    return RUN_ALL_TESTS();
}
