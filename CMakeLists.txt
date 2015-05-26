PROJECT(radiation_machine)

cmake_minimum_required(VERSION 2.8)

set(TARGET main)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_C_FLAGS -Wall)

include_directories(${PROJECT_SOURCE_DIR}/include
					${PROJECT_SOURCE_DIR}/components/include
					${PROJECT_SOURCE_DIR}/components/433
					${PROJECT_SOURCE_DIR}/utilities/include
					${PROJECT_SOURCE_DIR}/utilities/sqlite3
					${PROJECT_SOURCE_DIR}/utilities/ini
					${PROJECT_SOURCE_DIR}/utilities/modbus
					)

add_subdirectory(utilities)
add_subdirectory(components)
add_subdirectory(src)
