set(CALC_SOURCE_FILES
  main.cpp
  MathOperationDataModel.cpp
  NumberDisplayDataModel.cpp
  NumberSourceDataModel.cpp
)

set(CALC_HEADER_FILES
  AdditionModel.hpp
  DivisionModel.hpp
  DecimalData.hpp
  MathOperationDataModel.hpp
  MultiplicationModel.hpp
  NumberDisplayDataModel.hpp
  NumberSourceDataModel.hpp
  SubtractionModel.hpp
)

add_executable(calculator
  WIN32
  ${CALC_SOURCE_FILES}
  ${CALC_HEADER_FILES}
  CMakeLists.txt
)

target_link_libraries(calculator QtNodes)

set(HEADLESS_CALC_SOURCE_FILES
  headless_main.cpp
  MathOperationDataModel.cpp
  NumberDisplayDataModel.cpp
  NumberSourceDataModel.cpp
)

add_executable(headless_calculator 
  ${HEADLESS_CALC_SOURCE_FILES}
  ${CALC_HEADER_FILES}
  CMakeLists.txt
)

target_link_libraries(headless_calculator QtNodes)

if(WIN32 AND BUILD_SHARED_LIBS)
  add_custom_command(
    TARGET calculator
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E env VCINSTALLDIR=${CMAKE_GENERATOR_INSTANCE}/VC
            ${WINDEPLOYQT_EXECUTABLE} --pdb "$<TARGET_FILE:calculator>")
  add_custom_command(
    TARGET headless_calculator
    POST_BUILD
    COMMAND
      ${CMAKE_COMMAND} -E env VCINSTALLDIR=${CMAKE_GENERATOR_INSTANCE}/VC
      ${WINDEPLOYQT_EXECUTABLE} --pdb "$<TARGET_FILE:headless_calculator>")
endif()
