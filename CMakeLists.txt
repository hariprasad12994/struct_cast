project(struct_cast)
cmake_minimum_required(VERSION 3.20)

set(SINGLE_HEADER_GENERATOR "AMALGAM" CACHE STRING "Single header generator, choices: AMALGAM, QUOM")

# Add Catch2
Include(FetchContent)
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.x
)
FetchContent_MakeAvailable(catch2)


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

# Set the paths
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SINGLE_HEADER_DIR "${CMAKE_SOURCE_DIR}/single_header")
set(TEST_DIR "${CMAKE_SOURCE_DIR}/test")
# Define the input and output for quom
set(INPUT_HEADER "${INCLUDE_DIR}/struct_cast.hpp")
set(OUTPUT_HEADER "${SINGLE_HEADER_DIR}/struct_cast.hpp")
# Get all header files in the include directory
file(GLOB_RECURSE HEADER_FILES "${INCLUDE_DIR}/*.hpp")

if(SINGLE_HEADER_GENERATOR STREQUAL "AMALGAM")
  add_custom_command(
    OUTPUT ${OUTPUT_HEADER}
    COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/amalgam.py ${INCLUDE_DIR} ${INPUT_HEADER} ${OUTPUT_HEADER}
    DEPENDS ${HEADER_FILES}
    COMMENT "Generating single header using amalgam"
    VERBATIM
  )
elseif(SINGLE_HEADER_GENERATOR STREQUAL "QUOM")
  # Custom command to run quom
  add_custom_command(
    OUTPUT ${OUTPUT_HEADER}
    COMMAND python -m quom ${INPUT_HEADER} ${OUTPUT_HEADER}
    DEPENDS ${HEADER_FILES}
    COMMENT "Generating single header using quom"
    VERBATIM
  )
else()
  message(FATAL_ERROR "Invalid SINGLE_HEADER_GENERATOR value. Use QUOM or AMALGAM")
endif()

# Custom target that depends on the generated header
add_custom_target(generate_single_header ALL DEPENDS ${OUTPUT_HEADER})

# Add the test executable
add_executable(test_struct_cast ${TEST_DIR}/test_struct_cast.cpp)
target_link_libraries(test_struct_cast PRIVATE Catch2::Catch2)
target_include_directories(test_struct_cast PRIVATE ${catch2_SOURCE_DIR}/single_include)

# temporary tests with out catch for easier debugging in gdb
# add_executable(test_temp ${TEST_DIR}/test_temp.cpp)
# target_include_directories(test_temp PRIVATE ${catch2_SOURCE_DIR}/single_include)
