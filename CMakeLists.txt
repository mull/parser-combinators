cmake_minimum_required(VERSION 3.20)

project(
  parser-combinators
  VERSION 0.0.1
  LANGUAGES CXX)


add_executable(01 01.cpp)

set_property(TARGET 01 PROPERTY 
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)

add_executable(02 02.cpp)

set_property(TARGET 02 PROPERTY 
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)

add_executable(03 03.cpp)

set_property(TARGET 03 PROPERTY 
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)

add_executable(04 04.cpp)

set_property(TARGET 04 PROPERTY 
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)