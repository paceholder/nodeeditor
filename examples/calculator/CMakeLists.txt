set(CALC_SOURCE_FILES
  main.cpp
  MathOperationDataModel.cpp
  NumberDisplayDataModel.cpp
  NumberSourceDataModel.cpp)

add_executable(calculator ${CALC_SOURCE_FILES})

target_link_libraries(calculator QtNodes)



set(HEADLESS_CALC_SOURCE_FILES
  headless_main.cpp
  MathOperationDataModel.cpp
  NumberDisplayDataModel.cpp
  NumberSourceDataModel.cpp)

add_executable(headless_calculator ${HEADLESS_CALC_SOURCE_FILES})

target_link_libraries(headless_calculator QtNodes)
