PROJECT(radiation_machine)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_C_FLAGS "-Wall -g")


include_directories(${PROJECT_SOURCE_DIR}/include
					${PROJECT_SOURCE_DIR}/components/io
					${PROJECT_SOURCE_DIR}/components/buffer
					${PROJECT_SOURCE_DIR}/components/database
					${PROJECT_SOURCE_DIR}/kernel
					${PROJECT_SOURCE_DIR}/3rd/sqlite3
					${PROJECT_SOURCE_DIR}/3rd/ini
					${PROJECT_SOURCE_DIR}/3rd/modbus
					${PROJECT_SOURCE_DIR}/3rd/zlog
					${PROJECT_SOURCE_DIR}/3rd/mxml
					${PROJECT_SOURCE_DIR}/3rd/lua
					)

link_directories(
				 ${PROJECT_SOURCE_DIR}/
				 ${PROJECT_SOURCE_DIR}/kernel
				 ${PROJECT_SOURCE_DIR}/components/io
				 ${PROJECT_SOURCE_DIR}/components/buffer
				 ${PROJECT_SOURCE_DIR}/components/thread_pool
				 ${PROJECT_SOURCE_DIR}/components/database
				 ${PROJECT_SOURCE_DIR}/3rd/common
				 ${PROJECT_SOURCE_DIR}/3rd/lua
				 ${PROJECT_SOURCE_DIR}/3rd/modbus
				 ${PROJECT_SOURCE_DIR}/3rd/mxml
				 ${PROJECT_SOURCE_DIR}/3rd/sqlite3
				 ${PROJECT_SOURCE_DIR}/3rd/zlog
				 )

add_subdirectory(kernel)
add_subdirectory(3rd)
add_subdirectory(components)

add_library(myevent STATIC build.c)
target_link_libraries(
	myevent
	"-Wl,--whole-archive"
	common kernel io buffer thread_pool database zlog sqlite3 lua mb mxml 
	"-Wl,--no-whole-archive"
	pthread dl m
)

add_subdirectory(application)
