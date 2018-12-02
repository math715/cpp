PROJECT(detect)
CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

# if(COMMAND cmake_policy)　　　　　　　　　　　　
#      cmake_policy(SET CMP0003 NEW)
# endif(COMMAND cmake_policy)

FIND_PACKAGE( OpenCV REQUIRED )

ADD_EXECUTABLE(OpenCV_Example  detect.cc)

TARGET_LINK_LIBRARIES(OpenCV_Example ${OpenCV_LIBS})
