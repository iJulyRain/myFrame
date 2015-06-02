aux_source_directory(. main)

link_directories(${PROJECT_SOURCE_DIR}/src/application
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

add_executable(${TARGET} ${main})

target_link_libraries(${TARGET} common app io buffer database zlog sqlite3 lua mb mxml pthread dl)
