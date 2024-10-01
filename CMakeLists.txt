# Keep in mind that the build files are generated with the command "ESP-IDF: Set
# Espressif Device Target" of ESP VSCode extension.

# The following lines of boilerplate have to be in your project's CMakeLists in
# this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME TrialExecutable)
# PROJECT_VER is set to the first line of 'version.txt', for more details:
# https://docs.espressif.com/projects/esp-idf/en/release-v3.3/api-guides/build-system-cmake.html#preset-component-variables
# set(PROJECT_VER_NUMBER 1)

# Count commits on the main branch
execute_process(COMMAND git rev-list --first-parent --count origin/main --
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE main_count
                OUTPUT_STRIP_TRAILING_WHITESPACE)

# Count commits on the develop branch
execute_process(COMMAND git rev-list --first-parent --count origin/develop --
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE develop_count
                OUTPUT_STRIP_TRAILING_WHITESPACE)

# Add the two counts together
math(EXPR total_count "${main_count} + ${develop_count}")

# Set the PROJECT_VER_NUMBER variable
set(PROJECT_VER_NUMBER ${total_count})

set(SDKCONFIG "${CMAKE_BINARY_DIR}/sdkconfig")

# This should be done before using the IDF_TARGET variable.
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

include(${CMAKE_SOURCE_DIR}/components.cmake)

# This is NOT a CMake function! It is a ESP macro which initializes the project
# (see esp-idf/tools/cmake/project.cmake)
project(${PROJECT_NAME})

message("Project version number is: ${PROJECT_VER_NUMBER}")
message("Project version string is: ${PROJECT_VER}")

if(CONFIG_IDF_TARGET_ESP32C2)
  include(relinker)
endif()

idf_build_set_property(CXX_COMPILE_OPTIONS
                       "-std=gnu++20;-Os;-DCHIP_HAVE_CONFIG_H" APPEND)
idf_build_set_property(C_COMPILE_OPTIONS "-Os" APPEND)
# For RISCV chips, project_include.cmake sets -Wno-format, but does not clear
# various flags that depend on -Wformat
idf_build_set_property(COMPILE_OPTIONS
                       "-Wno-format-nonliteral;-Wno-format-security" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-fdiagnostics-color=always" APPEND)

# Link all GE libraries to the project. Do not move this line because otherwise
# it generates build errors in the matter SDK or it uses overwritten CMake
# functions which ends in weird behavior.
add_subdirectory(apps)
# Add libraries to link here
target_link_libraries(${PROJECT_NAME}.elf PRIVATE ${USED_LIBRARIES})

# Add directory paths of libraries required by the files in main folder to
# PRIV_INCLUDE_DIRS list in main/CMakeLists.txt file.
