cmake_minimum_required(VERSION 3.14)
project(YetAnotherLispEvaluator)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(include)

enable_testing()

# add_executable(hello_test test/hello_test.cc)

file(GLOB_RECURSE yale_src CONFIGURE_DEPENDS "./src/*.cc")

add_executable(scanner_test ${yale_src} test/scanner_test.cc)

target_link_libraries(
#  hello_test
  scanner_test
  gtest_main
)

include(GoogleTest)

# gtest_discover_tests(hello_test)
gtest_discover_tests(scanner_test)
