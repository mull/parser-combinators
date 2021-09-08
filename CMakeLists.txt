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

add_executable(05 05.cpp)
set_property(TARGET 05 PROPERTY 
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)

add_executable(06 06.cpp)
set_property(TARGET 06 PROPERTY 
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)


add_executable(07 07_parse_str.cpp)
set_property(TARGET 07 PROPERTY 
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)


Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.0-preview3
)
FetchContent_MakeAvailable(Catch2)

add_executable(json json/main.cpp)
set_property(TARGET json PROPERTY 
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)

add_executable(json_test json/test.cpp)
set_property(TARGET json_test PROPERTY 
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)
target_link_libraries(json_test PRIVATE Catch2::Catch2WithMain)


add_executable(parsec_test parsec_test.cpp)
set_property(TARGET parsec_test PROPERTY 
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)
target_link_libraries(parsec_test PRIVATE Catch2::Catch2WithMain)