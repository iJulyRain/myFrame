aux_source_directory(. rserver)

set(TARGET rserver)

add_executable(${TARGET} ${rserver})
target_link_libraries(${TARGET} myevent)
