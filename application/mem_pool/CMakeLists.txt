aux_source_directory(. mem_pool)

set(TARGET mem_pool_test)

add_executable(${TARGET} ${mem_pool})

target_link_libraries(${TARGET} myevent)
