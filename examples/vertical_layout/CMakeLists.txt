file(GLOB_RECURSE CPPS  ./*.cpp )

add_executable(vertical_layout WIN32 ${CPPS})

target_link_libraries(vertical_layout QtNodes)

if(WIN32 AND BUILD_SHARED_LIBS)
  add_custom_command(
    TARGET vertical_layout
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E env VCINSTALLDIR=${CMAKE_GENERATOR_INSTANCE}/VC
            ${WINDEPLOYQT_EXECUTABLE} --pdb "$<TARGET_FILE:vertical_layout>")
endif()
