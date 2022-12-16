#include "AdditionModel.hpp"
#include "DivisionModel.hpp"
#include "MultiplicationModel.hpp"
#include "NumberDisplayDataModel.hpp"
#include "NumberSourceDataModel.hpp"
#include "SubtractionModel.hpp"

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/NodeDelegateModelRegistry>

using QtNodes::DataFlowGraphModel;
using QtNodes::NodeDelegateModelRegistry;
using QtNodes::NodeId;

static std::shared_ptr<NodeDelegateModelRegistry> registerDataModels()
{
    auto ret = std::make_shared<NodeDelegateModelRegistry>();
    ret->registerModel<NumberSourceDataModel>("Sources");

    ret->registerModel<NumberDisplayDataModel>("Displays");

    ret->registerModel<AdditionModel>("Operators");

    ret->registerModel<SubtractionModel>("Operators");

    ret->registerModel<MultiplicationModel>("Operators");

    ret->registerModel<DivisionModel>("Operators");

    return ret;
}

/**
 * This scene JSON was saved by the normal `calculator` example.
 * It has one source number node connected to both inputs of an addition node,
 * the result is rendered in a displayer node.
 *
 *                          ____________
 *                     /  O[            ]
 *    _____________  /     [  addition  ]          ______________
 *   [ source node ]O      [    node    ]O- - - -O[ display node ]
 *    -------------  \     [            ]          --------------
 *                     \  O[____________]
 */
static QString addingNumbersScene(
    R"(
    {
        "nodes": [
            {
                "id": 0,
                "internal-data": {
                    "model-name": "NumberSource",
                    "number": "3"
                },
                "position": {
                    "x": -338,
                    "y": -160
                }
            },
            {
                "id": 1,
                "internal-data": {
                    "model-name": "Addition"
                },
                "position": {
                    "x": -31,
                    "y": -264
                }
            },
            {
                "id": 2,
                "internal-data": {
                    "model-name": "Result"
                },
                "position": {
                    "x": 201,
                    "y": -129
                }
            }
        ],
        "connections": [
            {
                "inPortIndex": 0,
                "intNodeId": 2,
                "outNodeId": 1,
                "outPortIndex": 0
            },
            {
                "inPortIndex": 1,
                "intNodeId": 1,
                "outNodeId": 0,
                "outPortIndex": 0
            },
            {
                "inPortIndex": 0,
                "intNodeId": 1,
                "outNodeId": 0,
                "outPortIndex": 0
            }
        ]
    }
)");

int main(int argc, char *argv[])
{
    std::shared_ptr<NodeDelegateModelRegistry> registry = registerDataModels();

    // Here we create a graph model without attaching to any view or scene.
    DataFlowGraphModel dataFlowGraphModel(registry);

    // Alternatively you can create the graph by yourself with the functions
    // `DataFlowGraphModel::addNode` and `DataFlowGraphModel::addConnection` and
    // use the obtained `NodeId` to fetch the `NodeDelegateModel`s
    QJsonDocument sceneJson = QJsonDocument::fromJson(addingNumbersScene.toUtf8());

    dataFlowGraphModel.load(sceneJson.object());

    qInfo() << "Data Flow graph was created from a json-serialized graph";

    NodeId const nodeSource = 0;
    NodeId const nodeResult = 2;

    qInfo() << "========================================";
    qInfo() << "Entering the number " << 33.3 << "to the input node";
    dataFlowGraphModel.delegateModel<NumberSourceDataModel>(nodeSource)->setNumber(33.3);

    qInfo() << "Result of the addiion operation: "
            << dataFlowGraphModel.delegateModel<NumberDisplayDataModel>(nodeResult)->number();

    qInfo() << "========================================";
    qInfo() << "Entering the number " << -5. << "to the input node";
    dataFlowGraphModel.delegateModel<NumberSourceDataModel>(nodeSource)->setNumber(-5);

    qInfo() << "Result of the addiion operation: "
            << dataFlowGraphModel.delegateModel<NumberDisplayDataModel>(nodeResult)->number();
    return 0;
}
