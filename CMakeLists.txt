cmake_minimum_required(VERSION 3.1)
project(EventEngine)

set(CMAKE_CXX_STANDARD 14)


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/out)

include(CMakeLists-mylogger.txt)


add_library(eventLibrary STATIC ${PROJECT_SOURCE_DIR}/src/event.cpp 
                                ${PROJECT_SOURCE_DIR}/src/rule.cpp
                                ${PROJECT_SOURCE_DIR}/src/eventengine.cpp)

target_link_libraries(eventLibrary PUBLIC PocoFoundation mylogger)
target_include_directories(eventLibrary PUBLIC "${PROJECT_SOURCE_DIR}/include")

add_executable(event ${PROJECT_SOURCE_DIR}/test/main.cpp)
target_link_libraries(event eventLibrary)
