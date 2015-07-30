# Copyright (c) 2015 Thomas Heller
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

macro(add_hpx_compile_test category name)
  set(options FAILURE_EXPECTED)
  set(one_value_args SOURCE_ROOT FOLDER)
  set(multi_value_args SOURCES)

  cmake_parse_arguments(${name} "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

  set(expected FALSE)
  set(exclude_from_all)

  if(${name}_FAILURE_EXPECTED)
    set(expected TRUE)
    set(exclude_from_all EXCLUDE_FROM_ALL EXCLUDE_FROM_DEFAULT_BUILD)
  endif()

  add_hpx_executable(
    ${name}
    SOURCE_ROOT ${${name}_SOURCE_ROOT}
    SOURCES ${${name}_SOURCES}
    ${exclude_from_all}
    FOLDER ${${name}_FOLDER})

  add_test(NAME "${category}.${name}"
    COMMAND ${CMAKE_COMMAND}
      --build .
      --target ${name}_exe
      --config $<CONFIGURATION>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

  if(expected)
    set_tests_properties("${category}.${name}" PROPERTIES WILL_FAIL TRUE)
  endif()

endmacro()

macro(add_hpx_unit_compile_test category name)
  add_hpx_compile_test("tests.unit.${category}" ${name} ${ARGN})
endmacro()

macro(add_hpx_regression_compile_test category name)
  add_hpx_compile_test("tests.regressions.${category}" ${name} ${ARGN})
endmacro()

