cmake_minimum_required(VERSION 3.27)

project(rchol)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#############################################
# NOTE!
#
#   Any library that consumes RChol currently
#   needs to include the following lines in
#   its CMakeLists.txt before its call to
#   find_package(RChol REQUIRED) ...
#
#   message(STATUS "Checking for SuiteSparse::AMD ... ")
#   find_package(AMD REQUIRED)
#   include_directories(${AMD_INCLUDE_DIRS})
#   message(STATUS "AMD include directories: ${AMD_INCLUDE_DIRS}")
#   message(STATUS "Done checking for SuiteSparse::AMD.")
#
# TODO: update this build infrastructure so that isn't
# necessary.
#
#############################################

#############################################
# Main library 
#############################################

file(GLOB_RECURSE ALL_RChol_LIBRARY_SOURCES "rchol/*.cpp" "rchol_lap/*.cpp" "util/*.cpp" "csr/*.cpp")

add_library(RChol STATIC ${ALL_RChol_LIBRARY_SOURCES})  # Change to SHARED if you want a shared library
if(DEFINED TBBROOT)
    target_link_directories(RChol PRIVATE ${TBBROOT}/lib/intel64/gcc4.8/)
endif()
target_include_directories(RChol PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/rchol>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/rchol_lap>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/util>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/csr>
)
find_package(AMD REQUIRED)
target_link_libraries(RChol PRIVATE mkl_intel_ilp64 mkl_tbb_thread mkl_core pthread m SuiteSparse::AMD dl)
# ^ For reasons that I don't understand, we need to find_package(AMD), but link_libraries(SuiteSparse::AMD) ...

# Check for METIS
if(DEFINED METIS_INC AND DEFINED METIS_LIB)
    message(STATUS "METIS paths specified. Including directories and linking libraries.")
    find_library(METIS_LIBRARIES NAMES metis PATHS ${METIS_LIB} NO_DEFAULT_PATH)
    target_link_libraries(RChol PUBLIC ${METIS_LIBRARIES})
    target_include_directories(RChol PUBLIC ${METIS_INC})
else()
    message(WARNING "METIS not found. RChol will not link against METIS.")
endif()

# Install rules for the RChol library (optional)
install(TARGETS RChol
    EXPORT RCholTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    INCLUDES DESTINATION ${CMAKE_INSTALL_PREFIX}/include/RChol
)

install(EXPORT RCholTargets
    FILE RCholTargets.cmake
    NAMESPACE RChol::
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/RChol
)

file(GLOB_RECURSE HEADER_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/rchol/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/rchol_lap/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/util/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/csr/*.hpp"
)
install(FILES ${HEADER_FILES}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/RChol
)

include(CMakePackageConfigHelpers)

# Set the installation directory for the config files
set(RChol_CONFIG_DIR "${CMAKE_INSTALL_PREFIX}/lib/cmake/RChol")

# Configure the package config file
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/RCholConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/RCholConfig.cmake"
    INSTALL_DESTINATION "${RChol_CONFIG_DIR}"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# Install the config file
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/RCholConfig.cmake"
    DESTINATION "${RChol_CONFIG_DIR}"
)

#############################################
# Executables
#############################################

# add_executable(driver ex_laplace.cpp)
# target_compile_options(driver PRIVATE -Wall -O3)
# target_link_libraries(driver RChol)

# Check for METIS for driver_parallel
if(DEFINED METIS_INC AND DEFINED METIS_LIB)
    add_executable(driver_parallel ex_parallel.cpp)
    target_compile_options(driver_parallel PRIVATE -Wall -O3)
    target_link_libraries(driver_parallel RChol)
else()
    message(WARNING "Cannot build driver_parallel without METIS.")
endif()
