PROJECT(radiation_machine)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

#add_definitions(-DDAEMON_MODE)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_C_FLAGS "-g -Wall -O2 ")


include_directories(${PROJECT_SOURCE_DIR}/include
					${PROJECT_SOURCE_DIR}/components/io
					${PROJECT_SOURCE_DIR}/components/buffer
					${PROJECT_SOURCE_DIR}/components/thread_pool
					${PROJECT_SOURCE_DIR}/components/mem_pool
					${PROJECT_SOURCE_DIR}/components/database
					${PROJECT_SOURCE_DIR}/kernel
					${PROJECT_SOURCE_DIR}/3rd/sqlite3
					${PROJECT_SOURCE_DIR}/3rd/ini
					${PROJECT_SOURCE_DIR}/3rd/zlog
					${PROJECT_SOURCE_DIR}/3rd/mxml
					${PROJECT_SOURCE_DIR}/3rd/libini
					${PROJECT_SOURCE_DIR}/3rd/cJSON
					${PROJECT_SOURCE_DIR}/3rd/base64
					${PROJECT_SOURCE_DIR}/3rd/sha1
					${PROJECT_SOURCE_DIR}/3rd/zookeeper/include
					${PROJECT_SOURCE_DIR}/3rd/etpan/include
					${PROJECT_SOURCE_DIR}/3rd/dovecot/lib
					${PROJECT_SOURCE_DIR}/3rd/dovecot/
					${PROJECT_SOURCE_DIR}/application/socks5/
					)

link_directories(
				 ${PROJECT_SOURCE_DIR}/
				 ${PROJECT_SOURCE_DIR}/kernel
				 ${PROJECT_SOURCE_DIR}/components/io
				 ${PROJECT_SOURCE_DIR}/components/buffer
				 ${PROJECT_SOURCE_DIR}/components/thread_pool
				 ${PROJECT_SOURCE_DIR}/components/mem_pool
				 ${PROJECT_SOURCE_DIR}/components/database
				 ${PROJECT_SOURCE_DIR}/3rd/common
				 ${PROJECT_SOURCE_DIR}/3rd/mxml
				 ${PROJECT_SOURCE_DIR}/3rd/sqlite3
				 ${PROJECT_SOURCE_DIR}/3rd/zlog
				 ${PROJECT_SOURCE_DIR}/3rd/libini
				 ${PROJECT_SOURCE_DIR}/3rd/cJSON
				 ${PROJECT_SOURCE_DIR}/3rd/base64
				 ${PROJECT_SOURCE_DIR}/3rd/sha1
				 ${PROJECT_SOURCE_DIR}/3rd/zookeeper/lib
				 ${PROJECT_SOURCE_DIR}/3rd/etpan/lib
				 ${PROJECT_SOURCE_DIR}/3rd/dovecot/
				 )

add_subdirectory(kernel)
add_subdirectory(3rd)
add_subdirectory(components)

add_library(myevent STATIC build.c)
target_link_libraries(myevent
	common 
    kernel 
    io 
    buffer 
    thread_pool 
    mem_pool 
    database 
    zlog 
    sqlite3 
    mxml
    ini
    cjson
    base64
    sha1
    md5
	pthread dl m
)

add_subdirectory(application)
