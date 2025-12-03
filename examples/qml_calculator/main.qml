import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtNodes 1.0

Window {
    id: mainWindow
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
    
    // Draggable node item component
    component DraggableNodeButton: Rectangle {
        id: dragButton
        property string nodeType
        property string label
        property color accentColor: darkTheme ? "#888" : "#666"
        
        width: parent.width - 10
        height: 36
        radius: 4
        color: dragArea.containsMouse ? (darkTheme ? "#4a4a4a" : "#d0d0d0") : (darkTheme ? "#3a3a3a" : "#e8e8e8")
        border.color: accentColor
        border.width: 1
        
        Text {
            anchors.centerIn: parent
            text: label
            color: accentColor
            font.pixelSize: 11
            font.bold: true
        }
        
        MouseArea {
            id: dragArea
            anchors.fill: parent
            hoverEnabled: true
            
            property point startPos
            property bool isDragging: false
            
            onPressed: (mouse) => {
                startPos = Qt.point(mouse.x, mouse.y)
                isDragging = false
            }
            
            onPositionChanged: (mouse) => {
                if (pressed) {
                    var delta = Qt.point(mouse.x - startPos.x, mouse.y - startPos.y)
                    if (!isDragging && (Math.abs(delta.x) > 5 || Math.abs(delta.y) > 5)) {
                        isDragging = true
                        dragProxy.nodeType = nodeType
                        dragProxy.label = label
                        dragProxy.accentColor = accentColor
                        dragProxy.visible = true
                    }
                    if (isDragging) {
                        var globalPos = mapToItem(mainWindow.contentItem, mouse.x, mouse.y)
                        dragProxy.x = globalPos.x - dragProxy.width / 2
                        dragProxy.y = globalPos.y - dragProxy.height / 2
                    }
                }
            }
            
            onReleased: (mouse) => {
                if (isDragging) {
                    var globalPos = mapToItem(mainWindow.contentItem, mouse.x, mouse.y)
                    var canvasPos = mapToItem(nodeGraph, mouse.x, mouse.y)
                    
                    // Check if dropped on canvas
                    if (canvasPos.x > 0 && canvasPos.y > 0 && 
                        canvasPos.x < nodeGraph.width && canvasPos.y < nodeGraph.height) {
                        // Convert to canvas coordinates considering zoom and pan
                        var graphPos = nodeGraph.mapToCanvas(canvasPos.x, canvasPos.y)
                        var nodeId = model.addNode(nodeType)
                        if (nodeId >= 0) {
                            model.nodes.moveNode(nodeId, graphPos.x - 75, graphPos.y - 40)
                        }
                    }
                    dragProxy.visible = false
                }
                isDragging = false
            }
        }
    }
    
    // Drag proxy that follows the mouse
    Rectangle {
        id: dragProxy
        visible: false
        width: 120
        height: 36
        radius: 4
        z: 1000
        opacity: 0.8
        
        property string nodeType
        property string label
        property color accentColor
        
        color: darkTheme ? "#3a3a3a" : "#e8e8e8"
        border.color: accentColor
        border.width: 2
        
        Text {
            anchors.centerIn: parent
            text: dragProxy.label
            color: dragProxy.accentColor
            font.pixelSize: 11
            font.bold: true
        }
    }
    
    Row {
        anchors.fill: parent
        
        // Left sidebar with node palette
        Rectangle {
            id: sidebar
            width: 140
            height: parent.height
            color: darkTheme ? "#2d2d2d" : "#f0f0f0"
            
            Column {
                anchors.fill: parent
                spacing: 0
                
                // Top toolbar section
                Rectangle {
                    width: parent.width
                    height: 50
                    color: darkTheme ? "#3c3c3c" : "#e0e0e0"
                    
                    Row {
                        anchors.centerIn: parent
                        spacing: 5
                        
                        Button {
                            width: 36
                            height: 36
                            text: darkTheme ? "☀" : "🌙"
                            onClicked: darkTheme = !darkTheme
                            ToolTip.visible: hovered
                            ToolTip.text: darkTheme ? "Light Theme" : "Dark Theme"
                        }
                        
                        Button {
                            width: 36
                            height: 36
                            text: "↶"
                            enabled: model.canUndo
                            onClicked: model.undo()
                            ToolTip.visible: hovered
                            ToolTip.text: "Undo (Ctrl+Z)"
                        }
                        
                        Button {
                            width: 36
                            height: 36
                            text: "↷"
                            enabled: model.canRedo
                            onClicked: model.redo()
                            ToolTip.visible: hovered
                            ToolTip.text: "Redo (Ctrl+Y)"
                        }
                    }
                }
                
                // Scrollable node palette
                ScrollView {
                    width: parent.width
                    height: parent.height - 50
                    clip: true
                    
                    Column {
                        width: sidebar.width
                        spacing: 5
                        padding: 5
                        
                        // Numbers section
                        Label {
                            text: "NUMBERS"
                            color: darkTheme ? "#888" : "#666"
                            font.bold: true
                            font.pixelSize: 10
                            leftPadding: 5
                            topPadding: 10
                        }
                        
                        DraggableNodeButton {
                            nodeType: "NumberSource"
                            label: "Number"
                            accentColor: "#4CAF50"
                        }
                        
                        DraggableNodeButton {
                            nodeType: "IntegerSource"
                            label: "Integer"
                            accentColor: "#2196F3"
                        }
                        
                        // Math section
                        Label {
                            text: "MATH"
                            color: darkTheme ? "#888" : "#666"
                            font.bold: true
                            font.pixelSize: 10
                            leftPadding: 5
                            topPadding: 10
                        }
                        
                        DraggableNodeButton {
                            nodeType: "Addition"
                            label: "Add (+)"
                            accentColor: darkTheme ? "#aaa" : "#555"
                        }
                        
                        DraggableNodeButton {
                            nodeType: "Subtract"
                            label: "Subtract (−)"
                            accentColor: darkTheme ? "#aaa" : "#555"
                        }
                        
                        DraggableNodeButton {
                            nodeType: "Multiply"
                            label: "Multiply (×)"
                            accentColor: darkTheme ? "#aaa" : "#555"
                        }
                        
                        DraggableNodeButton {
                            nodeType: "Divide"
                            label: "Divide (÷)"
                            accentColor: darkTheme ? "#aaa" : "#555"
                        }
                        
                        // Conversion section
                        Label {
                            text: "CONVERSION"
                            color: darkTheme ? "#888" : "#666"
                            font.bold: true
                            font.pixelSize: 10
                            leftPadding: 5
                            topPadding: 10
                        }
                        
                        DraggableNodeButton {
                            nodeType: "ToInteger"
                            label: "To Integer"
                            accentColor: "#2196F3"
                        }
                        
                        DraggableNodeButton {
                            nodeType: "FormatNumber"
                            label: "Format Text"
                            accentColor: "#FF9800"
                        }
                        
                        // Logic section
                        Label {
                            text: "LOGIC"
                            color: darkTheme ? "#888" : "#666"
                            font.bold: true
                            font.pixelSize: 10
                            leftPadding: 5
                            topPadding: 10
                        }
                        
                        DraggableNodeButton {
                            nodeType: "GreaterThan"
                            label: "A > B"
                            accentColor: "#9C27B0"
                        }
                        
                        // Display section
                        Label {
                            text: "DISPLAY"
                            color: darkTheme ? "#888" : "#666"
                            font.bold: true
                            font.pixelSize: 10
                            leftPadding: 5
                            topPadding: 10
                        }
                        
                        DraggableNodeButton {
                            nodeType: "NumberDisplay"
                            label: "Decimal Display"
                            accentColor: "#4CAF50"
                        }
                        
                        DraggableNodeButton {
                            nodeType: "IntegerDisplay"
                            label: "Integer Display"
                            accentColor: "#2196F3"
                        }
                        
                        DraggableNodeButton {
                            nodeType: "BooleanDisplay"
                            label: "Boolean Display"
                            accentColor: "#9C27B0"
                        }
                        
                        DraggableNodeButton {
                            nodeType: "StringDisplay"
                            label: "Text Display"
                            accentColor: "#FF9800"
                        }
                        
                        // Spacer at bottom
                        Item { width: 1; height: 20 }
                    }
                }
            }
        }
        
        // Main canvas area
        NodeGraph {
            id: nodeGraph
            width: parent.width - sidebar.width
            height: parent.height
            graphModel: model
            style: darkTheme ? darkStyle : lightStyle
            
            // Helper function to convert screen coords to canvas coords
            function mapToCanvas(screenX, screenY) {
                return Qt.point(
                    (screenX - panOffset.x) / zoomLevel,
                    (screenY - panOffset.y) / zoomLevel
                )
            }
            
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
