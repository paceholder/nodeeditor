if(BUILD_TESTING)
  find_package(Catch2 2.13.7 QUIET)

  if(NOT Catch2_FOUND)
    add_subdirectory(Catch2)
  endif()
endif()
