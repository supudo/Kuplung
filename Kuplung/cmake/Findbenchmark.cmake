# Findbenchmark.cmake
# - Try to find benchmark
#
# The following variables are optionally searched for defaults
#  benchmark_ROOT_DIR:  Base directory where all benchmark components are found
#
# Once done this will define
#  benchmark_FOUND - System has benchmark
#  benchmark_INCLUDE_DIRS - The benchmark include directories
#  benchmark_LIBRARIES - The libraries needed to use benchmark

set(GBenchmark_ROOT_DIR "" CACHE PATH "Folder containing benchmark")

find_path(GBenchmark_INCLUDE_DIR "benchmark/benchmark.h"
  PATHS ${benchmark_ROOT_DIR}
  PATH_SUFFIXES include
  NO_DEFAULT_PATH)
find_path(GBenchmark_INCLUDE_DIR "benchmark/benchmark.h")

find_library(GBenchmark_LIBRARY NAMES "benchmark"
  PATHS ${benchmark_ROOT_DIR}
  PATH_SUFFIXES lib lib64
  NO_DEFAULT_PATH)
find_library(GBenchmark_LIBRARY NAMES "benchmark")

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set benchmark_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GBenchmark FOUND_VAR GBenchmark_FOUND
  REQUIRED_VARS GBenchmark_LIBRARY
  GBenchmark_INCLUDE_DIR)

if(GBenchmark_FOUND)
  set(GBenchmark_LIBRARIES ${benchmark_LIBRARY})
  set(GBenchmark_INCLUDE_DIRS ${benchmark_INCLUDE_DIR})
endif()

mark_as_advanced(GBenchmark_INCLUDE_DIR GBenchmark_LIBRARY)
