if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/single_include/catch2/catch.hpp")
  file(DOWNLOAD https://raw.githubusercontent.com/catchorg/Catch2/v2.4.1/single_include/catch2/catch.hpp
    "${CMAKE_CURRENT_BINARY_DIR}/single_include/catch2/catch.hpp"
    EXPECTED_HASH SHA256=a4b90030cb813f0452bb00e97c92ca6c2ecf9386a2f000b6effb8e265a53959e
  )
endif()

add_library(Catch2 INTERFACE)
add_library(Catch2::Catch2 ALIAS Catch2)
target_include_directories(Catch2
  INTERFACE
    "${CMAKE_CURRENT_BINARY_DIR}/single_include"
)
