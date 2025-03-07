cmake_minimum_required(VERSION 3.29)

# Set Cxx standard
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED On)
set (CMAKE_CXX_EXTENSIONS Off)


project (Benchy)



# TEMPORARY STUFF
add_library (
    benchy_lib
    "${CMAKE_CURRENT_SOURCE_DIR}/benchy/benchy.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchy/benchy.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/benchy/dispatcher.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchy/dispatcher.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/benchy/time.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchy/time_fitter.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchy/timers.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchy/argument_providers.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/benchy/series.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchy/series.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/benchy/benchmark_context.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchy/benchmark_context.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/benchy/benchmark.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchy/benchmark.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/benchy/exporter.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchy/exporter.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/benchy/entry.cpp"
)

target_include_directories (benchy_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")



add_executable (
    client
    "${CMAKE_CURRENT_SOURCE_DIR}/client/main.cpp"
)

target_include_directories (client PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries (client PRIVATE benchy_lib)