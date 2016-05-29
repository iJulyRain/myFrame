aux_source_directory(. ssocks)

set(TARGET ssocks)

add_executable(${TARGET} ${ssocks})
target_link_libraries(${TARGET} myevent)
