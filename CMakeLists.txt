cmake_minimum_required(VERSION 3.10)
project(TR-Assignment)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET_LIB tr)

find_package(Boost 1.74 REQUIRED COMPONENTS system filesystem)
find_package(OpenSSL REQUIRED)


include_directories(
	${Boost_INCLUDE_DIRS}
	${OPENSSL_INCLUDE_DIR}
	/usr/local/include/json
)

find_package(jsoncpp REQUIRED)
include_directories(${JSONCPP_INCLUDE_DIR})
link_libraries(${JSONCPP_LIBRARIES})

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(tests)

add_subdirectory(lib/spdlog)

add_subdirectory(third_party/googletest)
add_subdirectory(app)

enable_testing()