#include <nodes/Connection>
#include <nodes/Node>

#include <nodes/FlowScene>
#include <nodes/FlowGraphicsView>

#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>

#include <nodes/DataModelRegistry>

// simplest stupid model
class NaiveDataModel : public NodeDataModel
{
public:
  unsigned int nSlots(EndType) const override
  {
    return 3;
  }

  DataType dataType(EndType, slot) override
  {

  }
};

static bool
registerDataModels()
{
  DataModelRegistry::registerModel

  return true;
}


static bool registerOK = registerDataModels();

int
main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  FlowScene::instance();

  FlowScene::instance().createNode();

  FlowGraphicsView view(&FlowScene::instance());

  view.setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView,
                                        "Node-based flow editor"));
  view.resize(800, 600);
  view.show();

  //view.scale(0.5, 0.5);

  return app.exec();
}
