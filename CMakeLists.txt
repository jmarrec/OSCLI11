cmake_minimum_required(VERSION 3.17.0)
# cmake_policy(SET CMP0048 NEW)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Do not enable compiler specific extensions, for eg on GCC use -std=c++1z (=c++17) and not -std=gnu++17
set(CMAKE_CXX_EXTENSIONS OFF)

# Use ccache is available, has to be before "project()"
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  # Support Unix Makefiles and Ninja
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

project(os-cli11
  LANGUAGES CXX
  VERSION 0.0.1
)

# The PROJECT_SOURCE_DIR here is root of this repo, which we will use instead of using CMAKE_SOURCE_DIR
set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
# We will use BINARY_DIR instead of CMAKE_BINARY_DIR
set(PROJECT_BINARY_DIR "${CMAKE_BINARY_DIR}")

#MESSAGE(FATAL_ERROR "CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}, BINARY_DIR=${BINARY_DIR}, PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}, CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}, CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}")

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

#if(POLICY CMP0020)
  #cmake_policy(SET CMP0020 NEW)
#endif()
#if(POLICY CMP0022)
  #cmake_policy(SET CMP0022 NEW)
#endif()
#if(POLICY CMP0042)
  #cmake_policy(SET CMP0042 NEW)
#endif()
#if(POLICY CMP0054)
  #cmake_policy(SET CMP0054 NEW)
#endif()
#if(POLICY CMP0077)
  #cmake_policy(SET CMP0077 NEW)
#endif()

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

###############################################################################
#                                  N I N J A                                  #
###############################################################################
# Enhance error reporting and compiler messages
if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  if(WIN32)
    # On Windows cuda nvcc uses cl and not clang
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-fcolor-diagnostics> $<$<COMPILE_LANGUAGE:CXX>:-fcolor-diagnostics>)
  else()
    add_compile_options(-fcolor-diagnostics)
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(WIN32)
    # On Windows cuda nvcc uses cl and not gcc
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-fdiagnostics-color=always>
                        $<$<COMPILE_LANGUAGE:CXX>:-fdiagnostics-color=always>)
  else()
    add_compile_options(-fdiagnostics-color=always)
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND MSVC_VERSION GREATER 1900)
  add_compile_options(/diagnostics:column)
else()
  message(STATUS "No colored compiler diagnostic set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
endif()

# Enable runtime checking features: TSAN, ASAN, UBSAN
message("CMAKE_CXX_COMPILER_ID=${CMAKE_CXX_COMPILER_ID}")
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

  option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" ON)
  if(ENABLE_COVERAGE)
    target_compile_options(project_options INTERFACE --coverage -O0 -g)
    target_link_libraries(project_options INTERFACE --coverage)
  endif()
endif()

###############################################################################
#                         C M A K E    C O N T R O L                          #
###############################################################################

# High level project configuration
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/Products")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/Products")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/Products")

set(LIBRARY_SEARCH_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug")

###############################################################################
#          C O M P I L E R    A N D    S Y S T E M    O P T I O N S           #
###############################################################################

# Compiler and system specific options
if(UNIX)

  # all warnings
  target_compile_options(project_options INTERFACE -fPIC -fno-strict-aliasing -Winvalid-pch)
  # Treat all warnings as errors, extra errors, and be pedantic
  target_compile_options(project_options INTERFACE -Wall -Wextra -Werror -pedantic-errors -pedantic) # Turn on warnings
  if(APPLE)
     target_compile_options(project_options INTERFACE -Wno-overloaded-virtual -ftemplate-depth=1024)
  endif()

  # Note: CMAKE_CXX_STANDARD set to 20 should already take care of adding -std=c++20 or equivalent
elseif(WIN32)
  target_compile_options(project_options INTERFACE /utf-8)
endif()

#######################################################################
#                              C O N A N                              #
#######################################################################

set(CMAKE_CONAN_EXPECTED_HASH
    5cdb3042632da3efff558924eecefd580a0e786863a857ca097c3d1d43df5dcd)
set(CMAKE_CONAN_VERSION "0.18.1")

if(EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  file(SHA256 "${CMAKE_BINARY_DIR}/conan.cmake" CMAKE_CONAN_HASH)
elseif(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake"
       OR NOT "${CMAKE_CONAN_HASH}" MATCHES "${CMAKE_CONAN_EXPECTED_HASH}")
  # Put it in CMAKE_BINARY_DIR so we don't end up with two when building OpenStudioApplication
  message(
    STATUS
      "Downloading conan.cmake ${CMAKE_CONAN_VERSION} from https://github.com/conan-io/cmake-conan"
  )
  file(
    DOWNLOAD
    "https://raw.githubusercontent.com/conan-io/cmake-conan/${CMAKE_CONAN_VERSION}/conan.cmake"
    "${CMAKE_BINARY_DIR}/conan.cmake"
    EXPECTED_HASH SHA256=${CMAKE_CONAN_EXPECTED_HASH}
    TLS_VERIFY ON)
else()
  message(STATUS "Using existing conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_check(VERSION 1.43.0 REQUIRED)

message(STATUS "RUNNING CONAN")

list(APPEND CONAN_BUILD "missing")

if(CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CONAN_CMAKE_MULTI BUILDIRSTATUS_RESOURCES_HXXBUILDIRSTATUS_RESOURCES_HXXON)
  if (NOT CONAN_CONFIGURATION_TYPES)
    # Cache it, and let user change it (if they want to pull RelWithDebInfo for eg)
    set(CONAN_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "List of configurations for which you want to fetch conan packages")
    mark_as_advanced(CONAN_CONFIGURATION_TYPES)
  endif()
  message(STATUS "Conan: Using cmake_multi generator")
  set(CONAN_GENERATOR "cmake_find_package_multi")
else()
  message(STATUS "Conan: Using cmake generator")
  set(CONAN_CMAKE_MULTI OFF)
  set(CONAN_GENERATOR "cmake_find_package")
  set(CONAN_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})
endif()

message(STATUS "Conan: conan_cmake_configure")
# This will create the conanfile.txt
conan_cmake_configure(
  REQUIRES
  "fmt/9.1.0#811e918ca4b4e0b9ddd6d5a2883efa82"
  "gtest/1.11.0#8aca975046f1b60c660ee9d066764d69"
  #"cli11/2.2.0#33cd38722fa134b15ae308dfb4e6c942"
  "cli11/2.3.2#8ccdf14fb1ad53532d498c16ae580b4b"
  GENERATORS ${CONAN_GENERATOR}
)

foreach(build_type ${CONAN_CONFIGURATION_TYPES})
  conan_cmake_autodetect(settings BUILD_TYPE ${build_type})
  message(STATUS "Conan: Autodetected settings for build type ${build_type}: ${settings}")

  # Avoid polluting with cppstd which prevents downloading some existing binary packages (like boost)
  # Former deprecated conan_cmake_run was NOT adding compiler.cppstd
  foreach(ARG ${settings})
    if(${ARG} MATCHES "compiler.cppstd=(.*)")
      message("Removing ${ARG}")
      list(REMOVE_ITEM settings ${ARG})
    endif()
  endforeach()

  conan_cmake_install(PATH_OR_REFERENCE .
    BUILD ${CONAN_BUILD}
    OPTIONS ${CONAN_OPTIONS}
    SETTINGS ${settings}
    ${CONAN_UPDATE}
  )
endforeach()

# Loads the conanbuildinfo.cmake / conanbuildinfo_multi.cmake
# conan_load_buildinfo()
# conan_basic_setup in the conanbuildinfo.cmake. TARGETS => set cmake targets, NO_OUTPUT_DIRS => Don't modify the BIN / LIB folders etc
#conan_basic_setup(TARGETS NO_OUTPUT_DIRS)
# conan_basic_setup()

message("CONAN_LIBS=${CONAN_LIBS}")

# Search first in the binary dir, where conan will install finders, then
# search for modules in the root dir to override cmake ones
# Start with ROOT, then PROJECT_BINARY_DIR
list(APPEND CMAKE_MODULE_PATH "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/CMake")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

find_package(fmt REQUIRED)
find_package(CLI11 REQUIRED)
find_package(GTest REQUIRED)

###############################################################################
#                              E X E C U T A B L E                            #
###############################################################################

option(INCLUDE_WHAT_YOU_USE OFF)

set(IWYU_GLOBAL TRUE)

# Enable globally:
if (INCLUDE_WHAT_YOU_USE AND IWYU_GLOBAL)
  find_program(iwyu_path NAMES include-what-you-use iwyu REQUIRED)
  set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path} -Xiwyu
      --mapping_file=${CMAKE_CURRENT_SOURCE_DIR}/iwyu.imp
  )
  message("CMAKE_CXX_INCLUDE_WHAT_YOU_USE=${CMAKE_CXX_INCLUDE_WHAT_YOU_USE}")
endif()

find_program(iwyu_path NAMES include-what-you-use iwyu)

include_directories(${PROJECT_BINARY_DIR})

add_executable(os-cli11
  src/main.cpp

  src/Filesystem.hpp
  src/RunCommand.hpp
  src/RunCommand.cpp
)
target_link_libraries(os-cli11
  PRIVATE
  project_options
  fmt::fmt
  CLI11::CLI11
)

add_executable(mcve
  src/mcve.cpp
  src/Filesystem.hpp
)
target_link_libraries(mcve
  PRIVATE
  project_options
  fmt::fmt
  CLI11::CLI11
)

###############################################################################
#                                 T E S T I N G                               #
###############################################################################

# set(TEST_DIR_PATH "${PROJECT_SOURCE_DIR}/test")
# configure_file("${TEST_DIR_PATH}/resources.hxx.in" "${PROJECT_BINARY_DIR}/src/resources.hxx")
#
# add_executable(tests
#   test/BuildDirectoryInfo_GTest.cpp
#   test/TimeUtilities_GTest.cpp
#   ${PROJECT_BINARY_DIR}/src/resources.hxx
# )
# target_link_libraries(tests
#   PRIVATE
#   project_options
#   build_dir_status_lib
#   GTest::GTest
# )
#
# enable_testing()
#
# include(GoogleTest)
# gtest_discover_tests(tests
#   # WORKING_DIRECTORY Testing
# )

###############################################################################
#                              P A C K A G I N G                              #
###############################################################################


if (APPLE)
  set(CPACK_IFW_TARGET_DIRECTORY /Applications/os-cli11-${PROJECT_VERSION}/)
elseif(UNIX)

  # For debug
  # set(CPACK_DEBIAN_PACKAGE_DEBUG ON)

  set(CPACK_DEBIAN_PACKAGE_DEPENDS "")

  # These two will set the .deb install path correctly
  set(CPACK_SET_DESTDIR ON)
  set(CPACK_DEBIAN_PACKAGE_NAME "os-cli11-${PROJECT_VERSION}")

  set(CPACK_INSTALL_PREFIX /usr/local/${CPACK_DEBIAN_PACKAGE_NAME})
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Julien Marrec")

elseif(WIN32)
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "os-cli11-${PROJECT_VERSION}")
  set(CPACK_IFW_TARGET_DIRECTORY "C:/${CPACK_PACKAGE_INSTALL_DIRECTORY}")
  set(CPACK_BINARY_IFW ON CACHE BOOL "Enable to build IFW packages")
endif()

include(CPack)
include(CPackIFW)

cpack_add_component(Executable
  DISPLAY_NAME "os-cli11"
  DESCRIPTION "os-cli11 Exe"
)

install(TARGETS os-cli11 DESTINATION bin COMPONENT Executable)
