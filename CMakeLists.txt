cmake_minimum_required(VERSION 3.12.4)

if(NOT ESP_PLATFORM)
  project(boringjson C)
endif()

set(BORING_JSON_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

# boring json

set(SOURCES
    ${BORING_JSON_ROOT_DIR}/src/boring_json.c
    ${BORING_JSON_ROOT_DIR}/src/boring_json_encode.c
    ${BORING_JSON_ROOT_DIR}/src/boring_json_decode.c
    ${BORING_JSON_ROOT_DIR}/src/boring_json_simple_writer.c
    ${BORING_JSON_ROOT_DIR}/src/boring_json_mem_writer.c)

if(ZEPHYR_BASE)
  include(${CMAKE_CURRENT_LIST_DIR}/cmake/zephyr.cmake)
elseif(ESP_PLATFORM)
  include(${CMAKE_CURRENT_LIST_DIR}/cmake/esp.cmake)
else()
  include(${CMAKE_CURRENT_LIST_DIR}/cmake/linux.cmake)
endif()
