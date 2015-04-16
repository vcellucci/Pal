cmake_minimum_required(VERSION 2.6)
project(Pal)

add_subdirectory(tests)
if( DEFINED INCLUDE_TESTS )
    add_subdirectory(gmock-1.7.0)
endif( DEFINED INCLUDE_TESTS )

