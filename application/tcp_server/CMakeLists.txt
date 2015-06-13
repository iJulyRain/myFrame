aux_source_directory(. tcp_server)

set(TARGET tcp_server_test)

link_directories(${PROJECT_SOURCE_DIR}/kernel
				 ${PROJECT_SOURCE_DIR}/components/io
				 ${PROJECT_SOURCE_DIR}/components/buffer
				 ${PROJECT_SOURCE_DIR}/components/database
				 ${PROJECT_SOURCE_DIR}/utilities/common
				 ${PROJECT_SOURCE_DIR}/utilities/lua
				 ${PROJECT_SOURCE_DIR}/utilities/modbus
				 ${PROJECT_SOURCE_DIR}/utilities/mxml
				 ${PROJECT_SOURCE_DIR}/utilities/sqlite3
				 ${PROJECT_SOURCE_DIR}/utilities/zlog
				 )

add_executable(${TARGET} ${tcp_server})

target_link_libraries(${TARGET} common kernel io buffer database zlog sqlite3 lua mb mxml pthread dl)
