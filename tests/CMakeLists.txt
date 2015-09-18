cmake_minimum_required(VERSION 2.6)
project(tests)

file(GLOB SRC *.cpp)
file(GLOB HDR *.h)
file(GLOB INCLUDES ../include/Pal/*.h)
file(GLOB DETAIL ../include/Pal/details/*.h)

include_directories("../googlemock/include"
                    "../googletest/include"
                    "../include")



add_executable(tests ${SRC} ${HDR} ${INCLUDES} ${DETAIL})

list( APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

target_link_libraries(tests gmock gtest pthread)

source_group(Pal FILES 	${INCLUDES})
source_group(details FILES ${DETAIL})
