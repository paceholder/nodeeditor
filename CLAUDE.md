# CLAUDE.md - NodeEditor QML

Editor visual de nodes baseado em QtNodes com suporte completo a Qt Quick/QML.

## Visao Geral

nodeeditor_qml e um fork do QtNodes com suporte adicional a QML. Permite criar interfaces visuais de programacao (node-based) como:
- Editores de shaders
- Pipelines de processamento de dados
- Sistemas de estrategias de trading
- Blueprints de logica

## Arquitetura MVVM

```
┌─────────────────────────────────────────────────────────────┐
│                    QML (View)                               │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐ │
│  │ NodeGraph   │  │   Node      │  │   Connection        │ │
│  │   .qml      │  │   .qml      │  │     .qml            │ │
│  └──────┬──────┘  └──────┬──────┘  └────────┬────────────┘ │
└─────────┼────────────────┼──────────────────┼──────────────┘
          │                │                  │
          ▼                ▼                  ▼
┌─────────────────────────────────────────────────────────────┐
│               C++ Models (ViewModel)                         │
│  ┌─────────────────┐  ┌──────────────────────────────────┐  │
│  │ QuickGraphModel │  │ NodesListModel                   │  │
│  │ (controller)    │  │ ConnectionsListModel             │  │
│  └────────┬────────┘  └──────────────────────────────────┘  │
└───────────┼─────────────────────────────────────────────────┘
            │
            ▼
┌─────────────────────────────────────────────────────────────┐
│            DataFlowGraphModel (Model)                        │
│            NodeDelegateModelRegistry                         │
└─────────────────────────────────────────────────────────────┘
```

## Estrutura de Arquivos

```
nodeeditor_qml/
├── CMakeLists.txt
├── include/QtNodes/
│   ├── internal/                    # Core classes (Qt Widgets)
│   │   ├── NodeDelegateModel.hpp    # Base para nodes customizados
│   │   ├── NodeDelegateModelRegistry.hpp
│   │   ├── DataFlowGraphModel.hpp   # Modelo de dados principal
│   │   ├── NodeData.hpp             # Dados transferidos entre nodes
│   │   └── ...
│   └── qml/                         # QML-specific
│       ├── QuickGraphModel.hpp      # Controller QML
│       ├── NodesListModel.hpp       # Lista de nodes para Repeater
│       └── ConnectionsListModel.hpp # Lista de conexoes
├── src/
│   ├── *.cpp                        # Implementacoes core
│   └── qml/*.cpp                    # Implementacoes QML
├── resources/
│   ├── qml/
│   │   ├── NodeGraph.qml            # Canvas principal
│   │   ├── Node.qml                 # Componente node
│   │   ├── Connection.qml           # Curvas Bezier
│   │   └── NodeGraphStyle.qml       # Theming
│   └── qml.qrc
└── examples/
    └── qml_calculator/              # Exemplo de calculadora
```

## Build

### Opcoes CMake

```cmake
# IMPORTANTE: Habilitar suporte QML
set(BUILD_QML ON CACHE BOOL "" FORCE)

# Biblioteca estatica (recomendado para embedding)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(nodeeditor_qml)
target_link_libraries(meu_app PRIVATE QtNodes::QtNodes)
```

### Targets

| Target | Descricao |
|--------|-----------|
| `QtNodes` | Biblioteca principal |
| `QtNodes::QtNodes` | Alias para linkagem |

## Uso Basico em QML

### 1. Registrar Tipos

```cpp
// main.cpp
#include <QQmlApplicationEngine>
#include <QtNodes/qml/QuickGraphModel.hpp>
#include <QtNodes/qml/NodesListModel.hpp>
#include <QtNodes/qml/ConnectionsListModel.hpp>
#include <QtNodes/internal/NodeDelegateModelRegistry.hpp>
#include "MeuNode.hpp"

int main(int argc, char *argv[]) {
    // ...

    // Registrar tipos QML
    qmlRegisterType<QtNodes::QuickGraphModel>("QtNodes", 1, 0, "QuickGraphModel");
    qmlRegisterType<QtNodes::NodesListModel>("QtNodes", 1, 0, "NodesListModel");
    qmlRegisterType<QtNodes::ConnectionsListModel>("QtNodes", 1, 0, "ConnectionsListModel");
    qmlRegisterType<QtNodes::NodeGraphStyle>("QtNodes", 1, 0, "NodeGraphStyle");

    // Carregar recursos QML do QtNodes
    Q_INIT_RESOURCE(qml);

    // Criar registry e registrar nodes customizados
    auto registry = std::make_shared<QtNodes::NodeDelegateModelRegistry>();
    registry->registerModel<MeuNode>("Minha Categoria");

    // Criar graph model
    auto graphModel = new QtNodes::QuickGraphModel();
    graphModel->setRegistry(registry);

    // Expor para QML
    engine.rootContext()->setContextProperty("_graphModel", graphModel);

    // ...
}
```

### 2. Usar no QML

```qml
import QtQuick 2.15
import QtNodes 1.0

ApplicationWindow {
    NodeGraph {
        anchors.fill: parent
        graphModel: _graphModel

        // Theming customizado (opcional)
        style: NodeGraphStyle {
            canvasBackground: "#1e1e1e"
            nodeBackground: "#2d2d2d"
            nodeSelectedBorder: "#4a9eff"
        }
    }
}
```

## Criando Nodes Customizados

### 1. Definir Tipo de Dados

```cpp
// MeuDado.hpp
#include <QtNodes/internal/NodeData.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class DecimalData : public NodeData
{
public:
    DecimalData() : _value(0.0) {}
    DecimalData(double value) : _value(value) {}

    NodeDataType type() const override {
        return NodeDataType{"decimal", "Decimal"};
    }

    double value() const { return _value; }

private:
    double _value;
};
```

### 2. Implementar NodeDelegateModel

```cpp
// MeuNode.hpp
#include <QtNodes/internal/NodeDelegateModel.hpp>
#include "MeuDado.hpp"

using QtNodes::NodeDelegateModel;
using QtNodes::PortType;
using QtNodes::PortIndex;

class MeuNode : public NodeDelegateModel
{
    Q_OBJECT

public:
    MeuNode() : _result(0.0) {}

    // Identificacao
    QString caption() const override { return "Meu Node"; }
    QString name() const override { return "MeuNode"; }

    // Portas
    unsigned int nPorts(PortType portType) const override {
        return portType == PortType::In ? 2 : 1;  // 2 inputs, 1 output
    }

    NodeDataType dataType(PortType, PortIndex) const override {
        return DecimalData{}.type();
    }

    // Dados
    std::shared_ptr<NodeData> outData(PortIndex) override {
        return std::make_shared<DecimalData>(_result);
    }

    void setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override {
        auto decimalData = std::dynamic_pointer_cast<DecimalData>(data);

        if (portIndex == 0) {
            _input1 = decimalData ? decimalData->value() : 0.0;
        } else {
            _input2 = decimalData ? decimalData->value() : 0.0;
        }

        compute();
    }

    void compute() {
        _result = _input1 + _input2;  // Exemplo: soma
        emit dataUpdated(0);  // Notificar output
    }

    // Widget embarcado (opcional)
    QWidget* embeddedWidget() override { return nullptr; }

private:
    double _input1 = 0.0;
    double _input2 = 0.0;
    double _result = 0.0;
};
```

### 3. Registrar no Registry

```cpp
auto registry = std::make_shared<NodeDelegateModelRegistry>();

// Forma simples
registry->registerModel<MeuNode>("Categoria");

// Com factory customizada
registry->registerModel<MeuNode>(
    []() { return std::make_unique<MeuNode>(); },
    "Categoria"
);
```

## API QuickGraphModel

### Propriedades QML

| Propriedade | Tipo | Descricao |
|-------------|------|-----------|
| `nodes` | NodesListModel* | Lista de nodes |
| `connections` | ConnectionsListModel* | Lista de conexoes |
| `canUndo` | bool | Tem acao para desfazer? |
| `canRedo` | bool | Tem acao para refazer? |

### Metodos Invocaveis (Q_INVOKABLE)

```qml
// Adicionar node
var nodeId = graphModel.addNode("MeuNode")

// Remover node
graphModel.removeNode(nodeId)

// Criar conexao (output -> input)
graphModel.addConnection(outNodeId, outPortIndex, inNodeId, inPortIndex)

// Remover conexao
graphModel.removeConnection(outNodeId, outPortIndex, inNodeId, inPortIndex)

// Verificar se conexao e possivel
var possible = graphModel.connectionPossible(outNodeId, outPort, inNodeId, inPort)

// Obter tipo de dados da porta
var typeId = graphModel.getPortDataTypeId(nodeId, portType, portIndex)

// Undo/Redo
graphModel.undo()
graphModel.redo()
```

### NodesListModel Roles

| Role | ID | Tipo | Descricao |
|------|-----|------|-----------|
| NodeIdRole | 256 | int | ID unico do node |
| NodeTypeRole | 257 | QString | Nome do tipo |
| PositionRole | 258 | QPointF | Posicao no canvas |
| CaptionRole | 259 | QString | Titulo visivel |
| InPortsRole | 260 | QVariantList | Portas de entrada |
| OutPortsRole | 261 | QVariantList | Portas de saida |
| DelegateModelRole | 262 | QObject* | NodeDelegateModel* |

### Mover Node

```qml
// Mover node para posicao
graphModel.nodes.moveNode(nodeId, x, y)
```

## Componentes QML

### NodeGraph

Canvas principal com pan/zoom infinito e grade.

```qml
NodeGraph {
    graphModel: _graphModel
    style: NodeGraphStyle { }
    nodeContentDelegate: Component { /* conteudo customizado */ }

    // Propriedades
    zoomLevel: 1.0
    panOffset: Qt.point(0, 0)
    selectedNodeIds: ({})

    // Funcoes
    function selectNode(nodeId, additive) { }
    function clearSelection() { }
    function deleteSelected() { }
    function getSelectedNodeIds() { return [] }
}
```

### Node

Componente visual de node individual.

```qml
Node {
    graph: nodeGraphRef
    nodeId: model.nodeId
    nodeType: model.nodeType
    caption: model.caption
    inPorts: model.inPorts
    outPorts: model.outPorts
    delegateModel: model.delegateModel
    contentDelegate: customContentComponent
}
```

### Connection

Curva Bezier conectando portas.

```qml
Connection {
    graph: nodeGraphRef
    sourceNodeId: 1
    sourcePortIndex: 0
    destNodeId: 2
    destPortIndex: 0
}
```

### NodeGraphStyle

Tema customizavel.

```qml
NodeGraphStyle {
    // Canvas
    canvasBackground: "#2b2b2b"
    gridMinorLine: "#353535"
    gridMajorLine: "#151515"
    gridMinorSpacing: 20
    gridMajorSpacing: 100

    // Node
    nodeBackground: "#2d2d2d"
    nodeBorder: "black"
    nodeSelectedBorder: "#4a9eff"
    nodeBorderWidth: 2
    nodeSelectedBorderWidth: 3
    nodeRadius: 5
    nodeCaptionColor: "#eeeeee"
    nodeCaptionFontSize: 12

    // Portas
    portSize: 12
    portTypeColors: ({
        "decimal": "#4CAF50",
        "integer": "#2196F3",
        "string": "#FF9800",
        "boolean": "#9C27B0",
        "default": "#9E9E9E"
    })

    // Conexoes
    connectionWidth: 3
    connectionSelectedWidth: 4
    connectionSelectionOutline: "#4a9eff"

    // Selecao
    selectionRectFill: "#224a9eff"
    selectionRectBorder: "#4a9eff"
}
```

## Atalhos de Teclado

| Atalho | Acao |
|--------|------|
| Delete / Backspace / X | Deletar selecionados |
| Ctrl+Z | Desfazer |
| Ctrl+Shift+Z / Ctrl+Y | Refazer |
| Ctrl+Click | Selecao aditiva |
| Alt+Drag | Pan (alternativo) |
| Mouse wheel | Zoom (centrado no cursor) |
| Middle mouse drag | Pan |
| Left drag no canvas | Selecao marquee |

## Validacao de Conexoes

Conexoes so sao criadas se os tipos de dados forem compativeis:

```cpp
// No NodeDelegateModel
NodeDataType dataType(PortType, PortIndex) const override {
    return NodeDataType{"decimal", "Decimal"};
}
```

Nodes com tipos diferentes nao podem ser conectados. O sistema valida automaticamente usando `NodeData::sameType()`.

## Serializacao

### Salvar

```cpp
// DataFlowGraphModel tem suporte a save/load
auto model = graphModel->graphModel();
QJsonObject json = model->save();
```

### Carregar

```cpp
model->load(json);
```

## Undo/Redo

O sistema usa QUndoStack internamente. Operacoes suportadas:
- Adicionar/remover nodes
- Criar/remover conexoes
- Mover nodes

```qml
// Verificar estado
if (graphModel.canUndo) graphModel.undo()
if (graphModel.canRedo) graphModel.redo()
```

## Exemplo Completo: Calculadora

```cpp
// AddNode.hpp
class AddNode : public NodeDelegateModel {
    Q_OBJECT
public:
    QString caption() const override { return "Add"; }
    QString name() const override { return "Add"; }

    unsigned int nPorts(PortType pt) const override {
        return pt == PortType::In ? 2 : 1;
    }

    NodeDataType dataType(PortType, PortIndex) const override {
        return DecimalData{}.type();
    }

    void setInData(std::shared_ptr<NodeData> data, PortIndex idx) override {
        auto d = std::dynamic_pointer_cast<DecimalData>(data);
        if (idx == 0) _a = d ? d->value() : 0;
        else _b = d ? d->value() : 0;
        compute();
    }

    std::shared_ptr<NodeData> outData(PortIndex) override {
        return std::make_shared<DecimalData>(_a + _b);
    }

    QWidget* embeddedWidget() override { return nullptr; }

private:
    void compute() { emit dataUpdated(0); }
    double _a = 0, _b = 0;
};
```

## Troubleshooting

### "NodeGraphStyle unavailable"
Adicione no main.cpp antes de carregar QML:
```cpp
Q_INIT_RESOURCE(qml);
```

### Nodes nao aparecem
Verifique se `BUILD_QML=ON` no CMake e se o registry foi configurado corretamente.

### Conexoes nao sao criadas
Verifique se os tipos de dados (`NodeDataType.id`) sao compativeis entre as portas.

### Drag de conexao nao funciona
Certifique-se de que `focus: true` esta no NodeGraph.

### Performance com muitos nodes
- Use `visible: false` para nodes fora da viewport
- Reduza a frequencia de atualizacoes em `dataUpdated`
- Considere pooling de conexoes

## Referencias

- [QtNodes original](https://github.com/paceholder/nodeeditor)
- [Dear ImGui Node Editor](https://github.com/thedmd/imgui-node-editor) (alternativa)
- [Qt Quick Controls](https://doc.qt.io/qt-6/qtquickcontrols-index.html)
