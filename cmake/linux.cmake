set(SRCS ${BORING_JSON_ROOT_DIR}/src/boring_json_encode.c
         ${BORING_JSON_ROOT_DIR}/src/boring_json_decode.c)

add_library(boring_json ${SRCS})
target_include_directories(boring_json PUBLIC ${BORING_JSON_ROOT_DIR}/include)

add_executable(test_prim ${BORING_JSON_ROOT_DIR}/test/prim.c)
target_link_libraries(test_prim boring_json)
