aux_source_directory(. rcsocks)

set(TARGET rcsocks)

add_executable(${TARGET} ${rcsocks})
target_link_libraries(${TARGET} myevent)
