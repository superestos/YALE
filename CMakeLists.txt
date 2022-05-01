cmake_minimum_required(VERSION 3.14)
project(YetAnotherLispEvaluator)

set(CMAKE_CXX_STANDARD 17)

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

file(GLOB_RECURSE yale_src CONFIGURE_DEPENDS "./src/*.cc")

add_executable(yale ${yale_src} main.cc)

add_executable(hello_test test/hello_test.cc)
add_executable(scanner_test ${yale_src} test/scanner_test.cc)
add_executable(parser_test ${yale_src} test/parser_test.cc)
add_executable(expression_test ${yale_src} test/expression_test.cc)
add_executable(environment_test ${yale_src} test/environment_test.cc)
add_executable(procedure_test ${yale_src} test/procedure_test.cc)

target_link_libraries(hello_test gtest_main)
target_link_libraries(scanner_test gtest_main)
target_link_libraries(parser_test gtest_main)
target_link_libraries(expression_test gtest_main)
target_link_libraries(environment_test gtest_main)
target_link_libraries(procedure_test gtest_main)

include(GoogleTest)

# gtest_discover_tests(hello_test)
# gtest_discover_tests(scanner_test)
