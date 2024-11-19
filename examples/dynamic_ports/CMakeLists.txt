add_executable(
  dynamic_ports
  WIN32
  "CMakeLists.txt"
  "DynamicPortsModel.cpp"
  "DynamicPortsModel.hpp"
  "main.cpp"
  "PortAddRemoveWidget.cpp"
  "PortAddRemoveWidget.hpp")

target_link_libraries(dynamic_ports QtNodes)

if(WIN32 AND BUILD_SHARED_LIBS)
  add_custom_command(
    TARGET dynamic_ports
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E env VCINSTALLDIR=${CMAKE_GENERATOR_INSTANCE}/VC
            ${WINDEPLOYQT_EXECUTABLE} --pdb "$<TARGET_FILE:dynamic_ports>")
endif()
