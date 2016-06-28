aux_source_directory(. IO_SRC)
aux_source_directory(tcp_server TCP_SERVER_SRC)
aux_source_directory(io_pool IO_POOL_SERVER_SRC)

add_library(io STATIC ${IO_SRC} ${TCP_SERVER_SRC} ${IO_POOL_SERVER_SRC})
