import QtQuick 2.15
import QtQuick.Shapes 1.15

Shape {
    id: root
    property var graph
    property var modelData // The model roles are available directly in context, but passing 'model' helps sometimes.
    
    // Roles from ConnectionsListModel
    // sourceNodeId, sourcePortIndex, destNodeId, destPortIndex
    
    property var sourceNode: graph.nodeItems[sourceNodeId]
    property var destNode: graph.nodeItems[destNodeId]
    
    Connections {
        target: graph
        function onNodeRegistryChanged() {
             sourceNode = graph.nodeItems[sourceNodeId]
             destNode = graph.nodeItems[destNodeId]
        }
    }

    // 0 = In, 1 = Out. 
    // Source is Out (1), Dest is In (0).
    property point startPos: sourceNode ? sourceNode.getPortPos(1, sourcePortIndex) : Qt.point(0,0)
    property point endPos: destNode ? destNode.getPortPos(0, destPortIndex) : Qt.point(0,0)

    visible: sourceNode !== undefined && destNode !== undefined

    ShapePath {
        strokeWidth: 3
        strokeColor: "black"
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
