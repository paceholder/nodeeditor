file(GLOB_RECURSE CPPS  ./*.cpp )
file(GLOB_RECURSE HPPS  ./*.hpp )

add_executable(styles WIN32 ${CPPS} ${HPPS})

target_link_libraries(styles QtNodes)

if(WIN32 AND BUILD_SHARED_LIBS)
  add_custom_command(
    TARGET styles
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E env VCINSTALLDIR=${CMAKE_GENERATOR_INSTANCE}/VC
            ${WINDEPLOYQT_EXECUTABLE} --pdb "$<TARGET_FILE:styles>")
endif()
