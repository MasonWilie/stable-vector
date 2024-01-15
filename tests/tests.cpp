#include <gtest/gtest.h>
#include <string>
#include "../stable_vector.h"

namespace
{

class EmptyClass
{
};

}  // namespace

TEST(constructor, basic_int)
{
    sv::stable_vector<int> v;

    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
}

TEST(constructor, empty_class)
{
    sv::stable_vector<EmptyClass> v;

    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
}

TEST(constructor, string)
{
    sv::stable_vector<std::string> v;

    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
}

TEST(push_back, basic_int)
{
    constexpr int TEST_VALUE = 1;

    sv::stable_vector<int> v;
    v.push_back(TEST_VALUE);

    EXPECT_EQ(v.size(), 1);
    EXPECT_GT(v.capacity(), 0);
    
    ASSERT_NO_THROW(v.at(0));
    EXPECT_EQ(v.at(0), TEST_VALUE);
}

TEST(push_back, empty_class_lvalue)
{
    sv::stable_vector<EmptyClass> v;

    auto x = EmptyClass();
    v.push_back(x);

    EXPECT_EQ(v.size(), 1);
    EXPECT_GT(v.capacity(), 0);
    
    ASSERT_NO_THROW(v.at(0));
}

TEST(push_back, empty_class_rvalue)
{
    sv::stable_vector<EmptyClass> v;

    v.push_back(EmptyClass());

    EXPECT_EQ(v.size(), 1);
    EXPECT_GT(v.capacity(), 0);
    
    ASSERT_NO_THROW(v.at(0));
}

TEST(push_back, string_const_lvalue)
{
    static const std::string value = "value";
    
    sv::stable_vector<std::string> v;

    v.push_back(value);

    EXPECT_EQ(v.size(), 1);
    EXPECT_GT(v.capacity(), 0);

    ASSERT_NO_THROW(v.at(0));
    EXPECT_EQ(v.at(0), value);
}

TEST(push_back, string_non_const_lvalue)
{
    sv::stable_vector<std::string> v;

    std::string value = "value";
    v.push_back(value);

    EXPECT_EQ(v.size(), 1);
    EXPECT_GT(v.capacity(), 0);

    ASSERT_NO_THROW(v.at(0));
    EXPECT_EQ(v.at(0), value);
}

TEST(push_back, string_rvalue)
{
    sv::stable_vector<std::string> v;

    std::string s = "value";
    v.push_back(std::move(s));

    EXPECT_EQ(v.size(), 1);
    EXPECT_GT(v.capacity(), 0);

    ASSERT_NO_THROW(v.at(0));
    EXPECT_EQ(v.at(0), "value");
}