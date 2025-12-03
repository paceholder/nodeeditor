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
    property bool selected: {
        graph.selectionVersion
        return graph.isNodeSelected(nodeId)
    }
    
    // Style shortcut - use direct access for reactivity
    property var style: graph ? graph.style : null
    
    x: initialX
    y: initialY
    
    width: style.nodeMinWidth
    height: Math.max(Math.max(inPorts, outPorts) * (style.portSize + style.nodePortSpacing) + style.nodeHeaderHeight + 5, 50)
    
    color: style.nodeBackground
    border.color: selected ? style.nodeSelectedBorder : style.nodeBorder
    border.width: selected ? style.nodeSelectedBorderWidth : style.nodeBorderWidth
    radius: style.nodeRadius

    Component.onCompleted: {
        completed = true
    }
    
    TapHandler {
        onTapped: (eventPoint, button) => {
            graph.forceActiveFocus()
            var additive = (eventPoint.event.modifiers & Qt.ControlModifier)
            if (additive) {
                graph.toggleNodeSelection(nodeId)
            } else {
                graph.selectNode(nodeId, false)
            }
        }
    }
    
    // Separate handler for pointer press to handle selection on mouse down
    PointHandler {
        id: pointHandler
        acceptedButtons: Qt.LeftButton
        onActiveChanged: {
            if (active) {
                graph.forceActiveFocus()
                var additive = (point.modifiers & Qt.ControlModifier)
                if (additive) {
                    graph.toggleNodeSelection(nodeId)
                } else if (!root.selected) {
                    graph.selectNode(nodeId, false)
                }
            }
        }
    }
    
    DragHandler {
        id: dragHandler
        target: root
        
        property point lastPos: Qt.point(0, 0)
        property bool isDraggingGroup: false
        
        onActiveChanged: {
            if (active) {
                graph.bringToFront(root)
                lastPos = Qt.point(root.x, root.y)
                
                // If this node is selected and there are multiple selections, enable group drag
                isDraggingGroup = root.selected && Object.keys(graph.selectedNodeIds).length > 1
            }
        }
        
        onTranslationChanged: {
            if (isDraggingGroup) {
                var deltaX = root.x - lastPos.x
                var deltaY = root.y - lastPos.y
                
                // Move all other selected nodes by the same delta
                var selectedIds = graph.getSelectedNodeIds()
                for (var i = 0; i < selectedIds.length; i++) {
                    var id = selectedIds[i]
                    if (id !== nodeId) {
                        var node = graph.nodeItems[id]
                        if (node) {
                            node.x += deltaX
                            node.y += deltaY
                        }
                    }
                }
                lastPos = Qt.point(root.x, root.y)
            }
        }
    }
    
    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 8
        text: caption
        color: graph && graph.style ? graph.style.nodeCaptionColor : "#eeeeee"
        font.bold: graph && graph.style ? graph.style.nodeCaptionBold : true
        font.pixelSize: graph && graph.style ? graph.style.nodeCaptionFontSize : 12
    }
    
    Loader {
        id: contentLoader
        anchors.top: parent.top
        anchors.topMargin: style.nodeHeaderHeight
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 20
        height: parent.height - style.nodeHeaderHeight - 15
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
        anchors.topMargin: style.nodeHeaderHeight
        anchors.leftMargin: -style.portSize / 2 + 1
        spacing: style.nodePortSpacing
        
        Repeater {
            id: inRepeater
            model: inPorts
            delegate: Rectangle {
                id: inPortRect
                width: style.portSize; height: style.portSize
                radius: style.portSize / 2
                property string portTypeId: graph.getPortTypeId(root.nodeId, 0, index)
                property bool isCompatible: !graph.isDragging || 
                    (graph.activeConnectionStart && graph.activeConnectionStart.portType === 1 && 
                     graph.draftConnectionTypeId === portTypeId)
                property bool isDimmed: graph.isDragging && !isCompatible
                color: graph.getPortColor(portTypeId)
                opacity: isDimmed ? style.portDimmedOpacity : 1.0
                border.color: isCompatible && graph.isDragging ? style.portHighlightBorder : style.portBorderColor
                border.width: isCompatible && graph.isDragging ? style.portHighlightBorderWidth : style.portBorderWidth
                
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    preventStealing: true
                    onEntered: {
                        if (!graph.isDragging) {
                            parent.scale = style.portHoverScale
                            graph.setActivePort({nodeId: root.nodeId, portType: 0, portIndex: index})
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
                        return ap && ap.nodeId === root.nodeId && ap.portType === 0 && ap.portIndex === index
                    }
                    onIsActiveChanged: parent.scale = isActive ? style.portActiveScale : 1.0
                    onPressed: (mouse) => {
                        var existing = graph.graphModel.getConnectionAtInput(root.nodeId, index)
                        var mousePos = mapToItem(graph.canvas, mouse.x, mouse.y)
                        
                        if (existing.valid) {
                            // Remove existing connection and start draft from source
                            graph.graphModel.removeConnection(existing.outNodeId, existing.outPortIndex,
                                                              root.nodeId, index)
                            var sourceNode = graph.nodeItems[existing.outNodeId]
                            var sourcePos = sourceNode.getPortPos(1, existing.outPortIndex)
                            graph.startDraftConnection(existing.outNodeId, 1, existing.outPortIndex, sourcePos)
                            graph.updateDraftConnection(mousePos)
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
        anchors.topMargin: style.nodeHeaderHeight
        anchors.rightMargin: -style.portSize / 2 + 1
        spacing: style.nodePortSpacing
        
        Repeater {
            id: outRepeater
            model: outPorts
            delegate: Rectangle {
                id: outPortRect
                width: style.portSize; height: style.portSize
                radius: style.portSize / 2
                property string portTypeId: graph.getPortTypeId(root.nodeId, 1, index)
                property bool isCompatible: !graph.isDragging || 
                    (graph.activeConnectionStart && graph.activeConnectionStart.portType === 0 && 
                     graph.draftConnectionTypeId === portTypeId)
                property bool isDimmed: graph.isDragging && !isCompatible
                color: graph.getPortColor(portTypeId)
                opacity: isDimmed ? style.portDimmedOpacity : 1.0
                border.color: isCompatible && graph.isDragging ? style.portHighlightBorder : style.portBorderColor
                border.width: isCompatible && graph.isDragging ? style.portHighlightBorderWidth : style.portBorderWidth
                
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    preventStealing: true
                    onEntered: {
                        if (!graph.isDragging) {
                            parent.scale = style.portHoverScale
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
                    onIsActiveChanged: parent.scale = isActive ? style.portActiveScale : 1.0
                    
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
