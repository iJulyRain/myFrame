aux_source_directory(. rssocks)

set(TARGET rssocks)

add_executable(${TARGET} ${rssocks})
target_link_libraries(${TARGET} myevent)
