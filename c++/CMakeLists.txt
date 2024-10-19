cmake_minimum_required(VERSION 3.27)

project(rchol)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#############################################
# Main library 
#############################################

file(GLOB_RECURSE ALL_RChol_LIBRARY_SOURCES "sparse.cpp" "rchol/*.cpp" "rchol_lap/*.cpp" "util/*.cpp")
include_directories( rchol rchol_lap util )

# Add TBB malloc proxy if TBBROOT is defined
if(DEFINED TBBROOT)
    link_directories(${TBBROOT}/lib/intel64/gcc4.8/)
    set(TBB_MALLOC_PROXY_LIBRARIES tbbmalloc_proxy)
endif()

#add_library(RChol INTERFACE)
#set(RChol_REQUIRED_LIBRARIES mkl_intel_ilp64 mkl_tbb_thread mkl_core pthread m dl)
#target_link_libraries(RChol INTERFACE ${TBB_MALLOC_PROXY_LIBRARIES} ${RChol_REQUIRED_LIBRARIES} )

#install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/../RandBLAS.hh"
#  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
#install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
#  DESTINATION include FILES_MATCHING PATTERN "*.hh")

#install(TARGETS RandBLAS EXPORT RandBLAS
#  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RandBLAS
#  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

#install(EXPORT RandBLAS DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
#  EXPORT_LINK_INTERFACE_LIBRARIES)


#############################################
# Executables
#############################################

set(DRIVER_SOURCES "ex_laplace.cpp" ${ALL_RChol_LIBRARY_SOURCES})
list(REMOVE_ITEM DRIVER_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/rchol/find_separator.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/rchol/rchol_parallel.cpp"
)
add_executable(driver ${DRIVER_SOURCES})
message(STATUS "DRIVER_SOURCES: ${DRIVER_SOURCES}")
target_compile_options(driver PRIVATE -Wall -O3)
target_link_libraries(driver ${TBB_MALLOC_PROXY_LIBRARIES} mkl_intel_ilp64 mkl_tbb_thread mkl_core pthread m dl)


# Check for METIS
if(DEFINED METIS_INC AND DEFINED METIS_LIB)
    message(STATUS "METIS found. Including directories and linking libraries.")
    include_directories(${METIS_INC})
    link_directories(${METIS_LIB})
    set(METIS_LIBRARIES metis)

    set(DRIVER_PARALLEL_SOURCES "ex_laplace_parallel.cpp" ${ALL_RChol_LIBRARY_SOURCES})
    add_executable(driver_parallel ${DRIVER_PARALLEL_SOURCES})
    target_compile_options(driver_parallel PRIVATE -Wall -O3)
    target_link_libraries(driver_parallel ${METIS_LIBRARIES} ${TBB_MALLOC_PROXY_LIBRARIES} mkl_intel_ilp64 mkl_tbb_thread mkl_core pthread m dl)
else()
    message(WARNING "METIS not found. Excluding certain source files.")
    # If METIS is not found, exclude certain source files
endif()


