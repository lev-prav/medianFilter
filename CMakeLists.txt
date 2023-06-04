cmake_minimum_required(VERSION 3.22)
project(AMETest)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(median_filter main.cpp src/MedianFilter.cpp src/MedianFilter.h src/Histogram.h)

target_link_libraries(median_filter  PRIVATE ${OpenCV_LIBS}
                                                pthread)
