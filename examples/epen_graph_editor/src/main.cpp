#include "GraphEditorMainWindow.hpp"
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtWidgets/QApplication>

#include "DataFlowModel.hpp"
#include "data_models/FixedBuffer.hpp"
#include "data_models/Process.hpp"
#include "data_models/Scalar_Double_Fixed.hpp"
#include "data_models/Scalar_Double_Plain.hpp"
#include "data_models/Scalar_Double_Slider.hpp"

#include "data_models/Scalar_Float_Fixed.hpp"
#include "data_models/Scalar_Float_Plain.hpp"
#include "data_models/Scalar_Float_Slider.hpp"

#include "data_models/Scalar_Int_Fixed.hpp"
#include "data_models/Scalar_Int_Plain.hpp"
#include "data_models/Scalar_Int_Slider.hpp"

#include "data_models/Scalar_UnsignedInt_Fixed.hpp"
#include "data_models/Scalar_UnsignedInt_Plain.hpp"
#include "data_models/Scalar_UnsignedInt_Slider.hpp"

#include "data_models/Scalar_Boolean_Checkbox.hpp"
#include "data_models/Scalar_Boolean_Fixed.hpp"

#include "data_models/Scalar_Float4_Color.hpp"
#include "data_models/Scalar_Float4_Fixed.hpp"

#include "data_models/Array_Double_Fixed.hpp"
#include "data_models/Array_Float_Fixed.hpp"
#include "data_models/Array_Int_Fixed.hpp"
#include "data_models/Array_UnsignedInt_Fixed.hpp"

#include "data_models/VideoInput.hpp"
#include "data_models/VideoOutput.hpp"
#include <QtGui/QScreen>
#include <QtWidgets/QApplication>

using QtNodes::ConnectionStyle;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;
using QtNodes::NodeDelegateModelRegistry;

static std::shared_ptr<NodeDelegateModelRegistry> registerDataModels()
{
    auto ret = std::make_shared<NodeDelegateModelRegistry>();
    ret->registerModel<VideoInput>();
    ret->registerModel<VideoOutput>();
    ret->registerModel<FixedBuffer>();
    ret->registerModel<Process>();

    ret->registerModel<Scalar_UnsignedInt_Slider>();
    ret->registerModel<Scalar_UnsignedInt_Plain>();
    ret->registerModel<Scalar_UnsignedInt_Fixed>();

    ret->registerModel<Scalar_Int_Plain>();
    ret->registerModel<Scalar_Int_Slider>();
    ret->registerModel<Scalar_Int_Fixed>();

    ret->registerModel<Scalar_Double_Plain>();
    ret->registerModel<Scalar_Double_Slider>();
    ret->registerModel<Scalar_Double_Fixed>();

    ret->registerModel<Scalar_Float_Plain>();
    ret->registerModel<Scalar_Float_Slider>();
    ret->registerModel<Scalar_Float_Fixed>();

    ret->registerModel<Scalar_Boolean_Checkbox>();
    ret->registerModel<Scalar_Boolean_Fixed>();

    ret->registerModel<Scalar_Float4_Color>();
    ret->registerModel<Scalar_Float4_Fixed>();

    ret->registerModel<Array_Double_Fixed>();
    ret->registerModel<Array_Float_Fixed>();
    ret->registerModel<Array_Int_Fixed>();
    ret->registerModel<Array_UnsignedInt_Fixed>();

    return ret;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    std::shared_ptr<NodeDelegateModelRegistry> registry = registerDataModels();

    DataFlowModel dataFlowGraphModel(registry);

    DataFlowGraphicsScene scene(dataFlowGraphModel);

    GraphEditorWindow view(&scene, &dataFlowGraphModel);
    view.showNormal();

    return app.exec();
}
