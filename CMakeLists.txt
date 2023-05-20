cmake_minimum_required(VERSION 3.25)
project(AMETest)

set(CMAKE_CXX_STANDARD 17)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(median_filter main.cpp)

target_link_libraries(median_filter  PRIVATE ${OpenCV_LIBS})
