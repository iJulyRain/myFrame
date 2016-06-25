aux_source_directory(. rclient)

set(TARGET rclient)

add_executable(${TARGET} ${rclient})
target_link_libraries(${TARGET} myevent)
