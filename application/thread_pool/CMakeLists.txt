aux_source_directory(. thread_pool)

set(TARGET thread_pool_test)

add_executable(${TARGET} ${thread_pool})

target_link_libraries(${TARGET} myevent)
