CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
PROJECT(stable_vector_tests VERSION 1.0.0)

# Google Test
INCLUDE(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
)

SET(BUILD_GTEST ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Stable Vector Tests
SET(SRC
    tests.cpp
)

ADD_EXECUTABLE(stable_vector_test main.cpp ${SRC})
SET_PROPERTY(TARGET stable_vector_test PROPERTY CXX_STANDARD 17)
INCLUDE_DIRECTORIES(..)
TARGET_LINK_LIBRARIES(stable_vector_test gtest)