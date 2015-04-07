cmake_minimum_required(VERSION 2.6)
project(tests)

file(GLOB SRC *.cpp)
file(GLOB HDR *.h)
file(GLOB INCLUDES ../include/Pal/*.h)

include_directories("../gmock-1.7.0/include"
                    "../gmock-1.7.0/gtest/include"
                    "../include")



add_executable(tests ${SRC} ${HDR} ${INCLUDES})

list( APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

target_link_libraries(tests gmock gtest pthread)

source_group(Pal FILES 	${INCLUDES})
