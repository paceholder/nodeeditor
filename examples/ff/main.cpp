#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/ConnectionStyle>

#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QMenuBar>

#include <nodes/DataModelRegistry>

#include "ExpressionSourceModel.hpp"
#include "ExpressionDisplayModel.hpp"
#include "BoolConditionModel.hpp"
//#include "NumberDisplayDataModel.hpp"
//#include "AdditionModel.hpp"
//#include "SubtractionModel.hpp"
//#include "MultiplicationModel.hpp"
//#include "DivisionModel.hpp"
//#include "ModuloModel.hpp"
//#include "DecimalToIntegerModel.hpp"
//#include "IntegerToDecimalModel.hpp"

using QtNodes::DataModelRegistry;
using QtNodes::FlowScene;
using QtNodes::FlowView;
using QtNodes::ConnectionStyle;

static std::shared_ptr<DataModelRegistry>
registerDataModels()
{
  auto ret = std::make_shared<DataModelRegistry>();
  ret->registerModel<ExpressionSourceModel>("Sources");
  ret->registerModel<ExpressionDisplayModel>("Display");
  ret->registerModel<BoolConditionModel>("BoolCondition");

  return ret;
}


//static
//void
//setStyle()
//{
  //ConnectionStyle::setConnectionStyle(
  //R"(
  //{
    //"ConnectionStyle": {
      //"ConstructionColor": "gray",
      //"NormalColor": "black",
      //"SelectedColor": "gray",
      //"SelectedHaloColor": "deepskyblue",
      //"HoveredColor": "deepskyblue",

      //"LineWidth": 3.0,
      //"ConstructionLineWidth": 2.0,
      //"PointDiameter": 10.0,

      //"UseDataDefinedColors": true
    //}
  //}
  //)");
//}


int
main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  //setStyle();

  QWidget mainWidget;

  auto menuBar    = new QMenuBar();
  auto saveAction = menuBar->addAction("Save..");
  auto loadAction = menuBar->addAction("Load..");

  QVBoxLayout *l = new QVBoxLayout(&mainWidget);

  l->addWidget(menuBar);
  auto scene = new FlowScene(registerDataModels());
  l->addWidget(new FlowView(scene));
  l->setContentsMargins(0, 0, 0, 0);
  l->setSpacing(0);

  QObject::connect(saveAction, &QAction::triggered,
                   scene, &FlowScene::save);

  QObject::connect(loadAction, &QAction::triggered,
                   scene, &FlowScene::load);

  mainWidget.setWindowTitle("Field Function constructor");
  mainWidget.resize(800, 600);
  mainWidget.showNormal();

  return app.exec();
}
