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
            
            Component.onCompleted: {
                var n1 = model.addNode("NumberSource")
                var n2 = model.addNode("NumberSource")
                var n3 = model.addNode("Addition")
                var n4 = model.addNode("NumberDisplay")
                
                if (n1 >= 0 && n2 >= 0 && n3 >= 0 && n4 >= 0) {
                    model.nodes.moveNode(n1, 100, 100)
                    model.nodes.moveNode(n2, 100, 250)
                    model.nodes.moveNode(n3, 400, 175)
                    model.nodes.moveNode(n4, 700, 175)
                    
                    // Connect Source 1 to Addition In 0
                    model.addConnection(n1, 0, n3, 0)
                    // Connect Source 2 to Addition In 1
                    model.addConnection(n2, 0, n3, 1)
                    // Connect Addition Out 0 to Display In 0
                    model.addConnection(n3, 0, n4, 0)
                }
            }
            
            nodeContentDelegate: Component {
                Item {
                    property var delegateModel
                    property string nodeType
                    
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
