set(TARGET_NAME qml_calculator)

add_executable(${TARGET_NAME}
  main.cpp
  QmlCalculatorResources.qrc
  QmlNumberSourceDataModel.hpp
  QmlNumberDisplayDataModel.hpp
  AdditionModel.hpp
  MultiplyModel.hpp
  SubtractModel.hpp
  DivideModel.hpp
  FormatNumberModel.hpp
  StringDisplayModel.hpp
  IntegerSourceModel.hpp
  IntegerDisplayModel.hpp
  ToIntegerModel.hpp
  GreaterThanModel.hpp
  BooleanDisplayModel.hpp
  DecimalData.hpp
  StringData.hpp
  IntegerData.hpp
  BooleanData.hpp
)

target_link_libraries(${TARGET_NAME}
  PRIVATE
    QtNodes::QtNodes
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::QuickControls2
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
)
