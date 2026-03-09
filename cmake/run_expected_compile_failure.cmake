if(NOT DEFINED COMPILER)
  message(FATAL_ERROR "COMPILER is required")
endif()

if(NOT DEFINED FIXTURE)
  message(FATAL_ERROR "FIXTURE is required")
endif()

if(NOT DEFINED C_STANDARD)
  message(FATAL_ERROR "C_STANDARD is required")
endif()

if(NOT DEFINED C_EXTENSIONS)
  message(FATAL_ERROR "C_EXTENSIONS is required")
endif()

if(NOT DEFINED INCLUDE_DIRS)
  message(FATAL_ERROR "INCLUDE_DIRS is required")
endif()

if(NOT DEFINED SCENARIO)
  message(FATAL_ERROR "SCENARIO is required")
endif()

if(NOT DEFINED OBJECT_FILE)
  message(FATAL_ERROR "OBJECT_FILE is required")
endif()

if(NOT DEFINED EXPECTED_TOKEN)
  message(FATAL_ERROR "EXPECTED_TOKEN is required")
endif()

if(NOT DEFINED STAMP_FILE)
  message(FATAL_ERROR "STAMP_FILE is required")
endif()

set(compile_command "${COMPILER}")

if(C_EXTENSIONS)
  list(APPEND compile_command "-std=gnu${C_STANDARD}")
else()
  list(APPEND compile_command "-std=c${C_STANDARD}")
endif()

foreach(include_dir IN LISTS INCLUDE_DIRS)
  list(APPEND compile_command "-I${include_dir}")
endforeach()

foreach(compile_definition IN LISTS COMPILE_DEFINITIONS)
  list(APPEND compile_command "-D${compile_definition}")
endforeach()

foreach(compile_option IN LISTS COMPILE_OPTIONS)
  list(APPEND compile_command "${compile_option}")
endforeach()

list(APPEND compile_command "-D${SCENARIO}" "-c" "${FIXTURE}" "-o" "${OBJECT_FILE}")

execute_process(
  COMMAND ${compile_command}
  RESULT_VARIABLE compile_result
  OUTPUT_VARIABLE compile_stdout
  ERROR_VARIABLE compile_stderr)

file(REMOVE "${OBJECT_FILE}")

if(compile_result EQUAL 0)
  file(REMOVE "${STAMP_FILE}")
  message(FATAL_ERROR
    "Negative compile scenario ${SCENARIO} compiled successfully.\n"
    "Expected compile failure for ${FIXTURE}.\n"
    "stdout:\n${compile_stdout}\n"
    "stderr:\n${compile_stderr}")
endif()

string(FIND "${compile_stderr}" "${EXPECTED_TOKEN}" expected_token_pos)

if(expected_token_pos EQUAL -1)
  file(REMOVE "${STAMP_FILE}")
  message(FATAL_ERROR
    "Negative compile scenario ${SCENARIO} failed for an unexpected reason.\n"
    "Expected diagnostic token: ${EXPECTED_TOKEN}\n"
    "stdout:\n${compile_stdout}\n"
    "stderr:\n${compile_stderr}")
endif()

file(WRITE "${STAMP_FILE}" "${SCENARIO}\n")
