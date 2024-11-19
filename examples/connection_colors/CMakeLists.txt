file(GLOB_RECURSE CPPS  ./*.cpp )
file(GLOB_RECURSE HPPS  ./*.hpp )

add_executable(connection_colors WIN32 ${CPPS} ${HPPS})

target_link_libraries(connection_colors QtNodes)

if(WIN32 AND BUILD_SHARED_LIBS)
  add_custom_command(
    TARGET connection_colors
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E env VCINSTALLDIR=${CMAKE_GENERATOR_INSTANCE}/VC
            ${WINDEPLOYQT_EXECUTABLE} --pdb "$<TARGET_FILE:connection_colors>")
endif()
