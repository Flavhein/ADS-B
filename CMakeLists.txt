# Link with the "Threads library (required to link with AFF3CT after)
cmake_minimum_required(VERSION 3.15) 
project(ads_b)  

set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(PYBIND11_NEWPYTHON ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

execute_process(
    COMMAND python3 -c "
import streampu
import os

streampu_path = os.path.dirname(streampu.__file__)
include_dir = os.path.join(streampu_path, 'include')
lib_dir = os.path.join(streampu_path, 'lib')
print(f'{include_dir};{lib_dir}')
"
    OUTPUT_VARIABLE STREAMPU_PATHS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

list(GET STREAMPU_PATHS 0 STREAMPU_INCLUDE_DIR)
list(GET STREAMPU_PATHS 1 STREAMPU_LIB_DIR)
#message(STATUS "Include dir: ${STREAMPU_INCLUDE_DIR}")
#message(STATUS "Lib dir: ${STREAMPU_LIB_DIR}")

include_directories(${STREAMPU_INCLUDE_DIR})
link_directories(${STREAMPU_LIB_DIR})

include_directories(${CMAKE_SOURCE_DIR}/lib/include)
include_directories(${STREAMPU_INCLUDE_DIR})

find_package(pybind11 CONFIG REQUIRED )
find_package(Threads REQUIRED)
find_package(cpptrace REQUIRED)
find_package(streampu REQUIRED)
#find_package(pystreampu REQUIRED)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/pybind11/)
pybind11_add_module(ads_b MODULE ${SOURCES})

set_target_properties(ads_b PROPERTIES
                      OUTPUT_NAME ads_b
                      POSITION_INDEPENDENT_CODE ON) # set -fpic

target_link_libraries     (ads_b PRIVATE spu::spu-static-lib)
target_include_directories(ads_b PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_library(STREAMPU_LIBRARY NAMES streampu PATHS ${STREAMPU_LIB_DIR})
if(STREAMPU_LIBRARY)
    target_link_libraries(ads_b PRIVATE ${STREAMPU_LIBRARY})
else()
    message(FATAL_ERROR "La librairie libstreampu.a n'a pas été trouvée dans ${STREAMPU_LIB_DIR}")
endif()
