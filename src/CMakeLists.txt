# 获取源文件列表
file(GLOB_RECURSE SOURCES "*.cpp")

add_library(${TARGET_LIB} SHARED ${SOURCES})

set_target_properties(${TARGET_LIB} PROPERTIES
    VERSION 1.0.0
	SOVERSION 1
)

target_link_libraries(${TARGET_LIB} PRIVATE
	spdlog::spdlog
	OpenSSL::SSL OpenSSL::Crypto
	jsoncpp
)



target_include_directories(${TARGET_LIB} PRIVATE 
	${CMAKE_SOURCE_DIR}/include
)