# Compatibility
CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

# Project name
PROJECT (jsoncpp)

# Project version
SET (${PROJECT_NAME}_MAJOR_VERSION 0)
SET (${PROJECT_NAME}_MINOR_VERSION 1)
SET (${PROJECT_NAME}_PATCH_LEVEL 0)

INCLUDE_DIRECTORIES ("${PROJECT_SOURCE_DIR}/jsoncpp/include")
INCLUDE_DIRECTORIES ("${PROJECT_SOURCE_DIR}/jsoncpp/src/lib_json")

SET(JSONCPP_INCLUDE_DIRECTORY "${PROJECT_SOURCE_DIR}/jsoncpp/src/lib_json" "${PROJECT_SOURCE_DIR}/jsoncpp/include")

SET (CMAKE_BUILD_TYPE Release)

FILE (GLOB SOURCES ./jsoncpp/src/lib_json/*.cpp)
FILE (GLOB HEADERS ./jsoncpp/src/lib_json/*.h ./jsoncpp/src/lib_json/*.inl ./jsoncpp/include/json/*.h)


SET (PROJECT_LIBRARY ${PROJECT_NAME})


ADD_LIBRARY (${PROJECT_LIBRARY} ${SOURCES} ${HEADERS})
