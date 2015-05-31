aux_source_directory(. main)

link_directories(${PROJECT_SOURCE_DIR}/src/app
				 ${PROJECT_SOURCE_DIR}/components/protocol/io
				 ${PROJECT_SOURCE_DIR}/utilities/common
				 ${PROJECT_SOURCE_DIR}/utilities/sqlite3)

add_executable(${TARGET} ${main})

target_link_libraries(${TARGET} common app zlog pthread dl)
