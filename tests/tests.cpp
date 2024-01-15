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

TEST(emplace_back, empty_class_no_arguments)
{
    sv::stable_vector<EmptyClass> v;

    v.emplace_back();

    EXPECT_EQ(v.size(), 1);
    EXPECT_GT(v.capacity(), 0);
    
    ASSERT_NO_THROW(v.at(0));
}

TEST(emplace_back, string_no_arguments)
{
    sv::stable_vector<std::string> v;

    v.emplace_back();

    EXPECT_EQ(v.size(), 1);
    EXPECT_GT(v.capacity(), 0);
    
    ASSERT_NO_THROW(v.at(0));

    EXPECT_EQ(v.at(0), "");
}

namespace
{

class ClassWithArguments
{
 public:
    ClassWithArguments(int a, std::string b, double c) noexcept
        : _a(a), _b(b), _c(c)
    {}

    bool operator==(const ClassWithArguments& other) const
    {
        return _a == other._a
            && _b == other._b
            && _c == other._c;
    }

 private:
    const int _a;
    const std::string _b;
    const double _c;
};

}  // namespace

TEST(emplace_back, class_with_arguments)
{
    sv::stable_vector<ClassWithArguments> v;

    v.emplace_back(1, "test", 5.0);

    EXPECT_EQ(v.size(), 1);
    EXPECT_GT(v.capacity(), 0);
    
    ASSERT_NO_THROW(v.at(0));

    EXPECT_EQ(v.at(0), ClassWithArguments(1, "test", 5.0));
}

TEST(front_back, int)
{
    sv::stable_vector<int> v;

    v.emplace_back(1);
    v.emplace_back(2);
    v.emplace_back(3);

    EXPECT_EQ(v.size(), 3);
    EXPECT_GE(v.capacity(), 3);

    auto& front = v.front();
    auto& back = v.back();

    EXPECT_EQ(front, 1);
    EXPECT_EQ(back, 3);

    front = 5;
    back = 6;

    EXPECT_EQ(v.front(), 5);
    EXPECT_EQ(v.back(), 6);   
}

TEST(at, throws_exception)
{
    sv::stable_vector<int> v;
    EXPECT_THROW(v.at(0), std::out_of_range);

    v.push_back(1);

    EXPECT_NO_THROW(v.at(0));
    EXPECT_THROW(v.at(1), std::out_of_range);
}