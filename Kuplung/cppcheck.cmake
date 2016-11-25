# additional target to perform cppcheck run, requires cppcheck
# get all project files
# HACK this workaround is required to avoid qml files checking ^_^

FILE(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h)
SET(PROJECT_TRDPARTY_DIR ${PROJECT_SOURCE_DIR}/include)
FOREACH(SOURCE_FILE ${ALL_SOURCE_FILES})
  STRING(FIND ${SOURCE_FILE} ${PROJECT_TRDPARTY_DIR} PROJECT_TRDPARTY_DIR_FOUND)
  IF(NOT ${PROJECT_TRDPARTY_DIR_FOUND} EQUAL -1)
    LIST(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
  ENDIF()
  IF(${SOURCE_FILE} MATCHES "/stb_*/")
    LIST(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
  ENDIF()
  IF(${SOURCE_FILE} MATCHES "/imgui*/")
    LIST(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
  ENDIF()
  IF(${SOURCE_FILE} MATCHES "/noiseutil*/")
    LIST(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
  ENDIF()
ENDFOREACH()

ADD_CUSTOM_TARGET(cppcheck
                  COMMAND
                  /usr/local/bin/cppcheck
                  --enable=warning,performance,portability,information,missingInclude
                  --std=c++11
                  --library=qt.cfg
                  --template="[{severity}][{id}] {message} {callstack} \\n"
                  --verbose
                  --quiet ${ALL_SOURCE_FILES}
)
