aux_source_directory(. lua)
add_definitions(-DLUA_USE_LINUX)

add_library(lua STATIC ${lua})
