if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/single_include/catch2/catch.hpp")
  file(DOWNLOAD https://raw.githubusercontent.com/catchorg/Catch2/v2.13.7/single_include/catch2/catch.hpp
    "${CMAKE_CURRENT_BINARY_DIR}/single_include/catch2/catch.hpp"
    EXPECTED_HASH SHA256=ea379c4a3cb5799027b1eb451163dff065a3d641aaba23bf4e24ee6b536bd9bc
  )
endif()

add_library(Catch2 INTERFACE)
add_library(Catch2::Catch2 ALIAS Catch2)
target_include_directories(Catch2
  INTERFACE
    "${CMAKE_CURRENT_BINARY_DIR}/single_include"
)
