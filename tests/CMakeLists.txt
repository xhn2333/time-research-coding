enable_testing()

set(TARGET_TESTS test_exec)

# 获取测试源文件
file(GLOB_RECURSE TEST_SOURCES "*.cpp")
add_executable(${TARGET_TESTS} ${TEST_SOURCES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fPIC -Issl -Wall -Wextra -O2")

# 链接库和 gtest_main（使用默认 main）
target_link_libraries(${TARGET_TESTS} PRIVATE
    ${TARGET_LIB}
    gtest
    gtest_main
    spdlog::spdlog
    OpenSSL::SSL OpenSSL::Crypto
    jsoncpp
)

target_include_directories(${TARGET_TESTS} PRIVATE ${CMAKE_SOURCE_DIR}/include)

foreach(test_file ${TEST_SOURCES})
    get_filename_component(test_name ${test_file} NAME_WE)
    add_test(NAME ${test_name} COMMAND ${TARGET_TESTS} ${test_name})
endforeach()
