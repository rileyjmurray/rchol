cmake_minimum_required(VERSION 3.27)

project(rchol)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#############################################
# Main library 
#############################################

file(GLOB_RECURSE ALL_RChol_LIBRARY_SOURCES "sparse.cpp" "rchol/*.cpp" "rchol_lap/*.cpp" "util/*.cpp")

add_library(RChol STATIC ${ALL_RChol_LIBRARY_SOURCES})  # Change to SHARED if you want a shared library
if(DEFINED TBBROOT)
    target_link_directories(RChol PUBLIC ${TBBROOT}/lib/intel64/gcc4.8/)
endif()
target_include_directories(RChol PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/rchol>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/rchol_lap>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/util>
)
target_link_libraries(RChol PUBLIC mkl_intel_ilp64 mkl_tbb_thread mkl_core pthread m dl)

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
    "${CMAKE_CURRENT_SOURCE_DIR}/sparse.hpp"
)
install(FILES ${HEADER_FILES}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/RChol
)

#############################################
# Executables
#############################################

add_executable(driver ex_laplace.cpp)
target_compile_options(driver PRIVATE -Wall -O3)
target_link_libraries(driver RChol)

# Check for METIS for driver_parallel
if(DEFINED METIS_INC AND DEFINED METIS_LIB)
    add_executable(driver_parallel ex_laplace_parallel.cpp)
    target_compile_options(driver_parallel PRIVATE -Wall -O3)
    target_link_libraries(driver_parallel RChol)
else()
    message(WARNING "Cannot build driver_parallel without METIS.")
endif()
