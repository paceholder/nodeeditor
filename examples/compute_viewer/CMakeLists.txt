set(COMPUTE_SOURCE_FILES
    main.cpp
    InputModel.cpp
    OutputModel.cpp
    DecimationModel.cpp
    DecimationWidget.cpp
    DasPwModel.cpp
    DasPwWidget.cpp
    ClutterFilterModel.cpp
    ClutterFilterWidget.cpp
    CalculatorModel.cpp
    CalculatorWidget.cpp
    RoiRectModel.cpp
    RoiRectWidget.cpp
    RoiSphereModel.cpp
    AverageModel.cpp
    RoiSphereWidget.cpp
    DopplerViewerModel.cpp)

set(COMPUTE_HEADER_FILES
    InputData.hpp
    InputModel.hpp
    OutputModel.hpp
    DecimationModel.hpp
    DecimationData.hpp
    DecimationWidget.hpp
    ClutterFilterModel.hpp
    ClutterFilterData.hpp
    ClutterFilterWidget.hpp
    CalculatorModel.hpp
    CalculatorData.hpp
    CalculatorWidget.hpp
    RoiData.hpp
    RoiRectModel.hpp
    RoiRectWidget.hpp
    RoiSphereModel.hpp
    RoiSphereWidget.hpp
    DasPwModel.hpp
    DasPwData.hpp
    DopplerViewerModel.hpp
    DopplerViewerData.hpp
    AverageModel.hpp
    AverageData.hpp
    DasPwWidget.hpp)

add_executable(compute_viewer WIN32 ${COMPUTE_SOURCE_FILES}
                                    ${COMPUTE_HEADER_FILES} CMakeLists.txt)

target_link_libraries(compute_viewer QtNodes)

if(WIN32 AND BUILD_SHARED_LIBS)
  add_custom_command(
    TARGET compute_viewer
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E env VCINSTALLDIR=${CMAKE_GENERATOR_INSTANCE}/VC
            ${WINDEPLOYQT_EXECUTABLE} --pdb "$<TARGET_FILE:calculator_qml>")
endif()
