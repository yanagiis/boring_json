cmake_minimum_required(VERSION 3.12.4)

option(CONFIG_DEBUG "Enable debug print" OFF)
option(CONFIG_TESTING "Build testing" OFF)

project(boringjson C)

set(BORING_JSON_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

# boring json

set(SRCS
    ${BORING_JSON_ROOT_DIR}/src/boring_json.c
    ${BORING_JSON_ROOT_DIR}/src/boring_json_encode.c
    ${BORING_JSON_ROOT_DIR}/src/boring_json_decode.c
    ${BORING_JSON_ROOT_DIR}/src/boring_json_simple_writer.c)

add_library(boring_json ${SRCS})
target_include_directories(
  boring_json
  PUBLIC ${BORING_JSON_ROOT_DIR}/include
  PRIVATE ${BORING_JSON_ROOT_DIR}/src)

if(CONFIG_DEBUG)
  target_compile_options(boring_json PUBLIC "-Wall" "-Wextra" "-Werror" PRIVATE "-DCONFIG_DEBUG")
else()
  target_compile_options(boring_json PUBLIC "-Wall" "-Wextra" "-Werror")
endif()

if(CONFIG_TESTING)

  # deps/unity

  set(UNITY_SRCS ${BORING_JSON_ROOT_DIR}/deps/unity/src/unity.c)
  add_library(unity ${UNITY_SRCS})
  target_include_directories(unity
                             PUBLIC ${BORING_JSON_ROOT_DIR}/deps/unity/src)
  target_compile_options(unity PUBLIC "-DUNITY_INCLUDE_DOUBLE")

  # test

  set(TEST_SRCS
      ${BORING_JSON_ROOT_DIR}/test/test_decode.c
      ${BORING_JSON_ROOT_DIR}/test/test_encode.c
      ${BORING_JSON_ROOT_DIR}/test/runner.c)

  add_executable(boring_json_test ${TEST_SRCS})
  target_include_directories(boring_json_test PRIVATE deps/unity/src)
  target_link_libraries(boring_json_test boring_json unity)

endif()
