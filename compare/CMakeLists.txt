set(OPENCV_INC_DIR "D:/lib/opencv/build/include")
set(OPENCV_LIB_DIR "D:/lib/opencv/build/x64/vc14/lib")
set(OPENCV_LIBS_DEBUG "opencv_world320d.lib")
set(OPENCV_LIBS_RELEASE "opencv_world320.lib")

cmake_minimum_required(VERSION 3.4)
project(Compare)

set(srcs main.cpp compare.cpp)

include_directories("${OPENCV_INC_DIR}")
link_directories("${OPENCV_LIB_DIR}")

add_executable (Compare ${srcs})
target_link_libraries(Compare debug ${OPENCV_LIBS_DEBUG})
target_link_libraries(Compare optimized ${OPENCV_LIBS_RELEASE})

install (TARGETS Compare DESTINATION "${PROJECT_SOURCE_DIR}/../")
