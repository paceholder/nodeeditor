set(ACQ_SOURCE_FILES main.cpp AcqModel.cpp)

set(ACQ_HEADER_FILES AcqData.hpp AcqModel.hpp)

add_executable(acquisition_viewer WIN32 ${ACQ_SOURCE_FILES} ${ACQ_HEADER_FILES}
                                        CMakeLists.txt)

target_link_libraries(acquisition_viewer QtNodes)

if(WIN32 AND BUILD_SHARED_LIBS)
  add_custom_command(
    TARGET acquisition_viewer
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E env VCINSTALLDIR=${CMAKE_GENERATOR_INSTANCE}/VC
            ${WINDEPLOYQT_EXECUTABLE} --pdb "$<TARGET_FILE:acquisition_viewer>")
endif()
