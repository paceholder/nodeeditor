import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtNodes 1.0

Window {
    visible: true
    width: 1280
    height: 800
    title: "QML Calculator - Extended"

    property QuickGraphModel model: _graphModel
    property bool darkTheme: true
    
    // Custom dark theme
    NodeGraphStyle {
        id: darkStyle
        canvasBackground: "#1e1e1e"
        gridMinorLine: "#2a2a2a"
        gridMajorLine: "#0f0f0f"
        nodeBackground: "#2d2d2d"
        nodeBorder: "#1a1a1a"
        nodeSelectedBorder: "#4a9eff"
        nodeCaptionColor: "#eeeeee"
        nodeContentColor: "#ffffff"
        connectionSelectionOutline: "#4a9eff"
        selectionRectFill: "#224a9eff"
        selectionRectBorder: "#4a9eff"
    }
    
    // Custom light theme
    NodeGraphStyle {
        id: lightStyle
        canvasBackground: "#f5f5f5"
        gridMinorLine: "#e0e0e0"
        gridMajorLine: "#c0c0c0"
        nodeBackground: "#ffffff"
        nodeBorder: "#cccccc"
        nodeSelectedBorder: "#2196F3"
        nodeCaptionColor: "#333333"
        nodeContentColor: "#333333"
        connectionSelectionOutline: "#2196F3"
        selectionRectFill: "#222196F3"
        selectionRectBorder: "#2196F3"
    }
    
    Column {
        anchors.fill: parent
        
        // Toolbar with node buttons
        Rectangle {
            width: parent.width
            height: 50
            color: darkTheme ? "#3c3c3c" : "#e0e0e0"
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 5
                spacing: 10
                
                Button {
                    text: darkTheme ? "☀ Light" : "🌙 Dark"
                    onClicked: darkTheme = !darkTheme
                }
                
                Rectangle { width: 1; height: 30; color: darkTheme ? "#555" : "#bbb" }
                
                Button {
                    text: "↶ Undo"
                    enabled: model.canUndo
                    onClicked: model.undo()
                }
                Button {
                    text: "↷ Redo"
                    enabled: model.canRedo
                    onClicked: model.redo()
                }
                
                Rectangle { width: 1; height: 30; color: darkTheme ? "#555" : "#bbb" }
                
                Label { 
                    text: "Numbers:" 
                    color: darkTheme ? "#aaa" : "#555"
                    font.bold: true
                }
                Button { 
                    text: "Number" 
                    onClicked: model.addNode("NumberSource")
                    palette.buttonText: "#4CAF50"
                }
                
                Rectangle { width: 1; height: 30; color: darkTheme ? "#555" : "#bbb" }
                
                Label { 
                    text: "Math:" 
                    color: darkTheme ? "#aaa" : "#555"
                    font.bold: true
                }
                Button { text: "Add"; onClicked: model.addNode("Addition") }
                Button { text: "Subtract"; onClicked: model.addNode("Subtract") }
                Button { text: "Multiply"; onClicked: model.addNode("Multiply") }
                Button { text: "Divide"; onClicked: model.addNode("Divide") }
                
                Rectangle { width: 1; height: 30; color: darkTheme ? "#555" : "#bbb" }
                
                Label { 
                    text: "String:" 
                    color: darkTheme ? "#aaa" : "#555"
                    font.bold: true
                }
                Button { 
                    text: "Format" 
                    onClicked: model.addNode("FormatNumber")
                    palette.buttonText: "#FF9800"
                }
                Button { 
                    text: "Text Display" 
                    onClicked: model.addNode("StringDisplay")
                    palette.buttonText: "#FF9800"
                }
                
                Rectangle { width: 1; height: 30; color: darkTheme ? "#555" : "#bbb" }
                
                Label { 
                    text: "Integer:" 
                    color: darkTheme ? "#aaa" : "#555"
                    font.bold: true
                }
                Button { 
                    text: "Int" 
                    onClicked: model.addNode("IntegerSource")
                    palette.buttonText: "#2196F3"
                }
                Button { 
                    text: "To Int" 
                    onClicked: model.addNode("ToInteger")
                    palette.buttonText: "#2196F3"
                }
                
                Rectangle { width: 1; height: 30; color: darkTheme ? "#555" : "#bbb" }
                
                Label { 
                    text: "Logic:" 
                    color: darkTheme ? "#aaa" : "#555"
                    font.bold: true
                }
                Button { 
                    text: "A > B" 
                    onClicked: model.addNode("GreaterThan")
                    palette.buttonText: "#9C27B0"
                }
                
                Rectangle { width: 1; height: 30; color: darkTheme ? "#555" : "#bbb" }
                
                Label { 
                    text: "Display:" 
                    color: darkTheme ? "#aaa" : "#555"
                    font.bold: true
                }
                Button { 
                    text: "Decimal" 
                    onClicked: model.addNode("NumberDisplay")
                    palette.buttonText: "#4CAF50"
                }
                Button { 
                    text: "Int" 
                    onClicked: model.addNode("IntegerDisplay")
                    palette.buttonText: "#2196F3"
                }
                Button { 
                    text: "Bool" 
                    onClicked: model.addNode("BooleanDisplay")
                    palette.buttonText: "#9C27B0"
                }
                
                Item { Layout.fillWidth: true }
            }
        }
        
        NodeGraph {
            id: nodeGraph
            width: parent.width
            height: parent.height - 50
            graphModel: model
            style: darkTheme ? darkStyle : lightStyle
            
            Component.onCompleted: {
                // Create a demo graph: (5 + 3) * 2 = 16, formatted as text
                var num1 = model.addNode("NumberSource")
                var num2 = model.addNode("NumberSource")
                var num3 = model.addNode("NumberSource")
                var add = model.addNode("Addition")
                var mult = model.addNode("Multiply")
                var numDisplay = model.addNode("NumberDisplay")
                var format = model.addNode("FormatNumber")
                var textDisplay = model.addNode("StringDisplay")
                
                if (num1 >= 0) {
                    model.nodes.moveNode(num1, 50, 80)
                    model.nodes.moveNode(num2, 50, 200)
                    model.nodes.moveNode(num3, 50, 350)
                    model.nodes.moveNode(add, 250, 130)
                    model.nodes.moveNode(mult, 450, 200)
                    model.nodes.moveNode(numDisplay, 700, 150)
                    model.nodes.moveNode(format, 700, 280)
                    model.nodes.moveNode(textDisplay, 950, 280)
                    
                    // (num1 + num2) -> add
                    model.addConnection(num1, 0, add, 0)
                    model.addConnection(num2, 0, add, 1)
                    
                    // add * num3 -> mult
                    model.addConnection(add, 0, mult, 0)
                    model.addConnection(num3, 0, mult, 1)
                    
                    // mult -> numDisplay
                    model.addConnection(mult, 0, numDisplay, 0)
                    
                    // mult -> format -> textDisplay
                    model.addConnection(mult, 0, format, 0)
                    model.addConnection(format, 0, textDisplay, 0)
                }
            }
            
            nodeContentDelegate: Component {
                Item {
                    property var delegateModel
                    property string nodeType
                    property var contentColor: nodeGraph.style.nodeContentColor
                    
                    // NumberSource - editable number input
                    TextField {
                        anchors.centerIn: parent
                        width: parent.width
                        visible: nodeType === "NumberSource"
                        text: (delegateModel && delegateModel.number !== undefined) ? delegateModel.number.toString() : "0"
                        onEditingFinished: { 
                            if (delegateModel) delegateModel.number = parseFloat(text) 
                        }
                        onActiveFocusChanged: {
                            if (activeFocus) selectAll()
                        }
                        color: "black"
                        horizontalAlignment: Text.AlignHCenter
                        background: Rectangle { color: "white"; radius: 3 }
                    }
                    
                    // NumberDisplay - shows decimal result
                    Text {
                        anchors.centerIn: parent
                        visible: nodeType === "NumberDisplay"
                        text: (delegateModel && delegateModel.displayedText !== undefined) ? delegateModel.displayedText : "..."
                        color: "#4CAF50"
                        font.pixelSize: 18
                        font.bold: true
                    }
                    
                    // Math operation symbols
                    Text {
                        anchors.centerIn: parent
                        visible: nodeType === "Addition"
                        text: "+"
                        color: contentColor
                        font.pixelSize: 36
                        font.bold: true
                    }
                    
                    Text {
                        anchors.centerIn: parent
                        visible: nodeType === "Subtract"
                        text: "−"
                        color: contentColor
                        font.pixelSize: 36
                        font.bold: true
                    }
                    
                    Text {
                        anchors.centerIn: parent
                        visible: nodeType === "Multiply"
                        text: "×"
                        color: contentColor
                        font.pixelSize: 36
                        font.bold: true
                    }
                    
                    Text {
                        anchors.centerIn: parent
                        visible: nodeType === "Divide"
                        text: "÷"
                        color: contentColor
                        font.pixelSize: 36
                        font.bold: true
                    }
                    
                    // FormatNumber - editable format pattern + preview
                    Column {
                        anchors.fill: parent
                        anchors.margins: 2
                        visible: nodeType === "FormatNumber"
                        spacing: 4
                        
                        TextField {
                            width: parent.width
                            text: (delegateModel && delegateModel.formatPattern !== undefined) ? delegateModel.formatPattern : "Result: %1"
                            onEditingFinished: { 
                                if (delegateModel) delegateModel.formatPattern = text 
                            }
                            onActiveFocusChanged: {
                                if (activeFocus) selectAll()
                            }
                            color: "black"
                            font.pixelSize: 11
                            background: Rectangle { color: "#ffe0b2"; radius: 2 }
                            placeholderText: "Format: %1"
                        }
                        
                        Text {
                            width: parent.width
                            text: (delegateModel && delegateModel.formattedText !== undefined) ? delegateModel.formattedText : ""
                            color: "#FF9800"
                            font.pixelSize: 10
                            elide: Text.ElideRight
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                    
                    // StringDisplay - shows formatted text result
                    Text {
                        anchors.centerIn: parent
                        width: parent.width - 10
                        visible: nodeType === "StringDisplay"
                        text: (delegateModel && delegateModel.displayedText !== undefined) ? delegateModel.displayedText : "..."
                        color: "#FF9800"
                        font.pixelSize: 14
                        font.bold: true
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignHCenter
                    }
                    
                    // IntegerSource - editable integer input
                    TextField {
                        anchors.centerIn: parent
                        width: parent.width
                        visible: nodeType === "IntegerSource"
                        text: (delegateModel && delegateModel.number !== undefined) ? delegateModel.number.toString() : "0"
                        onEditingFinished: { 
                            if (delegateModel) delegateModel.number = parseInt(text) 
                        }
                        onActiveFocusChanged: {
                            if (activeFocus) selectAll()
                        }
                        color: "black"
                        horizontalAlignment: Text.AlignHCenter
                        background: Rectangle { color: "#bbdefb"; radius: 3 }
                        validator: IntValidator {}
                    }
                    
                    // IntegerDisplay
                    Text {
                        anchors.centerIn: parent
                        visible: nodeType === "IntegerDisplay"
                        text: (delegateModel && delegateModel.displayedText !== undefined) ? delegateModel.displayedText : "..."
                        color: "#2196F3"
                        font.pixelSize: 18
                        font.bold: true
                    }
                    
                    // ToInteger - shows conversion result
                    Text {
                        anchors.centerIn: parent
                        visible: nodeType === "ToInteger"
                        text: (delegateModel && delegateModel.resultValue !== undefined) ? "→ " + delegateModel.resultValue : "→ ?"
                        color: "#2196F3"
                        font.pixelSize: 14
                    }
                    
                    // GreaterThan - comparison result
                    Text {
                        anchors.centerIn: parent
                        visible: nodeType === "GreaterThan"
                        text: (delegateModel && delegateModel.resultText !== undefined) ? delegateModel.resultText : "?"
                        color: delegateModel && delegateModel.resultText === "TRUE" ? "#4CAF50" : "#f44336"
                        font.pixelSize: 16
                        font.bold: true
                    }
                    
                    // BooleanDisplay
                    Text {
                        anchors.centerIn: parent
                        visible: nodeType === "BooleanDisplay"
                        text: (delegateModel && delegateModel.displayedText !== undefined) ? delegateModel.displayedText : "..."
                        color: delegateModel && delegateModel.value ? "#4CAF50" : "#f44336"
                        font.pixelSize: 18
                        font.bold: true
                    }
                }
            }
        }
    }
}
