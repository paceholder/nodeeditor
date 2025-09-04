set(CALC_SOURCE_FILES
    main.cpp MathOperationDataModel.cpp MyNodeEditor.cpp NumberDisplayDataModel.cpp
    NumberSourceDataModel.cpp StringDataModel.cpp)

set(CALC_HEADER_FILES
    AdditionModel.hpp
    DivisionModel.hpp
    DecimalData.hpp
    MathOperationDataModel.hpp
    MultiplicationModel.hpp
    MyNodeEditor.hpp
    NumberDisplayDataModel.hpp
    NumberSourceDataModel.hpp
    SubtractionModel.hpp
    StringDataModel.hpp
    StringData.hpp)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
cmake_policy(SET CMP0071 NEW)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_resources(QT_RESOURCES "main.qrc")
qt_add_executable(calculator_qml ${CALC_SOURCE_FILES} ${CALC_HEADER_FILES}
                  ${QT_RESOURCES})

qt_add_qml_module(
  calculator_qml VERSION 1.0 URI hello QML_FILES main.qml
)

target_link_libraries(calculator_qml PRIVATE Qt6::Gui QtNodes)
