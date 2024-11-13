cmake_minimum_required(VERSION 3.10)
project(TR-Assignment)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET_LIB tr)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fPIC -Issl -Wall -Wextra -O2")

find_package(Boost 1.74 REQUIRED COMPONENTS system filesystem)
find_package(OpenSSL REQUIRED)
find_package(jsoncpp REQUIRED)

include_directories(
	${Boost_INCLUDE_DIRS}
	${OPENSSL_INCLUDE_DIR}
	${JSONCPP_INCLUDE_DIR}
)


include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(app)

add_subdirectory(lib/spdlog)
add_subdirectory(third_party/googletest)


enable_testing()