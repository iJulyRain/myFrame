add_definitions(-DSQLITE_MIXED_ENDIAN_64BIT_FLOAT)

aux_source_directory(. sqlite3)
add_library(sqlite3 STATIC ${sqlite3})
