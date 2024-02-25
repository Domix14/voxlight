cmake_minimum_required(VERSION 2.26)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(VoxlightTests entity_api/entity_api_test.cpp)

target_link_libraries(VoxlightTests GTest::gtest_main voxlight)

target_compile_features(VoxlightTests PRIVATE cxx_std_20)

include(GoogleTest)
gtest_discover_tests(VoxlightTests)
