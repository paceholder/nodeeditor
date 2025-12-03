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
        z: 10
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
                    preventStealing: true
                    onEntered: {
                        // Only highlight if not dragging or if we are the target
                        if (!graph.isDragging) {
                            parent.scale = 1.2
                            graph.setActivePort({nodeId: root.nodeId, portType: 0, portIndex: index})
                        }
                    }
                    onExited: {
                        if (!graph.isDragging) {
                            parent.scale = 1.0
                            graph.setActivePort(null)
                        }
                    }
                    
                    // Visual feedback based on activePort
                    property bool isActive: {
                        var ap = graph.activePort
                        return ap && ap.nodeId === root.nodeId && ap.portType === 0 && ap.portIndex === index
                    }
                    onIsActiveChanged: parent.scale = isActive ? 1.4 : 1.0
                    onPressed: (mouse) => {
                        var existing = graph.graphModel.getConnectionAtInput(root.nodeId, index)
                        
                        if (existing.valid) {
                            // Remove existing connection and start draft from source
                            graph.graphModel.removeConnection(existing.outNodeId, existing.outPortIndex,
                                                              root.nodeId, index)
                            var sourceNode = graph.nodeItems[existing.outNodeId]
                            var sourcePos = sourceNode.getPortPos(1, existing.outPortIndex)
                            graph.startDraftConnection(existing.outNodeId, 1, existing.outPortIndex, sourcePos)
                        } else {
                            var pos = mapToItem(graph.canvas, width/2, height/2)
                            graph.startDraftConnection(root.nodeId, 0, index, pos)
                        }
                    }
                    onPositionChanged: (mouse) => {
                        var pos = mapToItem(graph.canvas, mouse.x, mouse.y)
                        graph.updateDraftConnection(pos)
                    }
                    onReleased: {
                        graph.endDraftConnection()
                    }
                }
            }
        }
    }
    
    // Output Ports
    Column {
        id: outPortsColumn
        z: 10
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
                    preventStealing: true
                    onEntered: {
                        if (!graph.isDragging) {
                            parent.scale = 1.2
                            graph.setActivePort({nodeId: root.nodeId, portType: 1, portIndex: index})
                        }
                    }
                    onExited: {
                        if (!graph.isDragging) {
                            parent.scale = 1.0
                            graph.setActivePort(null)
                        }
                    }
                    
                    property bool isActive: {
                        var ap = graph.activePort
                        return ap && ap.nodeId === root.nodeId && ap.portType === 1 && ap.portIndex === index
                    }
                    onIsActiveChanged: parent.scale = isActive ? 1.4 : 1.0
                    
                    onPressed: (mouse) => {
                        var pos = mapToItem(graph.canvas, width/2, height/2)
                        graph.startDraftConnection(root.nodeId, 1, index, pos)
                    }
                    onPositionChanged: (mouse) => {
                         var pos = mapToItem(graph.canvas, mouse.x, mouse.y)
                         graph.updateDraftConnection(pos)
                    }
                    onReleased: {
                        graph.endDraftConnection()
                    }
                }
            }
        }
    }

    function getPortInfoAt(x, y) {
        // Map node-local coordinates to find which port is under mouse
        // Check input ports
        var inPos = inPortsColumn.mapFromItem(root, x, y)
        var inChild = inPortsColumn.childAt(inPos.x, inPos.y)
        
        if (inChild) {
             // Find index of this child in the repeater
             for (var i = 0; i < inRepeater.count; ++i) {
                 if (inRepeater.itemAt(i) === inChild) {
                     return {nodeId: root.nodeId, portType: 0, portIndex: i}
                 }
             }
        }
        
        var outPos = outPortsColumn.mapFromItem(root, x, y)
        var outChild = outPortsColumn.childAt(outPos.x, outPos.y)
        
        if (outChild) {
             for (var j = 0; j < outRepeater.count; ++j) {
                 if (outRepeater.itemAt(j) === outChild) {
                     return {nodeId: root.nodeId, portType: 1, portIndex: j}
                 }
             }
        }
        return null
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
