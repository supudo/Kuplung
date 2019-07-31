# additional target to perform cppcheck run, requires cppcheck
# get all project files
# HACK this workaround is required to avoid qml files checking ^_^

FILE(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h *.hpp)
SET(PROJECT_TRDPARTY_DIR ${PROJECT_SOURCE_DIR}/include)
FOREACH(SOURCE_FILE ${ALL_SOURCE_FILES})
  STRING(FIND ${SOURCE_FILE} ${PROJECT_TRDPARTY_DIR} PROJECT_TRDPARTY_DIR_FOUND)
  IF(NOT ${PROJECT_TRDPARTY_DIR_FOUND} EQUAL -1)
    LIST(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
  ENDIF()
  IF(${SOURCE_FILE} MATCHES "/stb_*")
    LIST(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
  ENDIF()
  IF(${SOURCE_FILE} MATCHES "/imgui*")
    LIST(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
  ENDIF()
  IF(${SOURCE_FILE} MATCHES "/saveopen/Kuplung*")
    LIST(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
  ENDIF()
  IF(${SOURCE_FILE} MATCHES "/oui*")
    LIST(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
  ENDIF()
  IF(${SOURCE_FILE} MATCHES "/nanovg*")
    LIST(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
  ENDIF()
  IF(${SOURCE_FILE} MATCHES "/json*")
    LIST(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
  ENDIF()
  IF(${SOURCE_FILE} MATCHES "/cpp-base64*")
    LIST(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
  ENDIF()
  IF(${SOURCE_FILE} MATCHES "/catch*")
    LIST(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
  ENDIF()
ENDFOREACH()

ADD_CUSTOM_TARGET(cppcheck
  COMMAND
  /usr/local/bin/cppcheck
  --enable=warning,performance,portability,information,missingInclude
  --template="[{severity}][{id}] {message} {callstack}"
  --std=c++14
  --language=c++
  --verbose
  --force
  --inconclusive
  --inline-suppr
  -I ${CMAKE_SOURCE_DIR}
  --quiet ${ALL_SOURCE_FILES}
  --output-file=${CMAKE_SOURCE_DIR}/cppcheck_results.log
)
