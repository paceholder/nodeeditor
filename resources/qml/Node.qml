import QtQuick 2.15

Rectangle {
    id: root
    property var graph
    property int nodeId
    property string nodeType
    property string caption
    property int inPorts
    property int outPorts
    property var delegateModel // QObject* from C++
    property Component contentDelegate
    
    property real initialX
    property real initialY
    
    property bool completed: false
    
    x: initialX
    y: initialY
    
    width: 150
    height: Math.max(Math.max(inPorts, outPorts) * 20 + 40, 50)
    
    color: "#2d2d2d"
    border.color: "black"
    border.width: 2
    radius: 5

    Component.onCompleted: {
        completed = true
    }
    
    DragHandler {
        target: root
    }
    
    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 8
        text: caption
        color: "#eeeeee"
        font.bold: true
    }
    
    Loader {
        id: contentLoader
        anchors.top: parent.top
        anchors.topMargin: 35
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 20
        height: parent.height - 50
        sourceComponent: contentDelegate
        
        onLoaded: {
            if (item) {
                // Use explicit binding objects to ensure updates propagate
                item.delegateModel = Qt.binding(function(){ return root.delegateModel })
                item.nodeType = Qt.binding(function(){ return root.nodeType })
            }
        }

        Connections {
            target: root
            function onDelegateModelChanged() {
                if (contentLoader.item) contentLoader.item.delegateModel = root.delegateModel
            }
            function onNodeTypeChanged() {
                 if (contentLoader.item) contentLoader.item.nodeType = root.nodeType
            }
        }
    }

    // Input Ports
    Column {
        id: inPortsColumn
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 35
        anchors.leftMargin: -5 // Overlap edge
        spacing: 10
        
        Repeater {
            id: inRepeater
            model: inPorts
            delegate: Rectangle {
                width: 12; height: 12
                radius: 6
                color: "green"
                border.color: "black"
                
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: parent.scale = 1.2
                    onExited: parent.scale = 1.0
                    onPressed: {
                        // Handle drop? In ports usually receive connections.
                    }
                }
            }
        }
    }
    
    // Output Ports
    Column {
        id: outPortsColumn
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 35
        anchors.rightMargin: -5
        spacing: 10
        
        Repeater {
            id: outRepeater
            model: outPorts
            delegate: Rectangle {
                width: 12; height: 12
                radius: 6
                color: "orange"
                border.color: "black"
                
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: parent.scale = 1.2
                    onExited: parent.scale = 1.0
                    
                    onPressed: {
                        // Start dragging connection
                        // Notify graph
                    }
                }
            }
        }
    }

    function getPortPos(type, index) {
        var repeater = (type === 0) ? inRepeater : outRepeater
        var portItem = repeater.itemAt(index)
        
        if (portItem) {
            // Map to the graph's canvas (parent's parent usually, but let's be safe)
            // graph.contentItem is the Item inside Flickable.
            // root is inside that Item.
            return root.mapToItem(root.parent, 
                                portItem.x + (type === 0 ? inPortsColumn.x : outPortsColumn.x) + portItem.width/2, 
                                portItem.y + (type === 0 ? inPortsColumn.y : outPortsColumn.y) + portItem.height/2)
        }
        return Qt.point(x, y)
    }
}
