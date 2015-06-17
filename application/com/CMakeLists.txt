aux_source_directory(. com)

set(TARGET com_test)

add_executable(${TARGET} ${com})
target_link_libraries(${TARGET} myevent)
