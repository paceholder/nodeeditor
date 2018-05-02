#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/ConnectionStyle>
#include <nodes/TypeConverter>

#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QMenuBar>

#include <nodes/DataModelRegistry>

#include "NumberSourceDataModel.hpp"
#include "NumberDisplayDataModel.hpp"
#include "AdditionModel.hpp"
#include "SubtractionModel.hpp"
#include "MultiplicationModel.hpp"
#include "DivisionModel.hpp"
#include "ModuloModel.hpp"
#include "Converters.hpp"


using QtNodes::DataModelRegistry;
using QtNodes::FlowScene;
using QtNodes::FlowView;
using QtNodes::ConnectionStyle;
using QtNodes::TypeConverter;
using QtNodes::TypeConverterId;

static std::shared_ptr<DataModelRegistry>
registerDataModels()
{
  auto ret = std::make_shared<DataModelRegistry>();
  ret->registerModel<NumberSourceDataModel>("Sources");

  ret->registerModel<NumberDisplayDataModel>("Displays");

  ret->registerModel<AdditionModel>("Operators");

  ret->registerModel<SubtractionModel>("Operators");

  ret->registerModel<MultiplicationModel>("Operators");

  ret->registerModel<DivisionModel>("Operators");

  ret->registerModel<ModuloModel>("Operators");

  ret->registerTypeConverter(std::make_pair(DecimalData().type(),
                                            IntegerData().type()),
                             TypeConverter{DecimalToIntegerConverter()});



  ret->registerTypeConverter(std::make_pair(IntegerData().type(),
                                            DecimalData().type()),
                             TypeConverter{IntegerToDecimalConverter()});

  return ret;
}


static
auto
connectionStyle()
{
  auto style = ConnectionStyle::defaultStyle();
  
  style->setConstructionColor(QColor("gray"));
  style->setNormalColor(QColor("black"));
  style->setSelectedColor(QColor("gray"));
  style->setSelectedHaloColor(QColor("deepskyblue"));
  style->setHoveredColor(QColor("deepskyblue"));

  style->setLineWidth(3.0);
  style->setConstructionLineWidth(2.0);
  style->setPointDiameter(10.0);

  style->useDataDefinedColors(true);

  return style;
}


int
main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QWidget mainWidget;

  auto menuBar    = new QMenuBar();
  auto saveAction = menuBar->addAction("Save..");
  auto loadAction = menuBar->addAction("Load..");

  QVBoxLayout *l = new QVBoxLayout(&mainWidget);

  l->addWidget(menuBar);
  auto scene = new FlowScene(registerDataModels(), &mainWidget);
  scene->setConnectionStyle(connectionStyle());
  l->addWidget(new FlowView(scene));
  l->setContentsMargins(0, 0, 0, 0);
  l->setSpacing(0);

  QObject::connect(saveAction, &QAction::triggered,
                   scene, &FlowScene::save);

  QObject::connect(loadAction, &QAction::triggered,
                   scene, &FlowScene::load);

  mainWidget.setWindowTitle("Dataflow tools: simplest calculator");
  mainWidget.resize(800, 600);
  mainWidget.showNormal();

  return app.exec();
}
