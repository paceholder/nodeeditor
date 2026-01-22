#include "MyNodeEditor.hpp"

#include "AdditionModel.hpp"
#include "DivisionModel.hpp"
#include "MultiplicationModel.hpp"
#include "NumberDisplayDataModel.hpp"
#include "NumberSourceDataModel.hpp"
#include "StringDataModel.hpp"
#include "SubtractionModel.hpp"

std::shared_ptr<QtNodes::NodeDelegateModelRegistry> MyNodeEditor::registerDataModels() const
{
    auto ret = std::make_shared<QtNodes::NodeDelegateModelRegistry>();
    ret->registerModel<NumberSourceDataModel>("Sources");

    ret->registerModel<NumberDisplayDataModel>("Displays");

    ret->registerModel<AdditionModel>("Operators");

    ret->registerModel<SubtractionModel>("Operators");

    ret->registerModel<MultiplicationModel>("Operators");

    ret->registerModel<DivisionModel>("Operators");

    ret->registerModel<StringDataModel>("Sources");

    return ret;
}
