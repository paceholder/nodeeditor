import QtQuick 2.15
import QtQuick.Shapes 1.15

Shape {
    id: root
    property var graph
    
    // Roles from ConnectionsListModel are expected to be set on this item by the Repeater
    property int sourceNodeId: -1
    property int sourcePortIndex: -1
    property int destNodeId: -1
    property int destPortIndex: -1
    
    property var sourceNode: graph.nodeItems[sourceNodeId]
    property var destNode: graph.nodeItems[destNodeId]
    
    Connections {
        target: graph
        function onNodeRegistryChanged() {
             sourceNode = graph.nodeItems[sourceNodeId]
             destNode = graph.nodeItems[destNodeId]
        }
    }

    // Monitor changes in node position
    Connections {
        target: sourceNode
        function onXChanged() { root.updateStartPos() }
        function onYChanged() { root.updateStartPos() }
    }
    Connections {
        target: destNode
        function onXChanged() { root.updateEndPos() }
        function onYChanged() { root.updateEndPos() }
    }

    // 0 = In, 1 = Out. 
    // Source is Out (1), Dest is In (0).
    property point startPos: Qt.point(0,0)
    property point endPos: Qt.point(0,0)
    
    function updateStartPos() {
        if (sourceNode && sourceNode.completed) {
            startPos = sourceNode.getPortPos(1, sourcePortIndex)
        }
    }
    
    function updateEndPos() {
        if (destNode && destNode.completed) {
            endPos = destNode.getPortPos(0, destPortIndex)
        }
    }
    
    onSourceNodeChanged: updateStartPos()
    onDestNodeChanged: updateEndPos()
    
    Component.onCompleted: {
        updateStartPos()
        updateEndPos()
    }

    visible: sourceNode !== undefined && destNode !== undefined && sourceNode.completed && destNode.completed

    ShapePath {
        strokeWidth: 3
        strokeColor: "#eeeeee"
        fillColor: "transparent"
        
        startX: root.startPos.x
        startY: root.startPos.y
        
        PathCubic {
            x: root.endPos.x
            y: root.endPos.y
            control1X: root.startPos.x + Math.abs(root.endPos.x - root.startPos.x) * 0.5
            control1Y: root.startPos.y
            control2X: root.endPos.x - Math.abs(root.endPos.x - root.startPos.x) * 0.5
            control2Y: root.endPos.y
        }
    }
}
