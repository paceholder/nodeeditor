import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtNodes 1.0

Window {
    visible: true
    width: 1024
    height: 768
    title: "QML Calculator"

    // Context property set from C++
    property QuickGraphModel model: _graphModel 
    
    Column {
        anchors.fill: parent
        
        Row {
            height: 40
            spacing: 10
            padding: 5
            Button { text: "Add Source"; onClicked: model.addNode("NumberSource") }
            Button { text: "Add Display"; onClicked: model.addNode("NumberDisplay") }
            Button { text: "Add Addition"; onClicked: model.addNode("Addition") }
        }
        
        NodeGraph {
            width: parent.width
            height: parent.height - 40
            graphModel: model
            
            nodeContentDelegate: Component {
                Item {
                    // delegateModel and nodeType are provided by the Loader in Node.qml
                    
                    TextField {
                        anchors.centerIn: parent
                        width: parent.width
                        visible: nodeType === "NumberSource"
                        text: (delegateModel && delegateModel.number !== undefined) ? delegateModel.number.toString() : "0"
                        onEditingFinished: { 
                            if (delegateModel) delegateModel.number = parseFloat(text) 
                        }
                        color: "black"
                        background: Rectangle { color: "white" }
                    }
                    
                    Text {
                        anchors.centerIn: parent
                        visible: nodeType === "NumberDisplay"
                        text: (delegateModel && delegateModel.displayedText !== undefined) ? delegateModel.displayedText : "..."
                        color: "white"
                        font.pixelSize: 20
                    }
                    
                    Text {
                        anchors.centerIn: parent
                        visible: nodeType === "Addition"
                        text: "+"
                        color: "white"
                        font.pixelSize: 40
                        font.bold: true
                    }
                }
            }
        }
    }
}
