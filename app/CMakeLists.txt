set(TARGET_EXEC main_exec)

add_executable(${TARGET_EXEC} main.cpp)

target_link_libraries(${TARGET_EXEC} PRIVATE ${TARGET_LIB})
target_include_directories(${TARGET_EXEC} PRIVATE ${CMAKE_SOURCE_DIR}/include)
