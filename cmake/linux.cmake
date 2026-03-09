option(CONFIG_BORING_JSON_DEBUG "Enable debug print" OFF)
option(CONFIG_BORING_JSON_TESTING "Build testing" OFF)
option(CONFIG_BORING_JSON_TEST_SANITIZERS "Enable Clang sanitizers for runtime tests" OFF)

if(CONFIG_BORING_JSON_TEST_SANITIZERS)
  if(NOT CONFIG_BORING_JSON_TESTING)
    message(FATAL_ERROR
            "CONFIG_BORING_JSON_TEST_SANITIZERS requires CONFIG_BORING_JSON_TESTING=ON")
  endif()
  if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "CONFIG_BORING_JSON_TEST_SANITIZERS supports Linux runtime tests only")
  endif()
  if(NOT CMAKE_C_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR
            "CONFIG_BORING_JSON_TEST_SANITIZERS requires Clang for Linux runtime tests")
  endif()
endif()

add_library(boring_json ${SOURCES})
target_compile_features(boring_json PUBLIC c_std_11)

target_include_directories(
  boring_json
  PUBLIC ${BORING_JSON_ROOT_DIR}/include
  PRIVATE ${BORING_JSON_ROOT_DIR}/src)

if(CONFIG_BORING_JSON_TESTING)

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

  set(MACRO_COMPILE_FAIL_SRC ${BORING_JSON_ROOT_DIR}/test/test_macro_compile_fail.c)
  set(MACRO_COMPILE_FAIL_SCRIPT ${BORING_JSON_ROOT_DIR}/cmake/run_expected_compile_failure.cmake)
  set(MACRO_COMPILE_FAIL_HEADER ${BORING_JSON_ROOT_DIR}/include/boring_json.h)
  set(MACRO_COMPILE_FAIL_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_macro_compile_fail)
  get_target_property(MACRO_COMPILE_FAIL_INCLUDE_DIRS boring_json INCLUDE_DIRECTORIES)
  get_target_property(MACRO_COMPILE_FAIL_COMPILE_DEFINITIONS boring_json COMPILE_DEFINITIONS)
  get_target_property(MACRO_COMPILE_FAIL_COMPILE_OPTIONS boring_json COMPILE_OPTIONS)

  if(MACRO_COMPILE_FAIL_INCLUDE_DIRS STREQUAL "MACRO_COMPILE_FAIL_INCLUDE_DIRS-NOTFOUND")
    set(MACRO_COMPILE_FAIL_INCLUDE_DIRS)
  endif()
  if(MACRO_COMPILE_FAIL_COMPILE_DEFINITIONS STREQUAL
     "MACRO_COMPILE_FAIL_COMPILE_DEFINITIONS-NOTFOUND")
    set(MACRO_COMPILE_FAIL_COMPILE_DEFINITIONS)
  endif()
  if(MACRO_COMPILE_FAIL_COMPILE_OPTIONS STREQUAL "MACRO_COMPILE_FAIL_COMPILE_OPTIONS-NOTFOUND")
    set(MACRO_COMPILE_FAIL_COMPILE_OPTIONS)
  endif()

  set(MACRO_COMPILE_FAIL_SCENARIOS
      TEST_WRONG_BOOL_MEMBER_TYPE
      TEST_WRONG_INT_MEMBER_TYPE
      TEST_WRONG_INT64_MEMBER_TYPE
      TEST_WRONG_DOUBLE_MEMBER_TYPE
      TEST_WRONG_EXIST_FIELD_TYPE
      TEST_INFERRED_EXIST_FIELD_MISSING
      TEST_INFERRED_EXIST_FIELD_WRONG_TYPE
      TEST_WRONG_FLAGS_FIELD_TYPE
      TEST_WRONG_BOOL_OR_NULL_FLAGS_FIELD_TYPE
      TEST_WRONG_INT_OR_NULL_FLAGS_FIELD_TYPE
      TEST_WRONG_INT64_OR_NULL_FLAGS_FIELD_TYPE
      TEST_WRONG_DOUBLE_OR_NULL_FLAGS_FIELD_TYPE
      TEST_CSTR_ARRAY_POINTER_TYPE
      TEST_CSTR_ARRAY_OR_NULL_POINTER_TYPE
      TEST_OBJECT_OR_NULL_WRONG_FLAGS_FIELD_TYPE
      TEST_WRONG_COUNT_FIELD_TYPE
      TEST_ARRAY_OR_NULL_WRONG_COUNT_FIELD_TYPE
      TEST_TYPED_OBJECT_WRONG_MEMBER_TYPE
      TEST_TYPED_OBJECT_INFERRED_EXIST_FIELD_MISSING
      TEST_TYPED_OBJECT_OR_NULL_WRONG_MEMBER_TYPE
      TEST_TYPED_ARRAY_WRONG_ELEMENT_TYPE
      TEST_TYPED_ARRAY_INFERRED_EXIST_FIELD_MISSING
      TEST_TYPED_ARRAY_OR_NULL_WRONG_ELEMENT_TYPE
      TEST_VALUE_STRUCT_WRONG_INT_MEMBER_TYPE
      TEST_VALUE_STRUCT_WRONG_INT64_MEMBER_TYPE
      TEST_VALUE_STRUCT_WRONG_DOUBLE_MEMBER_TYPE
      TEST_VALUE_STRUCT_CSTR_ARRAY_POINTER_TYPE
      TEST_VALUE_STRUCT_WRONG_COUNT_FIELD_TYPE
      TEST_VALUE_STRUCT_OBJECT_TYPED_WRONG_MEMBER_TYPE
      TEST_VALUE_STRUCT_ARRAY_TYPED_WRONG_ELEMENT_TYPE)
  set(MACRO_COMPILE_FAIL_OUTPUTS)

  foreach(scenario IN LISTS MACRO_COMPILE_FAIL_SCENARIOS)
    set(expected_token)
    if(scenario STREQUAL TEST_WRONG_BOOL_MEMBER_TYPE)
      set(expected_token BO_JSON_expected_bool_member)
    elseif(scenario STREQUAL TEST_WRONG_INT_MEMBER_TYPE)
      set(expected_token BO_JSON_expected_int_member)
    elseif(scenario STREQUAL TEST_WRONG_INT64_MEMBER_TYPE)
      set(expected_token BO_JSON_expected_int64_t_member)
    elseif(scenario STREQUAL TEST_WRONG_DOUBLE_MEMBER_TYPE)
      set(expected_token BO_JSON_expected_double_member)
    elseif(scenario STREQUAL TEST_WRONG_EXIST_FIELD_TYPE)
      set(expected_token BO_JSON_expected_bool_exist_field)
    elseif(scenario STREQUAL TEST_INFERRED_EXIST_FIELD_MISSING)
      set(expected_token value_exist)
    elseif(scenario STREQUAL TEST_INFERRED_EXIST_FIELD_WRONG_TYPE)
      set(expected_token BO_JSON_expected_bool_exist_field)
    elseif(scenario STREQUAL TEST_WRONG_FLAGS_FIELD_TYPE)
      set(expected_token BO_JSON_expected_unsigned_char_flags_field)
    elseif(scenario STREQUAL TEST_WRONG_BOOL_OR_NULL_FLAGS_FIELD_TYPE)
      set(expected_token BO_JSON_expected_unsigned_char_flags_field)
    elseif(scenario STREQUAL TEST_WRONG_INT_OR_NULL_FLAGS_FIELD_TYPE)
      set(expected_token BO_JSON_expected_unsigned_char_flags_field)
    elseif(scenario STREQUAL TEST_WRONG_INT64_OR_NULL_FLAGS_FIELD_TYPE)
      set(expected_token BO_JSON_expected_unsigned_char_flags_field)
    elseif(scenario STREQUAL TEST_WRONG_DOUBLE_OR_NULL_FLAGS_FIELD_TYPE)
      set(expected_token BO_JSON_expected_unsigned_char_flags_field)
    elseif(scenario STREQUAL TEST_CSTR_ARRAY_POINTER_TYPE)
      set(expected_token BO_JSON_expected_char_array_member)
    elseif(scenario STREQUAL TEST_CSTR_ARRAY_OR_NULL_POINTER_TYPE)
      set(expected_token BO_JSON_expected_unsigned_char_flags_field)
    elseif(scenario STREQUAL TEST_OBJECT_OR_NULL_WRONG_FLAGS_FIELD_TYPE)
      set(expected_token BO_JSON_expected_unsigned_char_flags_field)
    elseif(scenario STREQUAL TEST_WRONG_COUNT_FIELD_TYPE)
      set(expected_token BO_JSON_expected_size_t_count_field)
    elseif(scenario STREQUAL TEST_ARRAY_OR_NULL_WRONG_COUNT_FIELD_TYPE)
      set(expected_token BO_JSON_expected_unsigned_char_flags_field)
    elseif(scenario STREQUAL TEST_TYPED_OBJECT_WRONG_MEMBER_TYPE)
      set(expected_token BO_JSON_expected_typed_object_member)
    elseif(scenario STREQUAL TEST_TYPED_OBJECT_INFERRED_EXIST_FIELD_MISSING)
      set(expected_token value_exist)
    elseif(scenario STREQUAL TEST_TYPED_OBJECT_OR_NULL_WRONG_MEMBER_TYPE)
      set(expected_token BO_JSON_expected_typed_object_member)
    elseif(scenario STREQUAL TEST_TYPED_ARRAY_WRONG_ELEMENT_TYPE)
      set(expected_token BO_JSON_expected_typed_array_element)
    elseif(scenario STREQUAL TEST_TYPED_ARRAY_INFERRED_EXIST_FIELD_MISSING)
      set(expected_token values_exist)
    elseif(scenario STREQUAL TEST_TYPED_ARRAY_OR_NULL_WRONG_ELEMENT_TYPE)
      set(expected_token BO_JSON_expected_typed_array_element)
    elseif(scenario STREQUAL TEST_VALUE_STRUCT_WRONG_INT_MEMBER_TYPE)
      set(expected_token BO_JSON_expected_int_member)
    elseif(scenario STREQUAL TEST_VALUE_STRUCT_WRONG_INT64_MEMBER_TYPE)
      set(expected_token BO_JSON_expected_int64_t_member)
    elseif(scenario STREQUAL TEST_VALUE_STRUCT_WRONG_DOUBLE_MEMBER_TYPE)
      set(expected_token BO_JSON_expected_double_member)
    elseif(scenario STREQUAL TEST_VALUE_STRUCT_CSTR_ARRAY_POINTER_TYPE)
      set(expected_token BO_JSON_expected_char_array_member)
    elseif(scenario STREQUAL TEST_VALUE_STRUCT_WRONG_COUNT_FIELD_TYPE)
      set(expected_token BO_JSON_expected_size_t_count_field)
    elseif(scenario STREQUAL TEST_VALUE_STRUCT_OBJECT_TYPED_WRONG_MEMBER_TYPE)
      set(expected_token BO_JSON_expected_typed_object_member)
    elseif(scenario STREQUAL TEST_VALUE_STRUCT_ARRAY_TYPED_WRONG_ELEMENT_TYPE)
      set(expected_token BO_JSON_expected_typed_array_element)
    else()
      message(FATAL_ERROR "No expected diagnostic token configured for ${scenario}")
    endif()

    set(output ${MACRO_COMPILE_FAIL_DIR}/${scenario}.stamp)
    set(object_file ${MACRO_COMPILE_FAIL_DIR}/${scenario}.o)
    add_custom_command(
      OUTPUT ${output}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${MACRO_COMPILE_FAIL_DIR}
      COMMAND ${CMAKE_COMMAND}
              -DCOMPILER=${CMAKE_C_COMPILER}
              -DFIXTURE=${MACRO_COMPILE_FAIL_SRC}
              -DC_STANDARD=${CMAKE_C_STANDARD}
              -DC_EXTENSIONS=${CMAKE_C_EXTENSIONS}
              -DINCLUDE_DIRS=${MACRO_COMPILE_FAIL_INCLUDE_DIRS}
              -DCOMPILE_DEFINITIONS=${MACRO_COMPILE_FAIL_COMPILE_DEFINITIONS}
              -DCOMPILE_OPTIONS=${MACRO_COMPILE_FAIL_COMPILE_OPTIONS}
              -DSCENARIO=${scenario}
              -DEXPECTED_TOKEN=${expected_token}
              -DOBJECT_FILE=${object_file}
              -DSTAMP_FILE=${output}
              -P ${MACRO_COMPILE_FAIL_SCRIPT}
      DEPENDS ${MACRO_COMPILE_FAIL_SRC} ${MACRO_COMPILE_FAIL_HEADER}
              ${MACRO_COMPILE_FAIL_SCRIPT}
      VERBATIM)
    list(APPEND MACRO_COMPILE_FAIL_OUTPUTS ${output})
  endforeach()

  add_executable(boring_json_test ${TEST_SRCS})
  target_compile_features(boring_json_test PRIVATE c_std_11)
  target_include_directories(boring_json_test PRIVATE deps/unity/src)
  target_link_libraries(boring_json_test boring_json unity)

  add_custom_target(boring_json_negative_tests ALL DEPENDS ${MACRO_COMPILE_FAIL_OUTPUTS})
  add_dependencies(boring_json_test boring_json_negative_tests)

  if(CONFIG_BORING_JSON_TEST_SANITIZERS)
    set(BORING_JSON_TEST_SANITIZER_FLAGS
        -fsanitize=address
        -fsanitize=undefined
        -fsanitize=leak)
    set(BORING_JSON_TEST_SANITIZER_COMPILE_FLAGS
        ${BORING_JSON_TEST_SANITIZER_FLAGS}
        -fno-omit-frame-pointer)
    string(JOIN " " BORING_JSON_TEST_SANITIZER_LINK_FLAGS ${BORING_JSON_TEST_SANITIZER_FLAGS})

    foreach(target boring_json unity boring_json_test)
      target_compile_options(${target} PRIVATE ${BORING_JSON_TEST_SANITIZER_COMPILE_FLAGS})
      set_property(TARGET ${target} APPEND_STRING PROPERTY LINK_FLAGS
                                                          " ${BORING_JSON_TEST_SANITIZER_LINK_FLAGS}")
    endforeach()
  endif()

endif()
