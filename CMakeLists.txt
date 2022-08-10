cmake_minimum_required(VERSION 3.15)
project(xTeaLib CXX)

add_library(xTeaLib src/xTea.cpp)
target_include_directories(xTeaLib PUBLIC include)

set_target_properties(xTeaLib PROPERTIES PUBLIC_HEADER "src/xTea.h")
install(TARGETS xTeaLib)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/xTea_console.cpp")
	add_executable(xTea_console xTea_console.cpp)
	set_target_properties(xTea_console PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Debug")
	target_link_libraries(xTea_console PRIVATE xTeaLib)
endif()

