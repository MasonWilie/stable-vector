#include <gtest/gtest.h>
#include "../stable_vector.h"

namespace
{

class EmptyClass
{
};

}  // namespace

TEST(Constructor, Basic_int)
{
    sv::stable_vector<int> v;

    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
}

TEST(Constructor, Basic_EmptyClass)
{
    sv::stable_vector<EmptyClass> v;

    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
}