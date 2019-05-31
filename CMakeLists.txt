cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

project(AIDEN)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
else()
    set(MACOSX FALSE)
endif() #${CMAKE_SYSTEM_NAME} MATCHES "Darwin"

file(GLOB_RECURSE SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
  ${CMAKE_BINARY_DIR}/src/*.c)

file(GLOB_RECURSE INC_FILES #unused rn
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

#TODO: add library header directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/headers)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/headers)
include_directories(${CMAKE_BINARY_DIR}/include)

add_executable(AIDEN ${SRC_FILES})

if(MACOSX)
  #link_directories("/usr/local/lib")
  message("This is running on OSX")
  target_link_libraries(AIDEN /usr/local/lib/libtensorflow.so)
elseif(WIN32)
  #link_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/win/")
  target_link_libraries(AIDEN ${CMAKE_CURRENT_SOURCE_DIR}/libs/win/tensorflow.lib)
endif()

target_link_libraries(AIDEN OpenGL32)

#kinda doesn't ever work from my experience - ethan
file(COPY "res" DESTINATION "${CMAKE_BINARY_DIR}")
